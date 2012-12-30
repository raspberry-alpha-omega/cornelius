#include <stdint.h>
#include "corn.h"
#include "raspi.h"

#define MAX_DOORS 8

struct Room {

};

struct Door {
	const char* name;
	uint16_t size;
	void (*reset)(doorid_t id, uint16_t size);
	void (*action)(doorid_t id, uint16_t size, struct Crate* crate);
	struct Crate* a;
	struct Crate* b;
};

#define OKLED (1<<16)
void raspi_okled_fn(doorid_t id, uint16_t size, struct Crate* crate) {
	if (crate->data[0] == 0) {
		PUT32(GPCLR0, OKLED);
	} else {
		PUT32(GPSET0, OKLED);
	}
}
void raspi_okled_init(doorid_t id, uint16_t size) {
	uint32_t sel = GET32(GPFSEL1);
	sel &= ~(0b111 << 18);
	sel |= (0b001 << 18);
	PUT32(GPFSEL1,sel);
}
uint8_t raspi_okled_a[sizeof(struct CrateHeader) + 1];
uint8_t raspi_okled_b[sizeof(struct CrateHeader) + 1];

struct Door doors[MAX_DOORS] = {
		{
				"RasPi/OKLED",
				1,
				&raspi_okled_init,
				&raspi_okled_fn,
				(struct Crate*)&raspi_okled_a,
				(struct Crate*)&raspi_okled_b
		},
};
int ndoors = 1;

void setup_doors() {

}

void setup_rooms() {

}

extern void not_main();

__attribute__((no_instrument_function))  void os_init(void) {
	setup_doors();
	setup_rooms();

	not_main();
}

int streq(const char* a, const char* b) {
	if (0 == a && 0 == b) return 1;
	if (0 == a || 0 == b) return 0;
	for (;;) {
		if (*a != *b) return 0;
		if (0 == *a) return 1;
	}

	// should never get here
	return 0;
}

doorid_t dlookup(const char* dname) {
//	for (int i = 0; i < ndoors; ++i) {
//		if (streq(dname, doors[i].name)) return i;
//	}
//	return DOOR_ERROR;
	return 0;
}

void dreset(doorid_t id) {
	doors[id].reset(id, doors[id].size);
}

extern struct Crate* dopen(doorid_t id) {
	return doors[id].a;
}

extern void dclose(doorid_t id) {
	struct Door* door = &doors[id];
	door->action(id, door->size, door->a);
	struct Crate* t = door->a;
	door->a = door->b;
	door->b = door->a;
}
