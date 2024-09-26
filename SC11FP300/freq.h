#ifndef _FREQ_H_
#define _FREQ_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TOP_MISC_BASE	0x7050000000

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)

#define SG2044_CLK_USE_INIT_VAL		BIT(0) /* use default value */
#define SG2044_CLK_USE_REG_VAL		BIT(1) /* use reg divider value */
#define SG2044_CLK_RO			BIT(2) /* use reg divider value */

#define CLK_PLL	BIT(0)
#define CLK_MUX	BIT(1)

#define PLL_CTRL_OFFSET	0xC4
#define PLL_STAT_LOCK_OFFSET	0x10
#define PLL_SELECT_OFFSET	0x2020
#define CLK_MODE	0x4
#define CLK_MODE_MASK	0x3

#define REFDIV_MIN 1
#define REFDIV_MAX 63
#define FBDIV_MIN 8
#define FBDIV_MAX 1066

#define PLL_FREQ_MIN (1600 * MHZ)
#define PLL_FREQ_MAX (3200 * MHZ)

#define div_mask(width) ((1 << (width)) - 1)
#define TOP_PLL_CTRL(fbdiv, p1, p2, refdiv) \
	(BIT(30) | (0x2 << 27) | BIT(24) | (((p2 - 1) & 0x7) << 21) | \
	(((p1 - 1) & 0x7) << 18) | ((refdiv & 0x3f) << 12) | (fbdiv & 0xfff))

#define MPLL0_CLK	0
#define MPLL1_CLK	1
#define MPLL2_CLK	2
#define MPLL3_CLK	3
#define MPLL4_CLK	4
#define MPLL5_CLK	5
#define FPLL0_CLK	6
#define FPLL1_CLK	7
#define DPLL0_CLK	8
#define DPLL1_CLK	9
#define DPLL2_CLK	10
#define DPLL3_CLK	11
#define DPLL4_CLK	12
#define DPLL5_CLK	13
#define DPLL6_CLK	14
#define DPLL7_CLK	15

static inline __attribute__((const))
char is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

#define do_div(n,base) ({				\
	uint32_t __base = (base);			\
	uint32_t __rem;					\
	__rem = (uint32_t)(n) % __base;			\
	(n) = (uint32_t)(n) / __base;			\
	__rem;						\
 })

struct sg2044_pll_ctrl {
	unsigned int mode;
	unsigned long freq;

	unsigned int fbdiv;
	unsigned int postdiv1;
	unsigned int postdiv2;
	unsigned int refdiv;
};

struct sg2044_pll_clock {
	unsigned int id;
	char *name;
	const char *parent_name;
	// unsigned long flags;
	// struct clk_hw	hw;
	// struct regmap *syscon_top;

	/* Below lock used to protect PLL top register during write */
	// spinlock_t *lock;
	// u32 ini_flags;

	unsigned int status_offset;
	unsigned int enable_offset;

	struct sg2044_pll_ctrl pctrl_table[4];
};

int top_misc_read(int idx, uint64_t offset, uint32_t *value);
int top_misc_write(int idx, uint64_t offset, uint32_t value);
int sg2044_clk_pll_set_rate(int idx, int mpll_id, uint64_t rate, uint64_t parent_rate);

#endif