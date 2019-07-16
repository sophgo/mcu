#define isxdigit(c)	(('0' <= (c) && (c) <= '9') \
			 || ('a' <= (c) && (c) <= 'f') \
			 || ('A' <= (c) && (c) <= 'F'))

int asc2hex(unsigned char *phex,unsigned char *pasc,int len);
unsigned char  hex2asc(unsigned char *pasc,unsigned char *phex,int len);


