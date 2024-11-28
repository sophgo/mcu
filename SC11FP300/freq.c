
#include <freq.h>
#include <dbgi2c.h>
#include <system.h>
#include <common.h>
#include <gd32e50x.h>
#include <debug.h>

#define	POSTDIV_RESULT_INDEX	2

static int sg2044_pll_mux[][2] = {
	{MPLL0_CLK, 0}, {MPLL1_CLK, 1}, {MPLL2_CLK, 2}, {MPLL3_CLK, 3},
	{MPLL4_CLK, 4}, {MPLL5_CLK, 5}, {FPLL1_CLK, 6}, {DPLL0_CLK, 7},
	{DPLL1_CLK, 8}, {DPLL2_CLK, 9}, {DPLL3_CLK, 10}, {DPLL4_CLK, 11},
	{DPLL5_CLK, 12}, {DPLL6_CLK, 13}, {DPLL7_CLK, 14}
};

int postdiv1_2[][3] = {
	{2, 4,  8}, {3, 3,  9}, {2, 5, 10}, {2, 6, 12},
	{2, 7, 14}, {3, 5, 15}, {4, 4, 16}, {3, 6, 18},
	{4, 5, 20}, {3, 7, 21}, {4, 6, 24}, {5, 5, 25},
	{4, 7, 28}, {5, 6, 30}, {5, 7, 35}, {6, 6, 36},
	{6, 7, 42}, {7, 7, 49}
};

static inline unsigned long abs_diff(unsigned long a, unsigned long b)
{
	return (a > b) ? (a - b) : (b - a);
}

/* fixed clocks */
struct sg2044_pll_clock sg2044_root_pll_clks[] = {
	{
		.id = MPLL0_CLK,
		.name = "mpll0_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = MPLL1_CLK,
		.name = "mpll1_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = MPLL2_CLK,
		.name = "mpll2_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = MPLL3_CLK,
		.name = "mpll3_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = MPLL4_CLK,
		.name = "mpll4_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = MPLL5_CLK,
		.name = "mpll5_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = FPLL0_CLK,
		.name = "fpll0_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
	}, {
		.id = FPLL1_CLK,
		.name = "fpll1_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
	}, {
		.id = DPLL0_CLK,
		.name = "dpll0_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL1_CLK,
		.name = "dpll1_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL2_CLK,
		.name = "dpll2_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL3_CLK,
		.name = "dpll3_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL4_CLK,
		.name = "dpll4_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL5_CLK,
		.name = "dpll5_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL6_CLK,
		.name = "dpll6_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}, {
		.id = DPLL7_CLK,
		.name = "dpll7_clock",
		.parent_name = "cgi",
		// .flags = CLK_GET_RATE_NOCACHE | CLK_GET_ACCURACY_NOCACHE,
		// .ini_flags = SG2044_CLK_RO,
		.status_offset = 0x98,
		.enable_offset = 0x9c,
	}
};

int top_misc_read(int idx, uint64_t offset, uint32_t *value)
{
	uint64_t addr;
	int err;

	addr = TOP_MISC_BASE + offset;
	err = dbgi2c_read32(idx, addr, value);
	if (err)
		dbg_printf("failed on top misc reg read at addr 0x%lx\n", addr);
		return err;

	return err;
}


int top_misc_write(int idx, uint64_t offset, uint32_t value)
{
	uint64_t addr;
	int err;

	addr = TOP_MISC_BASE + offset;
	err = dbgi2c_write32(idx, addr, value);
	if (err)
		dbg_printf("failed on top misc reg write at addr 0x%lx\n", addr);
		return err;

	return err;
}


static int sg2044_pll_write(int idx, int id, int value)
{
	return top_misc_write(idx, PLL_CTRL_OFFSET + (id << 3), value);
}

static int sg2044_pll_read(int idx, int id, uint32_t *pvalue)
{
	return top_misc_read(idx, PLL_CTRL_OFFSET + (id << 3), pvalue);
}

static inline int sg2044_pll_write_l(int idx, int id, int value)
{
	return top_misc_write(idx, PLL_CTRL_OFFSET + (id << 3) - 4, value);
}

static inline int sg2044_pll_read_l(int idx, int id, uint32_t *pvalue)
{
	return top_misc_read(idx, PLL_CTRL_OFFSET + (id << 3) - 4, pvalue);
}

