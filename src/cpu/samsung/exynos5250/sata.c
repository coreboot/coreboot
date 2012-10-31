/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <ahci.h>
#include <common.h>
#include <fdtdec.h>
#include <scsi.h>
#include <asm/arch-exynos5/sata.h>
#include <asm/arch/pinmux.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/types.h>
#include <linux/compiler.h>

#define SATA_AHCI_AXI			0x122f0000
#define SATA_PHCTRL_APB			0x12170000
#define SATA_PHY_I2C_ABP		0x121d0000
#define EXYNOS5_SATA_PHY_CONTROL	(0x10040000 + 0x724)
#define S5P_PMU_SATA_PHY_CONTROL_EN	0x1

void * const phy_ctrl = (void *)SATA_PHCTRL_APB;
void * const phy_i2c_base = (void *)SATA_PHY_I2C_ABP;

typedef unsigned char bool;
#define true 1
#define false 0


#define SATA_TIME_LIMIT			10000
#define SATA_PHY_I2C_SLAVE_ADDRS	0x70

#define SATA_RESET			0x4
#define RESET_CMN_RST_N			(1 << 1)
#define LINK_RESET			0xF0000

#define SATA_MODE0			0x10

#define SATA_CTRL0			0x14
#define CTRL0_P0_PHY_CALIBRATED_SEL	(1 << 9)
#define CTRL0_P0_PHY_CALIBRATED		(1 << 8)

#define SATA_PHSATA_CTRLM		0xE0
#define PHCTRLM_REF_RATE		(1 << 1)
#define PHCTRLM_HIGH_SPEED		(1 << 0)

#define SATA_PHSATA_STATM		0xF0
#define PHSTATM_PLL_LOCKED		(1 << 0)


/********************** I2C**************/
#define SATA_I2C_CON			0x00
#define SATA_I2C_STAT			0x04
#define SATA_I2C_ADDR			0x08
#define SATA_I2C_DS			0x0C
#define SATA_I2C_LC			0x10

/* I2CCON reg */
#define CON_ACKEN			(1 << 7)
#define CON_CLK512			(1 << 6)
#define CON_CLK16			(~CON_CLK512)
#define CON_INTEN			(1 << 5)
#define CON_INTPND			(1 << 4)
#define CON_TXCLK_PS			(0xF)

/* I2CSTAT reg */
#define STAT_MSTT			(0x3 << 6)
#define STAT_BSYST			(1 << 5)
#define STAT_RTEN			(1 << 4)
#define STAT_LAST			(1 << 0)

#define LC_FLTR_EN			(1 << 2)

#define SATA_PHY_CON_RESET		0xF003F

#define SCLK_SATA_FREQ			(66 * MHZ)



enum {
	SATA_GENERATION1,
	SATA_GENERATION2,
	SATA_GENERATION3,
};

static bool sata_is_reg(void __iomem *base, u32 reg, u32 checkbit, u32 Status)
{
	if ((__raw_readl(base + reg) & checkbit) == Status)
		return true;
	else
		return false;
}

static bool wait_for_reg_status(void __iomem *base, u32 reg, u32 checkbit,
		u32 Status)
{
	u32 time_limit_cnt = 0;
	while (!sata_is_reg(base, reg, checkbit, Status)) {
		if (time_limit_cnt == SATA_TIME_LIMIT) {
			return false;
		}
		udelay(1000);
		time_limit_cnt++;
	}
	return true;
}


static void sata_set_gen(u8 gen)
{
	__raw_writel(gen, phy_ctrl + SATA_MODE0);
}

/* Address :I2C Address */
static void sata_i2c_write_addrs(u8 data)
{
	__raw_writeb((data & 0xFE), phy_i2c_base + SATA_I2C_DS);
}

static void sata_i2c_write_data(u8 data)
{
	__raw_writeb((data), phy_i2c_base + SATA_I2C_DS);
}

static void sata_i2c_start(void)
{
	u32 val;
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val |= STAT_BSYST;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);
}

static void sata_i2c_stop(void)
{
	u32 val;
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val &= ~STAT_BSYST;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);
}

static bool sata_i2c_get_int_status(void)
{
	if ((__raw_readl(phy_i2c_base + SATA_I2C_CON)) & CON_INTPND)
		return true;
	else
		return false;
}

