/*
 * Allwinner A10 DRAM controller initialization
 *
 * Based on sun4i Linux kernel sources mach-sunxi/pm/standby/dram*.c
 * and earlier U-Boot Allwiner A10 SPL work
 *
 * Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 * Copyright (C) 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 * Copyright (C) 2007-2012 Allwinner Technology Co., Ltd.
 *	Berg Xing <bergxing@allwinnertech.com>
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "clock.h"
#include "dramc.h"
#include "memmap.h"
#include "timer.h"

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>

static struct a1x_dramc *const dram = (void *)A1X_DRAMC_BASE;

static void mctl_ddr3_reset(void)
{
	if (a1x_get_cpu_chip_revision() != A1X_CHIP_REV_A) {
		setbits_le32(&dram->mcr, DRAM_MCR_RESET);
		udelay(2);
		clrbits_le32(&dram->mcr, DRAM_MCR_RESET);
	} else {
		clrbits_le32(&dram->mcr, DRAM_MCR_RESET);
		udelay(2);
		setbits_le32(&dram->mcr, DRAM_MCR_RESET);
	}
}

static void mctl_set_drive(void)
{
	clrsetbits_le32(&dram->mcr, DRAM_MCR_MODE_NORM(0x3),
			DRAM_MCR_MODE_EN(0x3) | 0xffc);
}

static void mctl_itm_disable(void)
{
	clrsetbits_le32(&dram->ccr, DRAM_CCR_INIT, DRAM_CCR_ITM_OFF);
}

static void mctl_itm_enable(void)
{
	clrbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);
}

static void mctl_enable_dll0(u32 phase)
{
	clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
			((phase >> 16) & 0x3f) << 6);
	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET, DRAM_DLLCR_DISABLE);
	udelay(2);

	clrbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET | DRAM_DLLCR_DISABLE);
	udelay(22);

	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_DISABLE, DRAM_DLLCR_NRESET);
	udelay(22);
}

/*
 * Note: This differs from pm/standby in that it checks the bus width
 */
static void mctl_enable_dllx(u32 phase)
{
	u32 i, n, bus_width;

	bus_width = read32(&dram->dcr);

	if ((bus_width & DRAM_DCR_BUS_WIDTH_MASK) ==
	    DRAM_DCR_BUS_WIDTH(DRAM_DCR_BUS_WIDTH_32BIT))
		n = DRAM_DCR_NR_DLLCR_32BIT;
	else
		n = DRAM_DCR_NR_DLLCR_16BIT;

	for (i = 1; i < n; i++) {
		clrsetbits_le32(&dram->dllcr[i], 0x4 << 14,
				(phase & 0xf) << 14);
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET,
				DRAM_DLLCR_DISABLE);
		phase >>= 4;
	}
	udelay(2);

	for (i = 1; i < n; i++)
		clrbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET |
			     DRAM_DLLCR_DISABLE);
	udelay(22);

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_DISABLE,
				DRAM_DLLCR_NRESET);
	udelay(22);
}

static u32 hpcr_value[32] = {
	0x0301, 0x0301, 0x0301, 0x0301,
	0x0301, 0x0301, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0x1031, 0x1031, 0x0735, 0x1035,
	0x1035, 0x0731, 0x1031, 0x0735,
	0x1035, 0x1031, 0x0731, 0x1035,
	0x1031, 0x0301, 0x0301, 0x0731
};

static void mctl_configure_hostport(void)
{
	u32 i;

	for (i = 0; i < 32; i++)
		write32(&dram->hpcr[i], hpcr_value[i]);
}