/*
 * @reg_value: current register value
 * @rate: request rate
 * @prate: parent rate
 * @pctrl_table: use to save div1/div2/fbdiv/refdiv
 *
 * We use below equation to get POSTDIV1 and POSTDIV2
 * POSTDIV = (parent_rate/REFDIV) x FBDIV/input_rate
 * above POSTDIV = POSTDIV1*POSTDIV2
 */
static int __pll_get_postdiv_1_2(uint64_t rate, uint64_t prate,
				 uint32_t fbdiv, uint32_t refdiv, uint32_t *postdiv1,
				 uint32_t *postdiv2)
{
	int index = 0;
	int ret = 0;
	uint64_t tmp0;

	/* calculate (parent_rate/refdiv)
	 * and result save to prate
	 */
	tmp0 = prate;
	do_div(tmp0, refdiv);

	/* calcuate ((parent_rate/REFDIV) x FBDIV)
	 * and result save to prate
	 */
	tmp0 *= fbdiv;

	/* calcuate (((parent_rate/REFDIV) x FBDIV)/input_rate)
	 * and result save to prate
	 * here *prate is (POSTDIV1*POSTDIV2)
	 */
	do_div(tmp0, rate);

	/* calculate div1 and div2 value */
	if (tmp0 <= 7) {
		/* (div1 * div2) <= 7, no need to use array search */
		*postdiv1 = tmp0;
		*postdiv2 = 1;
	} else {
		/* (div1 * div2) > 7, use array search */
		for (index = 0; index < ARRAY_SIZE(postdiv1_2); index++) {
			if (tmp0 > postdiv1_2[index][POSTDIV_RESULT_INDEX]) {
				continue;
			} else {
				/* found it */
				break;
			}
		}
		if (index < ARRAY_SIZE(postdiv1_2)) {
			*postdiv1 = postdiv1_2[index][1];
			*postdiv2 = postdiv1_2[index][0];
		} else {
			dbg_printf("%s out of postdiv array range!\n", __func__);
			ret = -1;
		}
	}

	return ret;
}

/**
 * __set_pll_vcosel() - select vco configuration for pll
 * bit[17:16] 2'd2 --> 1.6GHz - 2.4GHz
 * 			  2'd3 --> 2.4GHz - 3.2GHz
 * @sg2044_pll: sg2044 pll control struct
 * @foutvco: target foutvco value
 */
static void __set_pll_vcosel(int idx, struct sg2044_pll_clock *sg2044_pll, uint64_t foutvco)
{
	int vcosel;
	uint32_t value;

	if (foutvco < (2400 * MHZ))
		vcosel = 0x2;
	else
		vcosel = 0x3;

	sg2044_pll_read_l(idx, sg2044_pll->id, &value);
	value &= ~(0x3 << 16);
	value |= (vcosel << 16);
	sg2044_pll_write_l(idx, sg2044_pll->id, value);
}

static inline int sg2044_pll_id2shift(uint32_t id)
{
	for (int i = 0; i < 15; i++) {
		if (sg2044_pll_mux[i][0] == id)
			return sg2044_pll_mux[i][1];
	}
	//BUG_ON(1);
}

static inline int sg2044_pll_switch_mux(int idx, struct sg2044_pll_clock *pll, char en)
{
	uint32_t value;
	uint32_t id = pll->id;
	int shift;

	shift = sg2044_pll_id2shift(id);
	top_misc_read(idx, PLL_SELECT_OFFSET, &value);
	if (en) {
		top_misc_write(idx, PLL_SELECT_OFFSET, value & (~(1<<shift)));
	} else {
		top_misc_write(idx, PLL_SELECT_OFFSET, value | (1<<shift));
	}

	return 0;
}

