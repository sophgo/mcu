#ifndef ds1307
#define ds1307

#define SLAVE_ADDRESS 0x68 // the slave address (0xD0 read / 0xD1 write)
#define TIME_STRUCT_SIZE 0x08
#define RAM_SIZE 55
#define iic I2C2


typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    bool am;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    bool clock_halt;
    bool out;
    bool sqwe;
    bool rs1;
    bool rs0;
} time;

struct rtc_time {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
};
// RS1 | RS0 | SQ output | SQWE | OUT
//  0  |  0  |    1 Hz   |   1  |  X
//  0  |  1  |  4.096 Hz |   1  |  X
//  1  |  0  |  8.192 Hz |   1  |  X
//  1  |  1  | 32.768 Hz |   1  |  X
//  X  |  X  |     0     |   0  |  0
//  X  |  X  |     1     |   0  |  1

// uint8_t* readTime(void);
// void decodedata(const uint8_t *data, time *s_time);
// void writeTime(const time *s_time);
// void printTime(const time *s_time);
// void printByte(const uint8_t n);
// void printRawData(const uint8_t *data, const uint8_t size);
// time getTime(void);
int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			   uint8_t *w, size_t wn,
			   uint8_t *r, size_t rn);
// INFO: Addresses are relative!
// According to documentation:
// 0x00 - Timekeeper start
// 0x07 - Timekeeper end (8 bytes)
// 0x08 - RAM start
// 0x3F - RAM end (56 bytes)
// In this function - start address = 0x00 and end address = 0x38 (56 in dec)

void get_rtc_time(struct rtc_time *rtc_time);

#endif // ds1307