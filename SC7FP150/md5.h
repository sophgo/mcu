#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>

struct md5_ctx {
	uint32_t state[4];
	uint32_t count[2];
	uint8_t buffer[64];
};

void md5_init(struct md5_ctx *ctx);
void md5_update(struct md5_ctx *ctx, void *data, unsigned long len);
void md5_final(struct md5_ctx *ctx, void *md5);

#endif
