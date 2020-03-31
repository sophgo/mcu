#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CHANNEL_NUMBER		(6)
/* sample rate 10K */
#define SAMPLE_RATE		(10 * 1000)
#define BITS_PER_SAMPLE		(8)

#define DATA_START_SECTOR	(2)
#define SECTOR_SIZE		(512)

#define SIG_BUF_SIZE		(16 * (2 * SECTOR_SIZE))

#define CHANNEL_LABELS	\
	"12v-current\0"	\
	"12v-voltage\0"	\
	"pcie-3.3v\0"	\
	"clock\0"	\
	"mcu-io-3.3v\0"	\
	"pcie-reset\0"

/* 0xffffffff means infinity */
#define DEFAULT_ACQUIRE_TIME	(0xffffffff)

#endif
