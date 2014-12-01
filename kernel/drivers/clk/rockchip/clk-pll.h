#ifndef __RK_CLK_PLL_H
#define __RK_CLK_PLL_H

#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/rockchip/cru.h>


#define CLK_LOOPS_JIFFY_REF	(11996091ULL)
#define CLK_LOOPS_RATE_REF	(1200UL) //Mhz
#define CLK_LOOPS_RECALC(rate)  \
	div_u64(CLK_LOOPS_JIFFY_REF*(rate),CLK_LOOPS_RATE_REF*MHZ)

#define CLK_DIV_PLUS_ONE_SET(i, shift, width)	\
	((((i)-1) << (shift)) | (((2<<(width)) - 1) << ((shift)+16)))

/*******************RK3188 PLL******************************/
#define RK3188_PLL_CON(i)	((i) * 4)
/*******************PLL WORK MODE*************************/
#define _RK3188_PLL_MODE_MSK		0x3
#define _RK3188_PLL_MODE_SLOW		0x0
#define _RK3188_PLL_MODE_NORM		0x1
#define _RK3188_PLL_MODE_DEEP		0x2

#define _RK3188_PLL_MODE_GET(offset, shift)	\
	((cru_readl(offset) >> (shift)) & _RK3188_PLL_MODE_MSK)

#define _RK3188_PLL_MODE_IS_SLOW(offset, shift)	\
	(_RK3188_PLL_MODE_GET(offset, shift) == _RK3188_PLL_MODE_SLOW)

#define _RK3188_PLL_MODE_IS_NORM(offset, shift)	\
	(_RK3188_PLL_MODE_GET(offset, shift) == _RK3188_PLL_MODE_NORM)

#define _RK3188_PLL_MODE_IS_DEEP(offset, shift)	\
	(_RK3188_PLL_MODE_GET(offset, shift) == _RK3188_PLL_MODE_DEEP)

#define _RK3188_PLL_MODE_SET(val, shift)	\
	((val) << (shift)) | CRU_W_MSK(shift, _RK3188_PLL_MODE_MSK)

#define _RK3188_PLL_MODE_SLOW_SET(shift)	\
	_RK3188_PLL_MODE_SET(_RK3188_PLL_MODE_SLOW, shift)

#define _RK3188_PLL_MODE_NORM_SET(shift)	\
	_RK3188_PLL_MODE_SET(_RK3188_PLL_MODE_NORM, shift)

#define _RK3188_PLL_MODE_DEEP_SET(shift)	\
	_RK3188_PLL_MODE_SET(_RK3188_PLL_MODE_DEEP, shift)

/*******************PLL OPERATION MODE*********************/
#define _RK3188_PLL_BYPASS_SHIFT	0
#define _RK3188_PLL_POWERDOWN_SHIFT	1

#define _RK3188PLUS_PLL_BYPASS_SHIFT	0
#define _RK3188PLUS_PLL_POWERDOWN_SHIFT	1
#define _RK3188PLUS_PLL_RESET_SHIFT	5

#define _RK3188_PLL_OP_SET(val, shift)	\
	((val) << (shift)) | CRU_W_MSK(shift, 1)

#define _RK3188_PLL_BYPASS_SET(val)	\
	_RK3188_PLL_OP_SET(val, _RK3188_PLL_BYPASS_SHIFT)

#define _RK3188_PLL_POWERDOWN_SET(val)	\
	_RK3188_PLL_OP_SET(val, _RK3188_PLL_POWERDOWN_SHIFT)

#define _RK3188PLUS_PLL_BYPASS_SET(val)	\
	_RK3188_PLL_OP_SET(val, _RK3188PLUS_PLL_BYPASS_SHIFT)

#define _RK3188PLUS_PLL_POWERDOWN_SET(val)	\
	_RK3188_PLL_OP_SET(val, _RK3188PLUS_PLL_POWERDOWN_SHIFT)

#define _RK3188PLUS_PLL_RESET_SET(val)	\
	_RK3188_PLL_OP_SET(val, _RK3188PLUS_PLL_RESET_SHIFT)

/*******************PLL CON0 BITS***************************/
#define RK3188_PLL_CLKFACTOR_SET(val, shift, msk) \
	((((val) - 1) & (msk)) << (shift))

#define RK3188_PLL_CLKFACTOR_GET(reg, shift, msk) \
	((((reg) >> (shift)) & (msk)) + 1)

