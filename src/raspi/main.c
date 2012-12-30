#include <stdint.h>
#include "corn.h"

void not_main(void) {
	struct Crate* crate;
	doorid_t door = dlookup("RasPi/OKLED");
	if (DOOR_ERROR == door) return;

	dreset(door);
	int pause = 1000000;

	for (;;) {
		crate = dopen(door);
		crate->data[0] = 1;
		dclose(door);
		wait(pause);

		crate = dopen(door);
		crate->data[0] = 0;
		dclose(door);
		wait(pause);
	}
}
