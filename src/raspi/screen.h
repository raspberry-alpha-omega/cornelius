#ifndef SCREEN_H
#define SCREEN_H

#define RED   0b1111100000000000
#define GREEN 0b0000011111100000
#define BLUE  0b0000000000011111

extern void fb_init(void);
extern void clear_screen(int hue);
extern void setpixel(int x, int y, int hue);

#endif