static inline int sg2044_pll_enable(int idx, struct sg2044_pll_clock *pll, char en)
{
	uint32_t value;
	uint64_t enter;
	uint32_t id = pll->id;
	uint64_t counter = 0;

	if (en) {
		/* wait pll lock */
		//enter = jiffies;
		top_misc_read(idx, pll->status_offset, &value);
		while (!((value >> (PLL_STAT_LOCK_OFFSET + id)) & 0x1)) {
			top_misc_read(idx, pll->status_offset, &value);
			counter++;
			if (counter == 100) {
				dbg_printf("%s:top misc read fail\n", __LINE__);
				break;
			}
			// if (time_after(jiffies, enter + HZ / 10))
			// 	pr_warn("%s not locked\n", pll->name);
		}
		/* wait pll updating */
		//enter = jiffies;
		counter = 0;
		top_misc_read(idx, pll->status_offset, &value);
		while (((value >> id) & 0x1)) {
			top_misc_read(idx, pll->status_offset, &value);
			counter++;
			if (counter == 100) {
				dbg_printf("%s:top misc read fail\n", __LINE__);
				break;
			}
			// if (time_after(jiffies, enter + HZ / 10))
			// 	pr_warn("%s still updating\n", pll->name);
		}
		/* enable pll */
		top_misc_read(idx, pll->enable_offset, &value);
		top_misc_write(idx, pll->enable_offset, value | (1 << id));
	} else {
		/* disable pll */
		top_misc_read(idx, pll->enable_offset, &value);
		top_misc_write(idx, pll->enable_offset, value & (~(1 << id)));
	}

	return 0;
}

static int __get_pll_ctl_setting(struct sg2044_pll_ctrl *best,
			uint64_t req_rate, uint64_t parent_rate)
{
	int ret;
	uint32_t fbdiv, refdiv, fref, postdiv1, postdiv2;
	uint64_t tmp = 0, foutvco;

	fref = parent_rate;

	for (refdiv = REFDIV_MIN; refdiv < REFDIV_MAX + 1; refdiv++) {
		for (fbdiv = FBDIV_MIN; fbdiv < FBDIV_MAX + 1; fbdiv++) {
			foutvco = fref * fbdiv / refdiv;
			/* check fpostdiv pfd */
			if (foutvco < PLL_FREQ_MIN || foutvco > PLL_FREQ_MAX
					|| (fref / refdiv) < 10)
				continue;

			ret = __pll_get_postdiv_1_2(req_rate, fref, fbdiv,
					refdiv, &postdiv1, &postdiv2);
			if (ret)
				continue;

			tmp = foutvco / (postdiv1 * postdiv2);
			if (abs_diff(tmp, req_rate) < abs_diff(best->freq, req_rate)) {
				best->freq = tmp;
				best->refdiv = refdiv;
				best->fbdiv = fbdiv;
				best->postdiv1 = postdiv1;
				best->postdiv2 = postdiv2;
				if (tmp == req_rate)
					return 0;
			}
			continue;
		}
	}

	return 0;
}

int sg2044_clk_pll_set_rate(int idx, int mpll_id, uint64_t rate, uint64_t parent_rate)
{
	int ret = 0;
	uint64_t flags;
	uint32_t value;
	uint64_t foutvco;
	struct sg2044_pll_ctrl pctrl_table;
	struct sg2044_pll_clock *sg2044_pll = &sg2044_root_pll_clks[mpll_id];

	memset(&pctrl_table, 0, sizeof(struct sg2044_pll_ctrl));
	//spin_lock_irqsave(sg2044_pll->lock, flags);
	/* switch to fpll before modify mpll */
	sg2044_pll_switch_mux(idx, sg2044_pll, 1);
	if (sg2044_pll_enable(idx, sg2044_pll, 0)) {
		dbg_printf("Can't disable pll(%s), status error\n", sg2044_pll->name);
		ret = -1;
		goto out;
	}

	sg2044_pll_read(idx, sg2044_pll->id, &value);
	__get_pll_ctl_setting(&pctrl_table, rate, parent_rate);
	if (!pctrl_table.freq) {
		dbg_printf("%s: Can't find a proper pll setting\n", sg2044_pll->name);
		ret = -1;
		goto out;
	}

	value = TOP_PLL_CTRL(pctrl_table.fbdiv, pctrl_table.postdiv1,
			     pctrl_table.postdiv2, pctrl_table.refdiv);

	foutvco = parent_rate * pctrl_table.fbdiv / pctrl_table.refdiv;
	__set_pll_vcosel(idx, sg2044_pll, foutvco);

	//debug("cal val is %u \n", value);

	/* write the value to top register */
	sg2044_pll_write(idx, sg2044_pll->id, value);
	sg2044_pll_enable(idx, sg2044_pll, 1);
	/* switch back to mpll after modify mpll */
	sg2044_pll_switch_mux(idx, sg2044_pll, 0);
out:
	//spin_unlock_irqrestore(sg2044_pll->lock, flags);
	return ret;
}