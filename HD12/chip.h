#ifndef __CHIP_H__
#define __CHIP_H__

#include <common.h>

void chip_init(void);
void chip_listen(void);
void chip_update(void);
void chip_destroy(void);
int chip_is_enabled(void);
void sys_rst_disable(void);
int chip_enable(void);
void sys_rst_enable(void);
void set_vddr_wake_status(int chip, int flag);
int get_chip_vddr_status(int chip);
void host_powergood_init(void);
void reset_c2c(void);
void c2c_check(void);

#define BN_SYS_RST_ENABLE(n)					\
	do {							\
		gpio_set(SYS_RST_X_H_BM ## n ## _PORT,		\
			 SYS_RST_X_H_BM ## n ## _PIN);		\
	} while (0)

#define BN_SYS_RST_DISABLE(n)					\
	do {							\
		gpio_clear(SYS_RST_X_H_BM ## n ## _PORT,		\
			   SYS_RST_X_H_BM ## n ## _PIN);		\
	} while (0)

#endif
