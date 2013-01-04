#include <stdint.h>

#include "corn.h"
#include "screen.h"
#include "morse.h"
#include "raspi.h"

void not_main(void) {
	fb_init();

	clear_screen(GREEN);
	halt("a");
}
