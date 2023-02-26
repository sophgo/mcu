#ifndef __TCA9548A_H__
#define __TCA9548A_H__

#include <i2c_master.h>

enum {
	PCA9848_1 = 0,
	PCA9848_2,
	TCA9548A1,
	TCA9548A_MAX,
};

void pca9848_set(int id, unsigned int mask);
void pca9848_init(void);

#endif