#define RK3188_PLL_OD_MSK		(0x3f)
#define RK3188_PLL_OD_SHIFT 		(0x0)
#define RK3188_PLL_CLKOD(val)		RK3188_PLL_CLKFACTOR_SET(val, RK3188_PLL_OD_SHIFT, RK3188_PLL_OD_MSK)
#define RK3188_PLL_NO(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188_PLL_OD_SHIFT, RK3188_PLL_OD_MSK)
#define RK3188_PLL_CLKOD_SET(val)	(RK3188_PLL_CLKOD(val) | CRU_W_MSK(RK3188_PLL_OD_SHIFT, RK3188_PLL_OD_MSK))

#define RK3188_PLL_NR_MSK		(0x3f)
#define RK3188_PLL_NR_SHIFT		(8)
#define RK3188_PLL_CLKR(val)		RK3188_PLL_CLKFACTOR_SET(val, RK3188_PLL_NR_SHIFT, RK3188_PLL_NR_MSK)
#define RK3188_PLL_NR(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188_PLL_NR_SHIFT, RK3188_PLL_NR_MSK)
#define RK3188_PLL_CLKR_SET(val)	(RK3188_PLL_CLKR(val) | CRU_W_MSK(RK3188_PLL_NR_SHIFT, RK3188_PLL_NR_MSK))

#define RK3188PLUS_PLL_OD_MSK		(0xf)
#define RK3188PLUS_PLL_OD_SHIFT 	(0x0)
#define RK3188PLUS_PLL_CLKOD(val)	RK3188_PLL_CLKFACTOR_SET(val, RK3188PLUS_PLL_OD_SHIFT, RK3188PLUS_PLL_OD_MSK)
#define RK3188PLUS_PLL_NO(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188PLUS_PLL_OD_SHIFT, RK3188PLUS_PLL_OD_MSK)
#define RK3188PLUS_PLL_CLKOD_SET(val)	(RK3188PLUS_PLL_CLKOD(val) | CRU_W_MSK(RK3188PLUS_PLL_OD_SHIFT, RK3188PLUS_PLL_OD_MSK))

#define RK3188PLUS_PLL_NR_MSK		(0x3f)
#define RK3188PLUS_PLL_NR_SHIFT		(8)
#define RK3188PLUS_PLL_CLKR(val)	RK3188_PLL_CLKFACTOR_SET(val, RK3188PLUS_PLL_NR_SHIFT, RK3188PLUS_PLL_NR_MSK)
#define RK3188PLUS_PLL_NR(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188PLUS_PLL_NR_SHIFT, RK3188PLUS_PLL_NR_MSK)
#define RK3188PLUS_PLL_CLKR_SET(val)	(RK3188PLUS_PLL_CLKR(val) | CRU_W_MSK(RK3188PLUS_PLL_NR_SHIFT, RK3188PLUS_PLL_NR_MSK))

/*******************PLL CON1 BITS***************************/
#define RK3188_PLL_NF_MSK		(0xffff)
#define RK3188_PLL_NF_SHIFT		(0)
#define RK3188_PLL_CLKF(val)		RK3188_PLL_CLKFACTOR_SET(val, RK3188_PLL_NF_SHIFT, RK3188_PLL_NF_MSK)
#define RK3188_PLL_NF(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188_PLL_NF_SHIFT, RK3188_PLL_NF_MSK)
#define RK3188_PLL_CLKF_SET(val)	(RK3188_PLL_CLKF(val) | CRU_W_MSK(RK3188_PLL_NF_SHIFT, RK3188_PLL_NF_MSK))

#define RK3188PLUS_PLL_NF_MSK		(0x1fff)
#define RK3188PLUS_PLL_NF_SHIFT		(0)
#define RK3188PLUS_PLL_CLKF(val)	RK3188_PLL_CLKFACTOR_SET(val, RK3188PLUS_PLL_NF_SHIFT, RK3188PLUS_PLL_NF_MSK)
#define RK3188PLUS_PLL_NF(reg)		RK3188_PLL_CLKFACTOR_GET(reg, RK3188PLUS_PLL_NF_SHIFT, RK3188PLUS_PLL_NF_MSK)
#define RK3188PLUS_PLL_CLKF_SET(val)	(RK3188PLUS_PLL_CLKF(val) | CRU_W_MSK(RK3188PLUS_PLL_NF_SHIFT, RK3188PLUS_PLL_NF_MSK))

/*******************PLL CON2 BITS***************************/
#define RK3188_PLL_BWADJ_MSK		(0xfff)
#define RK3188_PLL_BWADJ_SHIFT		(0)
#define RK3188_PLL_CLK_BWADJ_SET(val)	((val) | CRU_W_MSK(RK3188_PLL_BWADJ_SHIFT, RK3188_PLL_BWADJ_MSK))

#define RK3188PLUS_PLL_BWADJ_MSK	(0xfff)
#define RK3188PLUS_PLL_BWADJ_SHIFT	(0)
#define RK3188PLUS_PLL_CLK_BWADJ_SET(val)	((val) | CRU_W_MSK(RK3188PLUS_PLL_BWADJ_SHIFT, RK3188PLUS_PLL_BWADJ_MSK))

