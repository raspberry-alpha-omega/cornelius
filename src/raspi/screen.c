#include <stdint.h>

#include "raspi.h"
#include "morse.h"
#include "mailbox.h"

#include "screen.h"

#define MailboxBase 0x2000B880
#define Mailbox_READ_REGISTER MailboxBase + 0x00
#define Mailbox_POLL_REGISTER MailboxBase + 0x10
#define Mailbox_SENDER_REGISTER MailboxBase + 0x14
#define Mailbox_STATUS_REGISTER MailboxBase + 0x18
#define Mailbox_CONFIG_REGISTER MailboxBase + 0x1C
#define Mailbox_WRITE_REGISTER MailboxBase + 0x20

/* Framebuffer initialisation failure codes
 * If the FB can't be initialised, one of the following numbers will be
 * flashed on the OK LED
 */

/* Mailbox call to get screen resolution failed */
#define FBFAIL_GET_RESOLUTION		1
/* Mailbox call returned bad resolution */
#define FBFAIL_GOT_INVALID_RESOLUTION	2
/* Mailbox call to setup FB failed */
#define FBFAIL_SETUP_FRAMEBUFFER	3
/* Setup call FB returned an invalid list of response tags */
#define FBFAIL_INVALID_TAGS		4
/* Setup FB call returned an invalid response for the framebuffer tag */
#define FBFAIL_INVALID_TAG_RESPONSE	5
/* Setup FB call returned an invalid address/size */
#define FBFAIL_INVALID_TAG_DATA		6
/* Read FB pitch call returned an invalid response */
#define FBFAIL_INVALID_PITCH_RESPONSE	7
/* Read FB pitch call returned an invalid pitch value */
#define FBFAIL_INVALID_PITCH_DATA	8

/* Character cells are 6x10 */
#define CHARSIZE_X	6
#define CHARSIZE_Y	10

/* Use some free memory in the area below the kernel/stack */
#define BUFFER_ADDRESS	0x1000

/* Screen parameters set in fb_init() */
static unsigned int screenbase, screensize;
static unsigned int fb_x, fb_y, pitch;
/* Max x/y character cell */
static unsigned int max_x, max_y;

/* Framebuffer initialisation failed. Can't display an error, so flashing
 * the OK LED will have to do
 */
static void fb_fail(unsigned int num)
{
	while(1)
		morse_dots(num);
}



