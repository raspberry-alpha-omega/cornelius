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

#define READ32(addr) (*(VUINT32*)(addr))
#define READ16(addr) (*(VUINT16*)(addr))
#define READ8(addr) (*(VUINT8*)(addr))

#define WRITE32(addr,value) (*(VUINT32*)(addr) = (VUINT32)(value))
#define WRITE16(addr,value) (*(VUINT16*)(addr) = (VUINT16)(value))
#define WRITE8(addr,value) (*(VUINT8*)(addr) = (VUINT8)(value))

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

#endif