/*******************PLL CON3 BITS***************************/
#define RK3188_PLL_RESET_MSK		(1 << 5)
#define RK3188_PLL_RESET_W_MSK		(RK3188_PLL_RESET_MSK << 16)
#define RK3188_PLL_RESET		(1 << 5)
#define RK3188_PLL_RESET_RESUME		(0 << 5)

#define RK3188_PLL_BYPASS_MSK		(1 << 0)
#define RK3188_PLL_BYPASS		(1 << 0)
#define RK3188_PLL_NO_BYPASS		(0 << 0)

#define RK3188_PLL_PWR_DN_MSK		(1 << 1)
#define RK3188_PLL_PWR_DN_W_MSK		(RK3188_PLL_PWR_DN_MSK << 16)
#define RK3188_PLL_PWR_DN		(1 << 1)
#define RK3188_PLL_PWR_ON		(0 << 1)

#define RK3188_PLL_STANDBY_MSK		(1 << 2)
#define RK3188_PLL_STANDBY		(1 << 2)
#define RK3188_PLL_NO_STANDBY		(0 << 2)

/*******************CLKSEL0 BITS***************************/
//core_preiph div
#define RK3188_CORE_PERIPH_W_MSK	(3 << 22)
#define RK3188_CORE_PERIPH_MSK		(3 << 6)
#define RK3188_CORE_PERIPH_2		(0 << 6)
#define RK3188_CORE_PERIPH_4		(1 << 6)
#define RK3188_CORE_PERIPH_8		(2 << 6)
#define RK3188_CORE_PERIPH_16		(3 << 6)

//clk_core
#define RK3188_CORE_SEL_PLL_MSK		(1 << 8)
#define RK3188_CORE_SEL_PLL_W_MSK	(1 << 24)
#define RK3188_CORE_SEL_APLL		(0 << 8)
#define RK3188_CORE_SEL_GPLL		(1 << 8)

#define RK3188_CORE_CLK_DIV_W_MSK	(0x1F << 25)
#define RK3188_CORE_CLK_DIV_MSK		(0x1F << 9)
#define RK3188_CORE_CLK_DIV(i)		((((i) - 1) & 0x1F) << 9)
#define RK3188_CORE_CLK_MAX_DIV		32

/*******************CLKSEL1 BITS***************************/
//aclk_core div
#define RK3188_CORE_ACLK_W_MSK		(7 << 19)
#define RK3188_CORE_ACLK_MSK		(7 << 3)
#define RK3188_CORE_ACLK_11		(0 << 3)
#define RK3188_CORE_ACLK_21		(1 << 3)
#define RK3188_CORE_ACLK_31		(2 << 3)
#define RK3188_CORE_ACLK_41		(3 << 3)
#define RK3188_CORE_ACLK_81		(4 << 3)
#define RK3188_GET_CORE_ACLK_VAL(reg)	((reg)>=4 ? 8:((reg)+1))

/*******************PLL SET*********************************/
#define _RK3188_PLL_SET_CLKS(_mhz, nr, nf, no) \
{ \
	.rate   = (_mhz) * KHZ, \
	.pllcon0 = RK3188_PLL_CLKR_SET(nr)|RK3188_PLL_CLKOD_SET(no), \
	.pllcon1 = RK3188_PLL_CLKF_SET(nf),\
	.pllcon2 = RK3188_PLL_CLK_BWADJ_SET(nf >> 1),\
	.rst_dly = ((nr*500)/24+1),\
}

#define _RK3188PLUS_PLL_SET_CLKS(_mhz, nr, nf, no) \
{ \
	.rate   = (_mhz) * KHZ, \
	.pllcon0 = RK3188PLUS_PLL_CLKR_SET(nr)|RK3188PLUS_PLL_CLKOD_SET(no), \
	.pllcon1 = RK3188PLUS_PLL_CLKF_SET(nf),\
	.pllcon2 = RK3188PLUS_PLL_CLK_BWADJ_SET(nf >> 1),\
	.rst_dly = ((nr*500)/24+1),\
}

#define _RK3188_APLL_SET_CLKS(_mhz, nr, nf, no, _periph_div, _aclk_div) \
{ \
	.rate   = _mhz * MHZ, \
	.pllcon0 = RK3188_PLL_CLKR_SET(nr) | RK3188_PLL_CLKOD_SET(no), \
	.pllcon1 = RK3188_PLL_CLKF_SET(nf),\
	.pllcon2 = RK3188_PLL_CLK_BWADJ_SET(nf >> 1),\
	.rst_dly = ((nr*500)/24+1),\
	.clksel0 = RK3188_CORE_PERIPH_W_MSK | RK3188_CORE_PERIPH_##_periph_div,\
	.clksel1 = RK3188_CORE_ACLK_W_MSK | RK3188_CORE_ACLK_##_aclk_div,\
	.lpj = (CLK_LOOPS_JIFFY_REF*_mhz) / CLK_LOOPS_RATE_REF,\
}