void fb_init(void) {
	unsigned int var;
	unsigned int count;
	volatile unsigned int *mailbuffer = (unsigned int *) BUFFER_ADDRESS;

	/* Get the display size */
	mailbuffer[0] = 8 * 4;		// Total size
	mailbuffer[1] = 0;		// Request
	mailbuffer[2] = 0x40003;	// Display size
	mailbuffer[3] = 8;		// Buffer size
	mailbuffer[4] = 0;		// Request size
	mailbuffer[5] = 0;		// Space for horizontal resolution
	mailbuffer[6] = 0;		// Space for vertical resolution
	mailbuffer[7] = 0;		// End tag

	writemailbox(8, (unsigned int)mailbuffer);

	var = readmailbox(8);

	/* Valid response in data structure */
	if(mailbuffer[1] != 0x80000000)
		fb_fail(FBFAIL_GET_RESOLUTION);

	fb_x = mailbuffer[5];
	fb_y = mailbuffer[6];

	if(fb_x==0 || fb_y==0)
		fb_fail(FBFAIL_GOT_INVALID_RESOLUTION);


	/* Set up screen */

	unsigned int c = 1;
	mailbuffer[c++] = 0;		// Request

	mailbuffer[c++] = 0x00048003;	// Tag id (set physical size)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 8;		// Req. + value length (bytes)
	mailbuffer[c++] = fb_x;		// Horizontal resolution
	mailbuffer[c++] = fb_y;		// Vertical resolution

	mailbuffer[c++] = 0x00048004;	// Tag id (set virtual size)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 8;		// Req. + value length (bytes)
	mailbuffer[c++] = fb_x;		// Horizontal resolution
	mailbuffer[c++] = fb_y;		// Vertical resolution

	mailbuffer[c++] = 0x00048005;	// Tag id (set depth)
	mailbuffer[c++] = 4;		// Value buffer size (bytes)
	mailbuffer[c++] = 4;		// Req. + value length (bytes)
	mailbuffer[c++] = 16;		// 16 bpp

	mailbuffer[c++] = 0x00040001;	// Tag id (allocate framebuffer)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 4;		// Req. + value length (bytes)
	mailbuffer[c++] = 16;		// Alignment = 16
	mailbuffer[c++] = 0;		// Space for response

	mailbuffer[c++] = 0;		// Terminating tag

	mailbuffer[0] = c*4;		// Buffer size

	writemailbox(8, (unsigned int)mailbuffer);

	var = readmailbox(8);

	/* Valid response in data structure */
	if(mailbuffer[1] != 0x80000000)
		fb_fail(FBFAIL_SETUP_FRAMEBUFFER);

	count=2;	/* First tag */
	while((var = mailbuffer[count]))
	{
		if(var == 0x40001)
			break;

		/* Skip to next tag
		 * Advance count by 1 (tag) + 2 (buffer size/value size)
		 *                          + specified buffer size
		*/
		count += 3+(mailbuffer[count+1]>>2);

		if(count>c)
			fb_fail(FBFAIL_INVALID_TAGS);
	}

	/* 8 bytes, plus MSB set to indicate a response */
	if(mailbuffer[count+2] != 0x80000008)
		fb_fail(FBFAIL_INVALID_TAG_RESPONSE);

	/* Framebuffer address/size in response */
	screenbase = mailbuffer[count+3];
	screensize = mailbuffer[count+4];

	if(screenbase == 0 || screensize == 0)
		fb_fail(FBFAIL_INVALID_TAG_DATA);

	/* Get the framebuffer pitch (bytes per line) */
	mailbuffer[0] = 7 * 4;		// Total size
	mailbuffer[1] = 0;		// Request
	mailbuffer[2] = 0x40008;	// Display size
	mailbuffer[3] = 4;		// Buffer size
	mailbuffer[4] = 0;		// Request size
	mailbuffer[5] = 0;		// Space for pitch
	mailbuffer[6] = 0;		// End tag

	writemailbox(8, (unsigned int)mailbuffer);

	var = readmailbox(8);

	/* 4 bytes, plus MSB set to indicate a response */
	if(mailbuffer[4] != 0x80000004)
		fb_fail(FBFAIL_INVALID_PITCH_RESPONSE);

	pitch = mailbuffer[5];
	if(pitch == 0)
		fb_fail(FBFAIL_INVALID_PITCH_DATA);

	/* Need to set up max_x/max_y before using console_write */
	max_x = fb_x / CHARSIZE_X;
	max_y = fb_y / CHARSIZE_Y;

//	console_write(COLOUR_PUSH BG_BLUE BG_HALF FG_CYAN
//			"Framebuffer initialised. Address = 0x");
//	console_write(tohex(screenbase, sizeof(screenbase)));
//	console_write(", size = 0x");
//	console_write(tohex(screensize, sizeof(screensize)));
//	console_write(", resolution = ");
//	console_write(todec(fb_x, 0));
//	console_write("x");
//	console_write(todec(fb_y, 0));
//	console_write(COLOUR_POP "\n");
}
//
//
//void MailboxWrite(uint32_t value, uint8_t channel) {
//	value &= 0xFFFFFFF0;
//	channel &= 0x0F;
//
//	uint32_t status;
//	do {
//		status = GET32(Mailbox_STATUS_REGISTER);
//	} while (status & 0x80000000);
//
//	PUT32(Mailbox_WRITE_REGISTER, value + channel);
//}
//
//uint32_t MailboxRead(uint8_t channel) {
//	channel &= 0x0F;
//
//	uint32_t status;
//	uint32_t ret;
//
//	do {
//		do {
//			morse_char('a');
//			status = GET32(Mailbox_STATUS_REGISTER);
//		} while (status & 0x40000000);
//
//		morse_char('b');
//		ret = GET32(Mailbox_READ_REGISTER);
//	} while (channel != (ret & 0x0F));
//
//	return ret & 0xFFFFFFF0;
//}
//
//struct FrameBufferInfo fbinfo __attribute__ ((aligned(16))) = {
//		/* 00: int width */ 1024,
//		/* 04: int height */ 768,
//		/* 08: int vwidth */ 1024,
//		/* 0C: int vheight */ 768,
//		/* 10: int pitch */ 0,
//		/* 14: int depth */ 16,
//		/* 18: int x */ 0,
//		/* 1C: int y */ 0,
//		/* 20: int buffer_p */ 0,
//		/* 24: int buffer_size */ 0
//};
//
//#define CHANNEL 1
//
//int init_fb(uint32_t width, uint32_t height, uint32_t depth) {
//	//We need to put the frame buffer structure somewhere with the lower 4 bits zero.
//	//2^22 is a convenient place not used by anything, and with sufficient alignment
//	volatile struct FrameBufferInfo *fb = (volatile struct FrameBufferInfo *)(1 << 22);
//	//See the comments for Bcm2835FrameBuffer
//	fb->width = width;
//	fb->height = height;
//	fb->vwidth = fb->width;
//	fb->vheight = fb->height;
//	fb->pitch = 0;
//	fb->depth = depth;
//	fb->x = 0;
//	fb->y = 0;
//	fb->buffer_p = 0;
//	fb->buffer_size = 0;
//
//	MailboxWrite(ArmToVc(&fbinfo), CHANNEL);
//	wait(10 * MSECOND);
//	uint32_t r = MailboxRead(CHANNEL);
//	morse_char('f');
//
//	if (r){ //If r != 0, some kind of error occurred
//		morse_char('m');
//		return -1;
//	}
//
//	if (!fb->buffer_p){ //if the frame buffer pointer is zero, an error occurred
//		morse_char('n');
//		return -2;
//	}
//
//	//Set up our frame buffer information
//	fbinfo.width = fb->width;
//	fbinfo.height = fb->height;
//	fbinfo.pitch = fb->pitch;
//	fbinfo.buffer_p = fb->buffer_p;
//
//	return 0;
//}
//
//struct FrameBufferInfo* InitialiseFrameBuffer(uint32_t width, uint32_t height, uint32_t depth) {
//	while (init_fb(width, height, depth)) {
//		morse_char('g');
//	}
////	uint32_t ret = 0;
////	do {
////		fbinfo.vwidth = width;
////		fbinfo.height = height;
////		fbinfo.vwidth = width;
////		fbinfo.vheight = height;
////		fbinfo.depth = bitDepth;
////
////		MailboxWrite((uint32_t)&fbinfo, CHANNEL);
////		wait(10 * MSECOND);
////		ret = MailboxRead(CHANNEL);
////		morse_char('f');
////	} while (ret == 0);
////
////	while (GET32(fbinfo.buffer_p + 0x20) == 0) {
////		morse_char('e');
////	}
////
////	fbinfo.buffer_p = GET32(fbinfo.buffer_p + 0x20);
////	fbinfo.buffer_size = GET32(fbinfo.buffer_p + 0x24);
////
//	morse_char('h');
//	return (struct FrameBufferInfo*)&fbinfo;
//}
