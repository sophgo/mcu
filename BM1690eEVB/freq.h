#ifndef _FREQ_H_
#define _FREQ_H_

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)

#define TPU_MPLL	2
#define K2K_MPLL	3
#define PARENT_FREQ	25 * MHZ


int top_misc_read(uint64_t offset, uint32_t *value);
int top_misc_write(uint64_t offset, uint32_t value);
int sg2044_clk_pll_set_rate(int mpll_id, uint64_t rate, uint64_t parent_rate);

#endif