static void mctl_setup_dram_clock(u32 clk)
{
	/* setup DRAM PLL */
	a1x_pll5_configure(clk / 24, 2, 2, 1);

	/* FIXME: This bit is not documented for A10, and altering it doesn't
	 * seem to change anything.
	 *
	 * #define CCM_PLL5_CTRL_VCO_GAIN (0x1 << 19)
	 * reg_val = read32(&ccm->pll5_cfg);
	 * reg_val &= ~CCM_PLL5_CTRL_VCO_GAIN;  // PLL VCO Gain off
	 * write32(reg_val, &ccm->pll5_cfg);
	 */
	udelay(5500);

	a1x_pll5_enable_dram_clock_output();

	/* reset GPS */
	/* FIXME: These bits are also undocumented, and seem to have no effect
	 * on A10.
	 *
	 * #define CCM_GPS_CTRL_RESET (0x1 << 0)
	 * #define CCM_GPS_CTRL_GATE (0x1 << 1)
	 * clrbits_le32(&ccm->gps_clk_cfg, CCM_GPS_CTRL_RESET | CCM_GPS_CTRL_GATE);
	 */
	a1x_periph_clock_enable(A1X_CLKEN_GPS);
	udelay(1);
	a1x_periph_clock_disable(A1X_CLKEN_GPS);

	/* setup MBUS clock */
	/* FIXME: The MBUS does not seem to be present or do anything on A10. It
	 * is documented in the A13 user manual, but changing settings on A10
	 * has no effect.
	 *
	 * #define CCM_MBUS_CTRL_M(n) (((n) & 0xf) << 0)
	 * #define CCM_MBUS_CTRL_M_MASK CCM_MBUS_CTRL_M(0xf)
	 * #define CCM_MBUS_CTRL_M_X(n) ((n) - 1)
	 * #define CCM_MBUS_CTRL_N(n) (((n) & 0xf) << 16)
	 * #define CCM_MBUS_CTRL_N_MASK CCM_MBUS_CTRL_N(0xf)
	 * #define CCM_MBUS_CTRL_N_X(n) (((n) >> 3) ? 3 : (((n) >> 2) ? 2 : (((n) >> 1) ? 1 : 0)))
	 * #define CCM_MBUS_CTRL_CLK_SRC(n) (((n) & 0x3) << 24)
	 * #define CCM_MBUS_CTRL_CLK_SRC_MASK CCM_MBUS_CTRL_CLK_SRC(0x3)
	 * #define CCM_MBUS_CTRL_CLK_SRC_HOSC 0x0
	 * #define CCM_MBUS_CTRL_CLK_SRC_PLL6 0x1
	 * #define CCM_MBUS_CTRL_CLK_SRC_PLL5 0x2
	 * #define CCM_MBUS_CTRL_GATE (0x1 << 31)
	 * reg_val = CCM_MBUS_CTRL_GATE |
	 *           CCM_MBUS_CTRL_CLK_SRC(CCM_MBUS_CTRL_CLK_SRC_PLL5) |
	 *           CCM_MBUS_CTRL_N(CCM_MBUS_CTRL_N_X(1)) |
	 *           CCM_MBUS_CTRL_M(CCM_MBUS_CTRL_M_X(2));
	 * write32(reg_val, &ccm->mbus_clk_cfg);
	 */
	/*
	 * open DRAMC AHB & DLL register clock
	 * close it first
	 */
	a1x_periph_clock_disable(A1X_CLKEN_SDRAM);

	udelay(22);

	/* then open it */
	a1x_periph_clock_enable(A1X_CLKEN_SDRAM);
	udelay(22);
}

static int dramc_scan_readpipe(void)
{
	u32 reg32;

	/* data training trigger */
	setbits_le32(&dram->ccr, DRAM_CCR_DATA_TRAINING);

	/* check whether data training process has completed */
	while (read32(&dram->ccr) & DRAM_CCR_DATA_TRAINING) ;

	/* check data training result */
	reg32 = read32(&dram->csr);
	if (reg32 & DRAM_CSR_FAILED)
		return -1;

	return 0;
}

