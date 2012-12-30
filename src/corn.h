#ifndef CORN_H_
#define CORN_H_

#include <stdint.h>

struct CrateHeader {
	uint16_t status;
	uint16_t length;
};

struct Crate {
	struct CrateHeader header;
	uint8_t data[];
};

typedef uint16_t doorid_t;
#define DOOR_ERROR 65535

doorid_t dlookup(const char* dname);
extern struct Crate* dopen(doorid_t id);
extern void dclose(doorid_t id);

void yield();

#endif
