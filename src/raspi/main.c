#include <stdint.h>

#include "corn.h"
#include "screen.h"

void not_main(void) {

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