static int dramc_scan_dll_para(void)
{
	const u32 dqs_dly[7] = { 0x3, 0x2, 0x1, 0x0, 0xe, 0xd, 0xc };
	const u32 clk_dly[15] = { 0x07, 0x06, 0x05, 0x04, 0x03,
		0x02, 0x01, 0x00, 0x08, 0x10,
		0x18, 0x20, 0x28, 0x30, 0x38
	};
	u32 clk_dqs_count[15];
	u32 dqs_i, clk_i, cr_i;
	u32 max_val, min_val;
	u32 dqs_index, clk_index;

	/* Find DQS_DLY Pass Count for every CLK_DLY */
	for (clk_i = 0; clk_i < 15; clk_i++) {
		clk_dqs_count[clk_i] = 0;
		clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
				(clk_dly[clk_i] & 0x3f) << 6);
		for (dqs_i = 0; dqs_i < 7; dqs_i++) {
			for (cr_i = 1; cr_i < 5; cr_i++) {
				clrsetbits_le32(&dram->dllcr[cr_i],
						0x4f << 14,
						(dqs_dly[dqs_i] & 0x4f) << 14);
			}
			udelay(2);
			if (dramc_scan_readpipe() == 0)
				clk_dqs_count[clk_i]++;
		}
	}
	/* Test DQS_DLY Pass Count for every CLK_DLY from up to down */
	for (dqs_i = 15; dqs_i > 0; dqs_i--) {
		max_val = 15;
		min_val = 15;
		for (clk_i = 0; clk_i < 15; clk_i++) {
			if (clk_dqs_count[clk_i] == dqs_i) {
				max_val = clk_i;
				if (min_val == 15)
					min_val = clk_i;
			}
		}
		if (max_val < 15)
			break;
	}

	/* Check if Find a CLK_DLY failed */
	if (!dqs_i)
		goto fail;

	/* Find the middle index of CLK_DLY */
	clk_index = (max_val + min_val) >> 1;
	if ((max_val == (15 - 1)) && (min_val > 0))
		/* if CLK_DLY[MCTL_CLK_DLY_COUNT] is very good, then the middle
		 * value can be more close to the max_val
		 */
		clk_index = (15 + clk_index) >> 1;
	else if ((max_val < (15 - 1)) && (min_val == 0))
		/* if CLK_DLY[0] is very good, then the middle value can be more
		 * close to the min_val
		 */
		clk_index >>= 1;
	if (clk_dqs_count[clk_index] < dqs_i)
		clk_index = min_val;

	/* Find the middle index of DQS_DLY for the CLK_DLY got above, and Scan
	 * read pipe again
	 */
	clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
			(clk_dly[clk_index] & 0x3f) << 6);
	max_val = 7;
	min_val = 7;
	for (dqs_i = 0; dqs_i < 7; dqs_i++) {
		clk_dqs_count[dqs_i] = 0;
		for (cr_i = 1; cr_i < 5; cr_i++) {
			clrsetbits_le32(&dram->dllcr[cr_i],
					0x4f << 14,
					(dqs_dly[dqs_i] & 0x4f) << 14);
		}
		udelay(2);
		if (dramc_scan_readpipe() == 0) {
			clk_dqs_count[dqs_i] = 1;
			max_val = dqs_i;
			if (min_val == 7)
				min_val = dqs_i;
		}
	}

	if (max_val < 7) {
		dqs_index = (max_val + min_val) >> 1;
		if ((max_val == (7 - 1)) && (min_val > 0))
			dqs_index = (7 + dqs_index) >> 1;
		else if ((max_val < (7 - 1)) && (min_val == 0))
			dqs_index >>= 1;
		if (!clk_dqs_count[dqs_index])
			dqs_index = min_val;
		for (cr_i = 1; cr_i < 5; cr_i++) {
			clrsetbits_le32(&dram->dllcr[cr_i],
					0x4f << 14,
					(dqs_dly[dqs_index] & 0x4f) << 14);
		}
		udelay(2);
		return dramc_scan_readpipe();
	}

 fail:
	clrbits_le32(&dram->dllcr[0], 0x3f << 6);
	for (cr_i = 1; cr_i < 5; cr_i++)
		clrbits_le32(&dram->dllcr[cr_i], 0x4f << 14);
	udelay(2);

	return dramc_scan_readpipe();
}

static void dramc_set_autorefresh_cycle(u32 clk)
{
	u32 reg32;
	u32 tmp_val;
	u32 reg_dcr;

	if (clk < 600) {
		reg_dcr = read32(&dram->dcr);
		if ((reg_dcr & DRAM_DCR_CHIP_DENSITY_MASK) <=
		    DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_1024M))
			reg32 = (131 * clk) >> 10;
		else
			reg32 = (336 * clk) >> 10;

		tmp_val = (7987 * clk) >> 10;
		tmp_val = tmp_val * 9 - 200;
		reg32 |= tmp_val << 8;
		reg32 |= 0x8 << 24;
		write32(&dram->drr, reg32);
	} else {
		write32(&dram->drr, 0x0);
	}
}

