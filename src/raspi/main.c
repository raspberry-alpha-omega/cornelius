#include <stdint.h>
#include "corn.h"

struct FBDescription {
	int width;
	int height;
	int vwidth;
	int vheight;
	int pitch;
	int bitdepth;
	int x;
	int y;
	int gpointer;
	int gsize;
};

//extern struct FBDescription* InitialiseFrameBuffer(uint32_t width, uint32_t height, uint32_t bitDepth);
//void SetGraphicsAddress(struct FBDescription* fbd);
//extern uint64_t DrawString(char* string, uint32_t length, uint32_t x, uint32_t y);
//
//extern void SetForeColour(uint16_t colour);
//extern void DrawPixel(uint32_t x, uint32_t y);
//extern void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

void not_main(void) {
//	struct FBDescription* fbd = InitialiseFrameBuffer(1024, 768, 16);
//	SetGraphicsAddress(fbd);
//	DrawString("Hello, World", 12, 0, 0);
//	SetForeColour(5555);
//	DrawLine(0,0,1000,1000);

	struct Crate* crate;
	doorid_t led = dlookup("RasPi/OKLED");
	doorid_t timer = dlookup("RasPi/TIMER");
	if (DOOR_ERROR == led || DOOR_ERROR == timer) return;

	int pause = 1000000;

	for (;;) {
		crate = dopen(led);
		crate->data[0] = 1;
		dclose(led);

		crate = dopen(timer);
		*(int32_t*)crate->data = pause;
		dclose(timer);

		crate = dopen(led);
		crate->data[0] = 0;
		dclose(led);

		crate = dopen(timer);
		*(int32_t*)crate->data = pause;
		dclose(timer);
	}
}
