/*
 * ds1307.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */
#include <assert.h>
#include "main.h"
#include "i2c_bm.h"
#include "rtc.h"

struct ds1307_map {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t control;
	uint8_t ram[0x38];
};

static struct ds1307_ctx {
	int set_idx;
	int set_rtc;
	int idx;
	struct ds1307_map map;
} ds1307_ctx;

static void ds1307_match(int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		ds1307_ctx.set_idx = 1;
	} else {
		/* update ds1307 shadow registers */
		/* stm32 rtc always working on 24h mode */
		RTC_TimeTypeDef time;
		HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
		struct ds1307_map *map = &ds1307_ctx.map;
		map->seconds = time.Seconds | (map->seconds & 0x80); /* CH bit remained */
		map->minutes = time.Minutes;
		if (map->hours & (1 << 6)) {
			/* 12 hours mode */
			int hour_binary = RTC_Bcd2ToByte(time.Hours);
			int pm = 0;
			if (hour_binary >= 12) {
				/* pm */
				pm = 1;
				hour_binary -= 12;
			}
			hour_binary = hour_binary ?: 12;
			map->hours = (1 << 6) | RTC_ByteToBcd2(hour_binary) | (pm << 5);
		} else {
			/* 24 hours mode */
			map->hours = time.Hours;
		}
		/* get date */
		RTC_DateTypeDef date;
		HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
		map->day = date.WeekDay;
		map->date = date.Date;
		map->month = date.Month;
		map->year = date.Year;
	}
}

static void ds1307_write(uint8_t data)
{
	if (ds1307_ctx.set_idx) {
		ds1307_ctx.idx = data % sizeof(struct ds1307_map);
		ds1307_ctx.set_idx = 0;
		return;
	}
	*((uint8_t *)(&ds1307_ctx.map) + ds1307_ctx.idx) = data;
	ds1307_ctx.idx = (ds1307_ctx.idx + 1) % sizeof(struct ds1307_map);
	ds1307_ctx.set_rtc = 1;
}

static uint8_t ds1307_read(void)
{
	uint8_t tmp = *((uint8_t *)(&ds1307_ctx.map) + ds1307_ctx.idx);
	ds1307_ctx.idx = (ds1307_ctx.idx + 1) % sizeof(struct ds1307_map);
	return tmp;
}

static void ds1307_stop(void)
{
	if (ds1307_ctx.set_rtc == 0)
		return;

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	struct ds1307_map *map = &ds1307_ctx.map;

	time.Seconds = map->seconds & ~(1 << 7);
	time.Minutes = map->minutes;
	if (map->hours & (1 << 6)) {
		/* 12 hours mode */
		int pm = (map->hours >> 5) & 1;
		uint8_t hour_binary = RTC_Bcd2ToByte(map->hours & 0x1f);
		hour_binary = (hour_binary % 12) + 12 * pm;
		time.Hours = RTC_ByteToBcd2(hour_binary);
	} else {
		time.Hours = map->hours;
	}
	date.WeekDay = map->day;
	date.Date = map->date;
	date.Month = map->month;
	date.Year = map->year;

	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD);
	ds1307_ctx.set_rtc = 0;
}

static struct i2c_slave_op slave = {
	.addr = 0x68,	/* ds1307 common slave address */
	.match = ds1307_match,
	.write = ds1307_write,
	.read = ds1307_read,
	.stop = ds1307_stop,
};

void ds1307_init(void)
{
	assert(sizeof(struct ds1307_map) == 0x40);
	i2c_slave_register(&slave,i2c_ctx0);
}