unsigned long dramc_init(struct dram_para *para)
{
	u32 reg32;
	int ret_val;

	/* check input dram parameter structure */
	if (!para)
		return 0;

	/* setup DRAM relative clock */
	mctl_setup_dram_clock(para->clock);

	/* reset external DRAM */
	mctl_ddr3_reset();

	mctl_set_drive();

	/* dram clock off */
	a1x_gate_dram_clock_output();

	/* select dram controller 1 */
	write32(&dram->csel, DRAM_CSEL_MAGIC);

	mctl_itm_disable();
	mctl_enable_dll0(para->tpr3);

	/* configure external DRAM */
	reg32 = 0x0;
	if (para->type == DRAM_MEMORY_TYPE_DDR3)
		reg32 |= DRAM_DCR_TYPE_DDR3;
	reg32 |= DRAM_DCR_IO_WIDTH(para->io_width >> 3);

	if (para->density == 256)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_256M);
	else if (para->density == 512)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_512M);
	else if (para->density == 1024)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_1024M);
	else if (para->density == 2048)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_2048M);
	else if (para->density == 4096)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_4096M);
	else if (para->density == 8192)
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_8192M);
	else
		reg32 |= DRAM_DCR_CHIP_DENSITY(DRAM_DCR_CHIP_DENSITY_256M);

	reg32 |= DRAM_DCR_BUS_WIDTH((para->bus_width >> 3) - 1);
	reg32 |= DRAM_DCR_RANK_SEL(para->rank_num - 1);
	reg32 |= DRAM_DCR_CMD_RANK_ALL;
	reg32 |= DRAM_DCR_MODE(DRAM_DCR_MODE_INTERLEAVE);
	write32(&dram->dcr, reg32);

	/* dram clock on */
	a1x_ungate_dram_clock_output();

	udelay(1);

	while (read32(&dram->ccr) & DRAM_CCR_INIT) ;

	mctl_enable_dllx(para->tpr3);

	/* set odt impendance divide ratio */
	reg32 = ((para->zq) >> 8) & 0xfffff;
	reg32 |= ((para->zq) & 0xff) << 20;
	reg32 |= (para->zq) & 0xf0000000;
	write32(&dram->zqcr0, reg32);

	/* set I/O configure register */
	reg32 = 0x00cc0000;
	reg32 |= (para->odt_en) & 0x3;
	reg32 |= ((para->odt_en) & 0x3) << 30;
	write32(&dram->iocr, reg32);

	/* set refresh period */
	dramc_set_autorefresh_cycle(para->clock);

	/* set timing parameters */
	write32(&dram->tpr0, para->tpr0);
	write32(&dram->tpr1, para->tpr1);
	write32(&dram->tpr2, para->tpr2);

	if (para->type == DRAM_MEMORY_TYPE_DDR3) {
		reg32 = DRAM_MR_BURST_LENGTH(0x0);
		reg32 |= DRAM_MR_CAS_LAT(para->cas - 4);
		reg32 |= DRAM_MR_WRITE_RECOVERY(0x5);
	} else if (para->type == DRAM_MEMORY_TYPE_DDR2) {
		reg32 = DRAM_MR_BURST_LENGTH(0x2);
		reg32 |= DRAM_MR_CAS_LAT(para->cas);
		reg32 |= DRAM_MR_WRITE_RECOVERY(0x5);
	}
	write32(&dram->mr, reg32);

	write32(&dram->emr, para->emr1);
	write32(&dram->emr2, para->emr2);
	write32(&dram->emr3, para->emr3);

	/* set DQS window mode */
	clrsetbits_le32(&dram->ccr, DRAM_CCR_DQS_DRIFT_COMP, DRAM_CCR_DQS_GATE);

	/* reset external DRAM */
	setbits_le32(&dram->ccr, DRAM_CCR_INIT);
	while (read32(&dram->ccr) & DRAM_CCR_INIT) ;

	/* scan read pipe value */
	mctl_itm_enable();
	if (para->tpr3 & (0x1 << 31)) {
		ret_val = dramc_scan_dll_para();
		if (ret_val == 0)
			para->tpr3 =
			    (((read32(&dram->dllcr[0]) >> 6) & 0x3f) << 16) |
			    (((read32(&dram->dllcr[1]) >> 14) & 0xf) << 0) |
			    (((read32(&dram->dllcr[2]) >> 14) & 0xf) << 4) |
			    (((read32(&dram->dllcr[3]) >> 14) & 0xf) << 8) |
			    (((read32(&dram->dllcr[4]) >> 14) & 0xf) << 12);
	} else {
		ret_val = dramc_scan_readpipe();
	}

	if (ret_val < 0)
		return 0;

	/* configure all host port */
	mctl_configure_hostport();

	return para->size;
}
