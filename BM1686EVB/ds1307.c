#include <system.h>
#include <common.h>
#include <stdio.h>
#include "ds1307.h"
#include <i2c_master.h>
#include <tick.h>
void decodedata(const uint8_t *data, time *s_time) {
    // decode seconds
    uint8_t msd = 0, lsd = 0;
    uint8_t /*am_pm = -1,*/_12h_mode = -1;

    lsd = (data[0] & 0b00001111);
    msd = (data[0] & 0b01110000) >> 4;
    s_time->seconds = lsd + 10 * msd;

    lsd = (data[1] & 0b00001111);
    msd = (data[1] & 0b01110000) >> 4;
    s_time->minutes = lsd + 10 * msd;

    // If 1, then 12-hour mode is enabled, 0 - 24-hour mode
    _12h_mode = (data[2] & 0b01000000) >> 6;

    // When 12-hour mode enabled, PM = 1, AM = 0, otherwise first bit of
    // hour_msd
    if (_12h_mode) {
        //am_pm = (data[2] & 0b00100000) >> 5;
        msd = (data[2] & 0b00010000) >> 4;
    } else {
        msd = (data[2] & 0b00110000) >> 4;
    }
    lsd = (data[2] & 0b00001111);
    s_time->hours = lsd + 10 * msd;

    s_time->day_of_week = (data[3] & 0b00000111);

    lsd = (data[4] & 0b00001111);
    msd = (data[4] & 0b00110000) >> 4;
    s_time->date = lsd + 10 * msd;

    lsd = (data[5] & 0b00001111);
    msd = (data[5] & 0b00010000) >> 4;
    s_time->month = lsd + 10 * msd;

    lsd = (data[6] & 0b00001111);
    msd = (data[6] & 0b11110000) >> 4;
    s_time->year = lsd + 10 * msd;

    s_time->clock_halt = (data[0] & 0b10000000) >> 7;
    s_time->out = (data[7] & 0b10000000) >> 7;
    s_time->sqwe = (data[7] & 0b00010000) >> 4;
    s_time->rs1 = (data[7] & 0b00000010) >> 1;
    s_time->rs0 = (data[7] & 0b00000001);
}

// Each number is represented in BCD format, according to documentation
uint8_t* encodedata(uint8_t *data, const time *s_time)
{
    uint8_t msd, lsd;

    // 0x00 Clock halt and seconds
    msd = s_time->seconds / 10;
    lsd = s_time->seconds - msd * 10;
    data[0] = (s_time->clock_halt << 7) | (msd << 4) | (lsd);

    // 0x01 Minutes
    msd = s_time->minutes / 10;
    lsd = s_time->minutes - msd * 10;
    data[1] = (msd << 4) | (lsd);

    // 0x02 Hours
    msd = s_time->hours / 10;
    lsd = s_time->hours - msd * 10;
    data[2] = (0 << 6 /*24h mode*/) | (msd << 4) | (lsd);

    // 0x03 Day of week
    data[3] = s_time->day_of_week;

    // 0x04 Date (day of month)
    msd = s_time->date / 10;
    lsd = s_time->date - msd * 10;
    data[4] = (msd << 4) | (lsd);

    // 0x05 Month
    msd = s_time->month / 10;
    lsd = s_time->month - msd * 10;
    data[5] = (msd << 4) | (lsd);

    // 0x06 Year
    msd = s_time->year / 10;
    lsd = s_time->year - msd * 10;
    data[6] = (msd << 4) | (lsd);

    // 0x07 Control part:
    // OUT, SQWE, RS1 and RS0
    data[7] = (s_time->out << 7) | (s_time->sqwe << 4) | (s_time->rs1 << 1)
            | (s_time->rs0);

    return data;
}

void printTime(const time *s_time) {
    printf("%2d:%2d:%2d\n\r", s_time->hours, s_time->minutes, s_time->seconds);

    switch (s_time->day_of_week) {
    case 1:
        printf("Monday\n\r");
        break;
    case 2:
        printf("Tuesday\n\r");
        break;
    case 3:
        printf("Wednesday\n\r");
        break;
    case 4:
        printf("Thursday\n\r");
        break;
    case 5:
        printf("Friday\n\r");
        break;
    case 6:
        printf("Saturday\n\r");
        break;
    case 7:
        printf("Sunday\n\r");
        break;

    default:
        printf("BORKEN\n\r");
        break;
    }

    printf("%2d.%2d.%2d\n\r", s_time->date, s_time->month, s_time->year);
    printf("Config: CH: %d OUT: %d SQWE: %d RS1: %d RS0: %d\n\r",
            s_time->clock_halt, s_time->out, s_time->sqwe, s_time->rs1,
            s_time->rs0);
}

void printByte(const uint8_t n) {
    unsigned i;
    for (i = 1 << 7; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
    printf("\n\r");
}

void printRawData(const uint8_t *data, const uint8_t size) {
    if (data == 0) {
        return;
    }

    uint8_t idx;

    for (idx = 0; idx < size; ++idx) {
        printByte(data[idx]);
    }
}

static int write_rtc(time *s_time)
{
    uint8_t data[8] = { 0 };

    encodedata(data, s_time);

    return i2c_transfer7_timeout(iic, SLAVE_ADDRESS, 1, data, ARRAY_SIZE(data), NULL, 0);

}

static int read_rtc(time *s_time)
{
    uint8_t data[8] = { 0};

    i2c_transfer7_timeout(iic, SLAVE_ADDRESS, 1, NULL, 0, data, ARRAY_SIZE(data));
    printRawData(data, ARRAY_SIZE(data));
    decodedata(data, s_time);
    printTime(s_time);

    return 0;
}

static unsigned int last_get_rtc_tick = 0;
void get_rtc_time(struct rtc_time *rtc_time)
{
    time s_time;
    unsigned int current_tick = tick_get();

    if (current_tick - last_get_rtc_tick >= 2000) {
        read_rtc(&s_time);
        write_rtc(&s_time);///////////////////////////////////注意后边删掉
        rtc_time->seconds = s_time.seconds;
        rtc_time->minutes = s_time.minutes;
        rtc_time->hours = s_time.hours;
        rtc_time->date = s_time.date;
        rtc_time->month = s_time.month;
        rtc_time->year = s_time.year;
        last_get_rtc_tick = current_tick;
    }
}