static bool sata_i2c_is_tx_ack(void)
{
	if ((__raw_readl(phy_i2c_base + SATA_I2C_STAT)) & STAT_LAST)
		return false;
	else
		return true;
}

static bool sata_i2c_is_bus_ready(void)
{
	if ((__raw_readl(phy_i2c_base + SATA_I2C_STAT)) & STAT_BSYST)
		return false;
	else
		return true;
}

static bool sata_i2c_wait_for_busready(u32 time_out)
{
	while (--time_out) {
		if (sata_i2c_is_bus_ready())
			return true;
		udelay(100);
	}
	return false;
}

static bool sata_i2c_wait_for_tx_ack(u32 time_out)
{
	while (--time_out) {
		if (sata_i2c_get_int_status()) {
			if (sata_i2c_is_tx_ack())
				return true;
		}
		udelay(100);
	}
	return false;
}

static void sata_i2c_clear_int_status(void)
{
	u32 val;
	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val &= ~CON_INTPND;
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);
}


static void sata_i2c_set_ack_gen(bool enable)
{
	u32 val;
	if (enable) {
		val = (__raw_readl(phy_i2c_base + SATA_I2C_CON)) | CON_ACKEN;
		__raw_writel(val, phy_i2c_base + SATA_I2C_CON);
	} else {
		val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
		val &= ~CON_ACKEN;
		__raw_writel(val, phy_i2c_base + SATA_I2C_CON);
	}

}

static void sata_i2c_set_master_tx(void)
{
	u32 val;
	/* Disable I2C */
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val &= ~STAT_RTEN;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);
	/* Clear Mode */
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val &= ~STAT_MSTT;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);

	sata_i2c_clear_int_status();
	/* interrupt disable */
	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val &= ~CON_INTEN;
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);

	/* Master, Send mode */
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val |=	STAT_MSTT;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);

	/* interrupt enable */
	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val |=	CON_INTEN;
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);

	/* Enable I2C */
	val = __raw_readl(phy_i2c_base + SATA_I2C_STAT);
	val |= STAT_RTEN;
	__raw_writel(val, phy_i2c_base + SATA_I2C_STAT);
}

static void sata_i2c_init(void)
{
	u32 val;

	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val &= CON_CLK16;
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);

	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val &= ~(CON_TXCLK_PS);
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);

	val = __raw_readl(phy_i2c_base + SATA_I2C_CON);
	val |= (2 & CON_TXCLK_PS);
	__raw_writel(val, phy_i2c_base + SATA_I2C_CON);

	val = __raw_readl(phy_i2c_base + SATA_I2C_LC);
	val &= ~(LC_FLTR_EN);
	__raw_writel(val, phy_i2c_base + SATA_I2C_LC);

	sata_i2c_set_ack_gen(false);
}
static bool sata_i2c_send(u8 slave_addrs, u8 addrs, u8 ucData)
{
	s32 ret = 0;
	if (!sata_i2c_wait_for_busready(SATA_TIME_LIMIT))
		return false;

	sata_i2c_init();
	sata_i2c_set_master_tx();

	__raw_writel(SATA_PHY_CON_RESET, phy_ctrl + SATA_RESET);
	sata_i2c_write_addrs(slave_addrs);
	sata_i2c_start();
	if (!sata_i2c_wait_for_tx_ack(SATA_TIME_LIMIT)) {
		ret = false;
		goto STOP;
	}
	sata_i2c_write_data(addrs);
	sata_i2c_clear_int_status();
	if (!sata_i2c_wait_for_tx_ack(SATA_TIME_LIMIT)) {
		ret = false;
		goto STOP;
	}
	sata_i2c_write_data(ucData);
	sata_i2c_clear_int_status();
	if (!sata_i2c_wait_for_tx_ack(SATA_TIME_LIMIT)) {
		ret = false;
		goto STOP;
	}
	ret = true;

STOP:
	sata_i2c_stop();
	sata_i2c_clear_int_status();
	sata_i2c_wait_for_busready(SATA_TIME_LIMIT);

	return ret;
}

