#include <stdint.h>
#include "corn.h"
#include "raspi.h"

struct Room {

};

#define DOOR_STATUS_RAW 0
#define DOOR_STATUS_READY 1
#define DOOR_STATUS_BUSY 2

struct Door {
	volatile uint8_t status;
	const char* name;
	uint16_t size;
	void (*reset)(doorid_t id);
	void (*action)(doorid_t id, uint16_t size, struct Crate* crate);
	struct Crate* a;
	struct Crate* b;
};

extern struct Door doors[];

#define OKLED (1<<16)
void raspi_okled_action(doorid_t id, uint16_t size, struct Crate* crate) {
	if (crate->data[0] == 1) {
		PUT32(GPCLR0, OKLED);
	} else {
		PUT32(GPSET0, OKLED);
	}
}

void raspi_okled_reset(doorid_t id) {
	doors[id].status = DOOR_STATUS_BUSY;
	uint32_t sel = GET32(GPFSEL1);
	sel &= ~(0b111 << 18);
	sel |= (0b001 << 18);
	PUT32(GPFSEL1,sel);
	doors[id].status = DOOR_STATUS_READY;
}
uint8_t raspi_okled_a[sizeof(struct CrateHeader) + sizeof(uint8_t)];
uint8_t raspi_okled_b[sizeof(struct CrateHeader) + sizeof(uint8_t)];

void raspi_timer_action(doorid_t id, uint16_t size, struct Crate* crate) {
	doors[id].status = DOOR_STATUS_BUSY;
	uint32_t start = GET32(COUNTER);
	uint32_t end = start + *(int32_t*)crate->data;
	while (GET32(COUNTER) < end)
		;
	doors[id].status = DOOR_STATUS_READY;
}
void raspi_timer_reset(doorid_t id) {
	doors[id].status = DOOR_STATUS_READY;
}
uint8_t raspi_timer_a[sizeof(struct CrateHeader) + sizeof(int32_t)];
uint8_t raspi_timer_b[sizeof(struct CrateHeader) + sizeof(int32_t)];

struct Door doors[] = {
		{
				DOOR_STATUS_RAW,
				"RasPi/OKLED",
				sizeof(uint8_t),
				&raspi_okled_reset,
				&raspi_okled_action,
				(struct Crate*)&raspi_okled_a,
				(struct Crate*)&raspi_okled_b
		},
		{
				DOOR_STATUS_RAW,
				"RasPi/TIMER",
				sizeof(int32_t),
				&raspi_timer_reset,
				&raspi_timer_action,
				(struct Crate*)&raspi_timer_a,
				(struct Crate*)&raspi_timer_b
		}
};
int ndoors = 2;

void setup_doors() {
	for (int i = 0; i < ndoors; ++i) {
		doors[i].reset(i);
	}
}

void setup_rooms() {

}

extern void not_main();

void yield() {
	; // do nothing in single-process mode
}

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
		if (0 == *a) break;
		++a;
		++b;
	}

	// should never get here
	return 1;
}

doorid_t dlookup(const char* dname) {
	for (int i = 0; i < ndoors; ++i) {
		if (streq(dname, doors[i].name)) return i;
	}
	return DOOR_ERROR;
}

extern struct Crate* dopen(doorid_t id) {
	struct Door* door = &doors[id];
	while (door->status != DOOR_STATUS_READY) {
		yield();
	}
	return doors[id].a;
}

extern void dclose(doorid_t id) {
	struct Door* door = &doors[id];

	doors[id].status = DOOR_STATUS_BUSY;
	struct Crate* t = door->a;
	door->a = door->b;
	door->b = t;
	doors[id].status = DOOR_STATUS_READY;

	// in single-process mode this happens inline here. for multi-process, expect a yield.
	door->action(id, door->size, door->b);
}
