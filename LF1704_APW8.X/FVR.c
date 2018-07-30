#include "FVR.h"
#include <xc.h>

void FVR_set(unsigned char buf1sel,unsigned char buf2sel)
{
    
	FVRCONbits.CDAFVR	= buf2sel;// FVR BUFFER2 VCDAFVR is 2.048
	FVRCONbits.ADFVR	= buf1sel;// FVR BUFFER1 ADC FVR Buffer is off
	FVRCONbits.FVREN	= 1;
	while (FVRCONbits.FVRRDY == 0)
	{
	}	
}