/*******************RK3288 PLL***********************************/
/*******************CLKSEL0 BITS***************************/
#define RK3288_CORE_SEL_PLL_W_MSK	(1 << 31)
#define RK3288_CORE_SEL_APLL		(0 << 15)
#define RK3288_CORE_SEL_GPLL		(1 << 15)

#define RK3288_CORE_CLK_SHIFT		8
#define RK3288_CORE_CLK_WIDTH		5
#define RK3288_CORE_CLK_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_CORE_CLK_SHIFT, RK3288_CORE_CLK_WIDTH)
#define RK3288_CORE_CLK_MAX_DIV		(2<<RK3288_CORE_CLK_WIDTH)

#define RK3288_ACLK_M0_SHIFT		0
#define RK3288_ACLK_M0_WIDTH		4
#define RK3288_ACLK_M0_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_ACLK_M0_SHIFT, RK3288_ACLK_M0_WIDTH)

#define RK3288_ACLK_MP_SHIFT		4
#define RK3288_ACLK_MP_WIDTH		4
#define RK3288_ACLK_MP_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_ACLK_MP_SHIFT, RK3288_ACLK_MP_WIDTH)

/*******************CLKSEL37 BITS***************************/
#define RK3288_CLK_L2RAM_SHIFT		0
#define RK3288_CLK_L2RAM_WIDTH		3
#define RK3288_CLK_L2RAM_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_CLK_L2RAM_SHIFT, RK3288_CLK_L2RAM_WIDTH)

#define RK3288_ATCLK_SHIFT		4
#define RK3288_ATCLK_WIDTH		5
#define RK3288_ATCLK_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_ATCLK_SHIFT, RK3288_ATCLK_WIDTH)

#define RK3288_PCLK_DBG_SHIFT		9
#define RK3288_PCLK_DBG_WIDTH		5
#define RK3288_PCLK_DBG_DIV(i)	\
	CLK_DIV_PLUS_ONE_SET(i, RK3288_PCLK_DBG_SHIFT, RK3288_PCLK_DBG_WIDTH)

#define _RK3288_APLL_SET_CLKS(_mhz, nr, nf, no, l2_div, m0_div, mp_div, atclk_div, pclk_dbg_div) \
{ \
	.rate   = _mhz * MHZ, \
	.pllcon0 = RK3188PLUS_PLL_CLKR_SET(nr) | RK3188PLUS_PLL_CLKOD_SET(no), \
	.pllcon1 = RK3188PLUS_PLL_CLKF_SET(nf),\
	.pllcon2 = RK3188PLUS_PLL_CLK_BWADJ_SET(nf >> 1),\
	.rst_dly = ((nr*500)/24+1),\
	.clksel0 = RK3288_ACLK_M0_DIV(m0_div) | RK3288_ACLK_MP_DIV(mp_div),\
	.clksel1 = RK3288_CLK_L2RAM_DIV(l2_div) | RK3288_ATCLK_DIV(atclk_div) | RK3288_PCLK_DBG_DIV(pclk_dbg_div),\
	.lpj = (CLK_LOOPS_JIFFY_REF*_mhz) / CLK_LOOPS_RATE_REF,\
}


struct pll_clk_set {
	unsigned long	rate;
	u32	pllcon0;
	u32	pllcon1;
	u32	pllcon2;
	unsigned long	rst_dly;//us
};

struct apll_clk_set {
	unsigned long	rate;
	u32	pllcon0;
	u32	pllcon1;
	u32	pllcon2;
	u32 	rst_dly;//us
	u32	clksel0;
	u32	clksel1;
	unsigned long	lpj;
};


#define to_clk_pll(_hw) container_of(_hw, struct clk_pll, hw)

struct clk_pll {
	struct clk_hw	hw;
	u32		reg;
	u32		width;
	u32		mode_offset;
	u8		mode_shift;
	u32		status_offset;
	u8		status_shift;
	u32		flags;
	const void	*table;
	spinlock_t	*lock;
};

const struct clk_ops *rk_get_pll_ops(u32 pll_flags);

struct clk *rk_clk_register_pll(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags, u32 reg,
		u32 width, u32 mode_offset, u8 mode_shift,
		u32 status_offset, u8 status_shift, u32 pll_flags,
		spinlock_t *lock);


#endif /* __RK_CLK_PLL_H */
