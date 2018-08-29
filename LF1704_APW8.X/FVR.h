#ifndef __FVR_H__
#define __FVR_H__
// depend on 
// (1)
// HFINTOSC FOSC<2:0> = 100 and IRCF<3:0> != 000x
// (2)
// BOR
// BOREN<1:0> = 11 BOR always enabled
// BOREN<1:0> = 10 and BORFS = 1 BOR disabled in Sleep mode, BOR Fast Start enabled
// BOREN<1:0> = 01 and BORFS = 1 BOR under software control, BOR Fast Start enabled
// (3)
// LDO All PIC16F1704/8 devices, when
// VREGPM = 1 and not in Sleep
// The device runs off of the ULP regulator when in Sleep mode
#define FVR_OFF		0b00
#define FVR_1024	0b01
#define FVR_2048	0b10
#define FVR_4096	0b11
void FVR_set(unsigned char buf1sel,unsigned char buf2sel);

#endif //__FVR_H__
