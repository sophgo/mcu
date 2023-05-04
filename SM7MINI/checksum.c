#include <stdint.h>
#include <string.h>

void checksum(void *out, void *in, unsigned long len)
{
	uint32_t *src = in;
	char *init = "*BITMAIN-SOPHON*";
	uint32_t result[4];
	unsigned long block = len >> 4;
	unsigned long left = len & 0xf;
	unsigned long i, j;

	memcpy(result, init, sizeof(result));


	for (i = 0; i < block; ++i, src += 4)
		for (j = 0; j < 4; ++j)
			result[j] ^= src[j];

	for (i = 0; i < left; ++i)
		((uint8_t *)result)[i] ^= ((uint8_t *)src)[i];

	memcpy(out, result, sizeof(result));
}
