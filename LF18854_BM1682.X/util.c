#include "util.h"

int asc2hex(unsigned char *phex,unsigned char *pasc,int len)
{
	int i;
	unsigned char asc;
	for (i = 0; i < len; ++i,++pasc)
	{
		asc = ((*pasc)>>4)&0x0F;
		*phex++ = (asc <= 9) ? asc+'0' : asc-0x0a+'A';
		
		asc = (*pasc)&0x0F;
		*phex++ = (asc <= 9) ? asc+'0' : asc-0x0a+'A';
	}
	return len*2;
}
unsigned char  hex2asc(unsigned char *pasc,unsigned char *phex,int len)
{
	int ret;
	int i;
	unsigned char hex;
	ret = len/2;
	if (len%2)
	{
		ret++;
		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc =  hex - '0';
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc =  hex - 'A' + 0x0A;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc =  hex - 'a' + 0x0A;
		}
		else
		{
		}
		pasc++;
		
	}
	while (len)
	{
		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc =  hex - '0';
			*pasc <<= 4;
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc =  hex - 'A' + 0x0A;
			*pasc <<= 4;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc =  hex - 'a' + 0x0A;
			*pasc <<= 4;
		}
		else
		{
		}

		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc |=  hex - '0';
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc |=  hex - 'A' + 0x0A;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc |=  hex - 'a' + 0x0A;
		}
		else
		{
		}
		pasc++;
	}
	return ret;
}

