#ifndef raspi_h
#define raspi_h

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028
#define TIMER_  0x20003000
#define COUNTER 0x20003004

#define NULL				0
#define TRUE				1
#define FALSE				0

#define ABS(a)				((a)>0?(a):-(a))
#define MIN(a,b)			((a)<(b)?(a):(b))
#define MAX(a,b)			((a)>(b)?(a):(b))
#define CLAMP(min,x,max)	((x)>(max)?(max):((x)<(min)?(min):(x)))

#define SET_BITS(data, mask)	((data)|=(mask))
#define CLEAR_BITS(data, mask)	((data)&=(~(mask)))
#define CHANGE_BITS(data, mask)	((data)^=(mask))
#define TEST_BITS(data, mask)	(((data)&(mask)) != 0)
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

extern void PUT32(uint32_t p, uint32_t value);
extern void PUT16(uint32_t p, uint16_t value);
extern void PUT8(uint32_t p, uint8_t value);
extern unsigned int GET32(uint32_t p);
extern unsigned int GET16(uint32_t p);
extern unsigned int GET8(uint32_t p);

#define SECOND 1000000
#define MINUTE (60 * SECOND)

#endif
