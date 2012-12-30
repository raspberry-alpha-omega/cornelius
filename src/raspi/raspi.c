#include <stdint.h>
#include "raspi.h"

void wait(int pause) {
	uint32_t start = GET32(COUNTER);
	uint32_t end = start + pause;
	while (GET32(COUNTER) < end)
		;
}



