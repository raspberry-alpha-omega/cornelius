#include <stdint.h>

#include "corn.h"
#include "screen.h"

void wait(doorid_t door, uint32_t pause) {
	struct Crate* crate = dopen(door);
	*(int32_t*)crate->data = pause;
	dclose(door);
}

void not_main(void) {
	struct Crate* crate;
	doorid_t led = dlookup("RasPi/OKLED");
	doorid_t timer = dlookup("RasPi/TIMER");

	if (DOOR_ERROR == led || DOOR_ERROR == timer) return;

	struct FrameBufferInfo* fbinfo = InitialiseFrameBuffer(1024, 768, 16);

	int pause = 1000000;

	for (;;) {
		crate = dopen(led);
		crate->data[0] = 1;
		dclose(led);

		wait(timer, pause);

		crate = dopen(led);
		crate->data[0] = 0;
		dclose(led);

		wait(timer, pause);
	}
}
