#ifndef __OPA_H__
#define __OPA_H__
#define OPA_PSEL_IN_P		0b01
#define OPA_PSEL_DAC_OUT	0b10
#define OPA_PSEL_FVR_BUF2	0b11

#define OPA_NSEL_IN_N		0b0
#define OPA_NSEL_OUT		0b1

void OPA_set(unsigned char psel,unsigned char nsel);

#endif	//__OPA_H__
