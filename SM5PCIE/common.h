#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#ifndef __packed
#define __packed __attribute__((packed))
#endif

/* bitmain pcie debugger */
#define MAGIC		"BMPD"
#define FMT_VER		1

struct ctrl_block {
	uint8_t magic[4];
	uint32_t version;
	uint32_t time;
	char padding[500];
} __packed;

struct info_block {
	uint32_t channels;
	uint32_t sample_rate;
	uint32_t bits_per_sample;
	uint32_t data_offset;
	uint64_t samples;
	/* each name terminated with 0 */
	char channel_labels[488];
} __packed;

#endif
