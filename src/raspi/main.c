#include <stdint.h>

#include "corn.h"
#include "screen.h"
#include "morse.h"
#include "raspi.h"

void flash_ok(int pause) {
	struct Crate* crate;
	doorid_t led = dlookup("RasPi/OKLED");

	crate = dopen(led);
	crate->data[0] = 1;
	dclose(led);

	wait(pause);

	crate = dopen(led);
	crate->data[0] = 0;
	dclose(led);

	wait(pause);
}


#define RED   0b1111100000000000
#define GREEN 0b0000011111100000
#define BLUE  0b0000000000011111

void set_pixel(struct FrameBufferInfo* fbinfo, int x, int y, uint16_t hue) {
	uint32_t fb = fbinfo->buffer_p;
	PUT16(fb + (y * fbinfo->width) + x, hue);
}

void set_screen_colour(struct FrameBufferInfo* fbinfo, uint16_t hue) {
	for (int i = 0; i < fbinfo->buffer_size; ++i) {
		PUT16(fbinfo->buffer_p + i, hue);
	}
}

void not_main(void) {
//	struct FrameBufferInfo* fbinfo = InitialiseFrameBuffer(1024, 768, 16);
//
//	if (0 != fbinfo && 0 != fbinfo->buffer_p) {
//		uint16_t hue = RED;
//		for (int i = 0; i < fbinfo->buffer_size; ++i) {
//			PUT16(fbinfo->buffer_p + i, hue);
//		}
//
//		for (;;) {
//			flash_ok(1000000);
//		}
//	} else {
//		for (;;) {
//			flash_ok(5000000);
//		}
//	}

	for (;;) {
		morse_string("frank ");
//		morse_glyph('-');
//		morse_char('o');
//		morse_char('s');
//		morse_char(' ');
	}
}
