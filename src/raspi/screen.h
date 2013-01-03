#ifndef SCREEN_H
#define SCREEN_H

struct FrameBufferInfo {
	int width;
	int height;
	int vwidth;
	int vheight;
	int pitch;
	int depth;
	int x;
	int y;
	int gpu_p;
	int gpu_size;
};

void MailboxWrite(uint32_t value, uint8_t channel);
uint32_t MailboxRead(uint8_t channel);
struct FrameBufferInfo* InitialiseFrameBuffer(uint32_t width, uint32_t height, uint32_t bitDepth);

#endif
