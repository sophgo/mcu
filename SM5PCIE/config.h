#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CHANNEL_NUMBER		(6)
/* sample rate 10K */
#define SAMPLE_RATE		(10 * 1000)
#define BITS_PER_SAMPLE		(8)

#define CTRL_START_SECTOR	(0)
#define INFO_START_SECTOR	(1)
#define DATA_START_SECTOR	(2)
#define SECTOR_SIZE		(512)

/* SIG_BUF_SIZE / 2 must CHANNEL_NUMBER aligned */
/* we should log current and voltage for power calculation */
#define SIG_BUF_SIZE		(15 * (2 * SECTOR_SIZE))

#define CURRENT_SCALE		(11000)
#define VOLTAGE_SCALE		(36300)

#define CHANNEL_LABELS	\
	"PCIE-12V-Current,scale:11\0"	\
	"PCIE-12V,scale:36.3\0"	\
	"PCIE-3.3V,scale:3.3\0"	\
	"Clock,scale:3.3\0"	\
	"MCU-3.3v,scale:3.3\0"	\
	"PCIE-Reset,scale:3.3\0"

/* 0xffffffff means infinity */
#define DEFAULT_ACQUIRE_TIME	(0xffffffff)

#endif
