#ifndef SCREEN_H
#define SCREEN_H

//struct FrameBufferInfo {
//	uint32_t width;
//	uint32_t height;
//	uint32_t vwidth;
//	uint32_t vheight;
//	uint32_t pitch;
//	uint32_t depth;
//	int32_t x;
//	int32_t y;
//	uint32_t buffer_p;
//	uint32_t buffer_size;
//};
//
//void MailboxWrite(uint32_t value, uint8_t channel);
//uint32_t MailboxRead(uint8_t channel);
//struct FrameBufferInfo* InitialiseFrameBuffer(uint32_t width, uint32_t height, uint32_t bitDepth);

extern void fb_init(void);
extern void clear_screen(int hue);

#endif
