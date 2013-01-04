#ifndef MORSE_H
#define MORSE_H

#include "corn.h"

void switch_on(doorid_t led);
void switch_off(doorid_t led);
void wait(int pause);

void morse_glyph(char glyph);
void morse_symbol(const char* symbol);
void morse_dots(int ndots);
void morse_char(char c);
void morse_string(const char* message);

#endif
