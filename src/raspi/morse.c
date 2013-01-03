#include "corn.h"

struct Morse { char letter; const char* symbol; } code[] = {
		{ 'a', ".-" },
		{ 'b', "-..." },
		{ 'c', "-.-." },
		{ 'd', "-.." },
		{ 'e', "." },
		{ 'f', "..-." },
		{ 'g', "--." },
		{ 'h', "...." },
		{ 'i', ".." },
		{ 'j', ".---" },
		{ 'k', "-.-" },
		{ 'l', ".-.." },
		{ 'm', "--" },
		{ 'n', "-." },
		{ 'o', "---" },
		{ 'p', ".--." },
		{ 'q', "--.-" },
		{ 'r', ".-." },
		{ 's', "..." },
		{ 't', "-" },
		{ 'u', "..-" },
		{ 'v', "...-" },
		{ 'w', ".--" },
		{ 'x', "-..-" },
		{ 'y', "-.--" },
		{ 'z', "--.." },
		{ ' ', " " },
		{ '0', "-----" },
		{ '1', ".----" },
		{ '2', "..---" },
		{ '3', "...--" },
		{ '4', "....-" },
		{ '5', "....." },
		{ '6', "-...." },
		{ '7', "--..." },
		{ '8', "---.." },
		{ '9', "----." }
};
static int nchars = sizeof(code) / sizeof (struct Morse);

#define dot_pause 500000
#define dash_pause (dot_pause * 3)
#define gap_pause 500000
#define letter_pause (gap_pause * 3)
#define space_pause (gap_pause * 6)

void wait(int pause) {
	doorid_t timer = dlookup("RasPi/TIMER");
	struct Crate* crate = dopen(timer);
	*(int32_t*)crate->data = pause;
	dclose(timer);
}

void switch_off(doorid_t led, int duration) {
	struct Crate* crate = dopen(led);
	crate->data[0] = 1;
	dclose(led);

	wait(duration);
}

void switch_on(doorid_t led, int duration) {
	struct Crate* crate = dopen(led);
	crate->data[0] = 0;
	dclose(led);

	wait(duration);
}

void morse_glyph(char c) {
	doorid_t led = dlookup("RasPi/OKLED");
	switch(c) {
	case '.':
		switch_on(led, dot_pause);
		break;
	case '-':
		switch_on(led, dash_pause);
		break;
	default:
		wait(space_pause);
		break;
	}

	switch_off(led, gap_pause);
}

void morse_symbol(const char* symbol) {
	struct Crate* crate;

	for (char c = *symbol; c != 0; c = *++symbol) {
		morse_glyph(c);
	}

	wait(letter_pause);
}

void morse_char(char c) {
	for (int i = 0; i < nchars; ++i) {
		if (c == code[i].letter) {
			morse_symbol(code[i].symbol);
			return;
		}
	}
}

void morse_string(const char* message) {
	for (char c = *message; c != 0; c = *++message) {
		morse_char(c);
	}
}
