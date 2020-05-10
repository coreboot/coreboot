/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <delay.h>
#include <soc/blsp.h>
#include <soc/clock.h>
#include <types.h>
#include <console/console.h>

#define CLOCK_UPDATE_DELAY		1000

/**
 * uart_clock_config - configures UART clocks
 *
 * Configures GSBI UART dividers, enable root and branch clocks.
 */
void uart_clock_config(unsigned int blsp_uart, unsigned int m,
		unsigned int n, unsigned int d)
{
	int i;

	/* Setup M, N & D */
	write32(GCC_BLSP1_UART_APPS_M(blsp_uart), m);
	write32(GCC_BLSP1_UART_APPS_N(blsp_uart), ~(n - m));
	write32(GCC_BLSP1_UART_APPS_D(blsp_uart), ~d);
	write32(GCC_BLSP1_UART_MISC(blsp_uart), 0);

	/* Setup source sel etc. */
	write32(GCC_BLSP1_UART_APPS_CFG_RCGR(blsp_uart),
			0 |		/*  0: 4 SRC_DIV = Bypass */
			0 << 8 |	/*  8:10 SRC_SEL = CxO */
			2 << 12);	/* 13:12 Mode = Dual Edge */

	/* Trigger update */
	setbits32(GCC_BLSP1_UART_APPS_CMD_RCGR(blsp_uart), 1);

	/* Wait for update */
	for (i = 0; i < CLOCK_UPDATE_DELAY; i++) {
		if (!(read32(GCC_BLSP1_UART_APPS_CMD_RCGR(blsp_uart)) & 1)) {
			/* Updated */
			break;
		}
		udelay(1);
	}

	/* Please refer to the comments in blsp_i2c_clock_config() */
	setbits32(GCC_CLK_BRANCH_ENA, BLSP1_AHB | BLSP1_SLEEP);
}

/**
 * nand_clock_config - configure NAND controller clocks
 *
 * Enable clocks to EBI2. Must be invoked before touching EBI2
 * registers.
 */
void nand_clock_config(void)
{
	write32(EBI2_CLK_CTL_REG,
		CLK_BRANCH_ENA(1) | ALWAYS_ON_CLK_BRANCH_ENA(1));

	/* Wait for clock to stabilize. */
	udelay(10);
}

/**
 * usb_clock_config - configure USB controller clocks and reset the controller
 */
void usb_clock_config(void)
{
	/* Magic clock initialization numbers, nobody knows how they work... */
	write32(USB30_MASTER_CLK_CTL_REG, 0x10);
	write32(USB30_1_MASTER_CLK_CTL_REG, 0x10);
	write32(USB30_MASTER_CLK_MD, 0x500DF);
	write32(USB30_MASTER_CLK_NS, 0xE40942);
	write32(USB30_MOC_UTMI_CLK_MD, 0x100D7);
	write32(USB30_MOC_UTMI_CLK_NS, 0xD80942);
	write32(USB30_MOC_UTMI_CLK_CTL, 0x10);
	write32(USB30_1_MOC_UTMI_CLK_CTL, 0x10);

	write32(USB30_RESET,
		1 << 5 |		/* assert port2 HS PHY async reset */
		1 << 4 |		/* assert master async reset */
		1 << 3 |		/* assert sleep async reset */
		1 << 2 |		/* assert MOC UTMI async reset */
		1 << 1 |		/* assert power-on async reset */
		1 << 0);		/* assert PHY async reset */
	udelay(5);
	write32(USB30_RESET, 0);	/* deassert all USB resets again */
}

int blsp_i2c_clock_config(blsp_qup_id_t id)
{
	int i;
	const int max_tries = 200;
	struct { void *cbcr, *cmd, *cfg; } clk[] = {
		{
			GCC_BLSP1_QUP1_I2C_APPS_CBCR,
			GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR,
			GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR,
		},
		{
			GCC_BLSP1_QUP1_I2C_APPS_CBCR,
			GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR,
			GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR,
		},
		{
			GCC_BLSP1_QUP1_I2C_APPS_CBCR,
			GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR,
			GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR,
		},
		{
			GCC_BLSP1_QUP1_I2C_APPS_CBCR,
			GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR,
			GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR,
		},
	};

	/*
	 * uart_clock_config() does this. Ideally, setting these bits once
	 * should suffice. However, if for some reason the order of invocation
	 * of uart_clock_config and blsp_i2c_clock_config gets changed or
	 * something, then one of the functions might not work. Hence, to steer
	 * clear of such dependencies, just replicating the setting of this
	 * bits.
	 *
	 * Moreover, they are read-modify-write and HW wise repeated setting of
	 * the same bits is harmless. Hence repeating them here should be ok.
	 * This will ensure root and branch clocks remain on.
	 */
	setbits32(GCC_CLK_BRANCH_ENA, BLSP1_AHB | BLSP1_SLEEP);

	/* Src Sel 1 (fepll 200), Src Div 10.5 */
	write32(clk[id].cfg, (1u << 8) | (20u << 0));

	write32(clk[id].cmd, BIT(0)); /* Update En */

	for (i = 0; i < max_tries; i++) {
		if (read32(clk[id].cmd) & BIT(0)) {
			udelay(5);
			continue;
		}
		break;
	}

	if (i == max_tries) {
		printk(BIOS_ERR, "%s failed\n", __func__);
		return -ETIMEDOUT;
	}

	write32(clk[id].cbcr, BIT(0));	/* Enable */

	return 0;
}
