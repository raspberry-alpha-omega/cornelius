#include <stdint.h>

#include "raspi.h"
#include "screen.h"

#define MailboxBase 0x2000B880
#define Mailbox_READ_REGISTER MailboxBase + 0x00
#define Mailbox_POLL_REGISTER MailboxBase + 0x10
#define Mailbox_SENDER_REGISTER MailboxBase + 0x14
#define Mailbox_STATUS_REGISTER MailboxBase + 0x18
#define Mailbox_CONFIG_REGISTER MailboxBase + 0x1C
#define Mailbox_WRITE_REGISTER MailboxBase + 0x20

void MailboxWrite(uint32_t value, uint8_t channel) {
	value &= 0xFFFFFFF0;
	channel &= 0x0F;

	uint32_t status;
	do {
		status = GET32(Mailbox_STATUS_REGISTER);
	} while (status & 0x80000000);

	PUT32(Mailbox_WRITE_REGISTER, value + channel);
}

uint32_t MailboxRead(uint8_t channel) {
	channel &= 0x0F;

	uint32_t status;
	uint32_t ret;

	do {
		do {
			status = GET32(Mailbox_STATUS_REGISTER);
		} while (status & 0x40000000);

		ret = GET32(Mailbox_READ_REGISTER);
	} while (channel != (ret & 0x0F));

	return ret & 0xFFFFFFF0;
}

volatile struct FrameBufferInfo fbinfo __attribute__ ((aligned(16))) = {
		/* int width */ 1024,
		/* int height */ 768,
		/* int vwidth */ 1024,
		/* int vheight */ 768,
		/* int pitch */ 0,
		/* int depth */ 16,
		/* int x */ 0,
		/* int y */ 0,
		/* int buffer_p */ 0,
		/* int buffer_size */ 0
};

#define CHANNEL 1

struct FrameBufferInfo* InitialiseFrameBuffer(uint32_t width, uint32_t height, uint32_t bitDepth) {
	fbinfo.vwidth = width;
	fbinfo.height = height;
	fbinfo.vwidth = width;
	fbinfo.vheight = height;
	fbinfo.depth = bitDepth;

	MailboxWrite((uint32_t)&fbinfo, CHANNEL);
	uint32_t ret = MailboxRead(CHANNEL);
	if (0 == ret) return 0;

	while (fbinfo.buffer_p == 0)
		;

	return (struct FrameBufferInfo*)&fbinfo;
}