static bool ahci_phy_init(void __iomem *mmio)
{
	u8 uCount, i = 0;
	/* 0x3A for 40bit I/F */
	u8 reg_addrs[] = {0x22, 0x21, 0x3A};
	/* 0x0B for 40bit I/F */
	u8 default_setting_value[] = {0x30, 0x4f, 0x0B};

	uCount = sizeof(reg_addrs)/sizeof(u8);
	while (i < uCount) {
		if (!sata_i2c_send(SATA_PHY_I2C_SLAVE_ADDRS, reg_addrs[i],
					default_setting_value[i]))
			return false;
		i++;
	}
	return true;
}

static int exynos5_ahci_init(void __iomem *mmio)
{
	int val, ret;

	__raw_writel(S5P_PMU_SATA_PHY_CONTROL_EN, EXYNOS5_SATA_PHY_CONTROL);

	val = 0;
	__raw_writel(val, phy_ctrl + SATA_RESET);
	val = __raw_readl(phy_ctrl + SATA_RESET);
	val |= 0x3D;
	__raw_writel(val, phy_ctrl + SATA_RESET);

	val = __raw_readl(phy_ctrl + SATA_RESET);
	val |= LINK_RESET;
	__raw_writel(val, phy_ctrl + SATA_RESET);

	val = __raw_readl(phy_ctrl + SATA_RESET);
	val |= RESET_CMN_RST_N;
	__raw_writel(val, phy_ctrl + SATA_RESET);

	val = __raw_readl(phy_ctrl + SATA_PHSATA_CTRLM);
	val &= ~PHCTRLM_REF_RATE;
	__raw_writel(val, phy_ctrl + SATA_PHSATA_CTRLM);

	/* High speed enable for Gen3 */
	val = __raw_readl(phy_ctrl + SATA_PHSATA_CTRLM);
	val |= PHCTRLM_HIGH_SPEED;
	__raw_writel(val, phy_ctrl + SATA_PHSATA_CTRLM);

	/* Port0 is available */
	__raw_writel(0x1, mmio + HOST_PORTS_IMPL);

	ret = ahci_phy_init(mmio);

	val = __raw_readl(phy_ctrl + SATA_CTRL0);
	val |= CTRL0_P0_PHY_CALIBRATED_SEL|CTRL0_P0_PHY_CALIBRATED;
	__raw_writel(val, phy_ctrl + SATA_CTRL0);
	sata_set_gen(SATA_GENERATION3);

	/* release cmu reset */
	val = __raw_readl(phy_ctrl + SATA_RESET);
	val &= ~RESET_CMN_RST_N;
	__raw_writel(val, phy_ctrl + SATA_RESET);

	val = __raw_readl(phy_ctrl + SATA_RESET);
	val |= RESET_CMN_RST_N;
	__raw_writel(val, phy_ctrl + SATA_RESET);

	if (wait_for_reg_status(phy_ctrl, SATA_PHSATA_STATM,
				PHSTATM_PLL_LOCKED, 1)) {
		return ret;
	}
	return 0;
}

static int exynos5_sata_enable_power(const void *blob)
{
	int node;
	struct fdt_gpio_state gpio;

	node = fdtdec_next_compatible(blob, 0, COMPAT_GOOGLE_SATA);
	if (node >= 0 &&
	    fdtdec_decode_gpio(blob, node, "enable-gpios", &gpio) == 0) {
		gpio_cfg_pin(gpio.gpio, EXYNOS_GPIO_OUTPUT);
		gpio_set_value(gpio.gpio, 1);
		return 0;
	}
	return -ENODEV;
}

static void exynos5_enable_clock_gates(void)
{
	/* Turn on all SATA clock gates & DMA gates.  */
	const unsigned cmu_toppart	= 0x10020000;
	const unsigned addr		= cmu_toppart + 0x944;
	const unsigned sata_clocks	= (1 << 25) | (1 << 24) | (1 << 6);
	const unsigned dma_clocks	= (2 << 1) | (1 << 1);
	const unsigned clk_gate_ip_fsys = readl(addr);
	writel(clk_gate_ip_fsys | sata_clocks | dma_clocks, addr);
}

int exynos5_sata_init(const void *blob)
{
	if (exynos5_sata_enable_power(blob) == 0) {
		exynos5_enable_clock_gates();

		if (exynos5_ahci_init((void *)SATA_AHCI_AXI)) {
			ahci_init(SATA_AHCI_AXI);
			scsi_scan(1);
			return 0;
		}
	}
	return -ENODEV;
}
