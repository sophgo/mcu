#ifndef __TCA9548A_H__
#define __TCA9548A_H__

#include <i2c_master.h>

enum {
	TCA9548A0 = 0,
	TCA9548A1,
	TCA9548A_MAX,
};

void tca9548a_set(int id, unsigned int mask);
void tca9548a_init(void);

#endif
