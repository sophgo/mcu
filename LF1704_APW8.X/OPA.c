#include <pic16lf1704.h>

#include "OPA.h"

#define OPA_MODE_NULL	0b0
//Op amp operates in high GBWP mode
#define OPA_MODE_SP		0b1	


void OPA_set(unsigned char psel,unsigned char nsel)
{
	OPA1CONbits.OPA1SP = OPA_MODE_SP;//bust bu set
	OPA1CONbits.OPA1UG = nsel;
	OPA1CONbits.OPA1PCH = psel;
	OPA1CONbits.OPA1EN = 1;
}
