/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_SOUTHBRIDGE_H
#define AMD_CEZANNE_SOUTHBRIDGE_H

#include <soc/iomap.h>

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PM_ISACONTROL			0x04
#define   ABCLKGATEEN			BIT(16)
#define PM_PCI_CTRL			0x08
#define   FORCE_SLPSTATE_RETRY		BIT(25)
#define PWR_RESET_CFG			0x10
#define   TOGGLE_ALL_PWR_GOOD		(1 << 1)
#define PM_SERIRQ_CONF			0x54
#define   PM_SERIRQ_NUM_BITS_17		0x0000
#define   PM_SERIRQ_NUM_BITS_18		0x0004
#define   PM_SERIRQ_NUM_BITS_19		0x0008
#define   PM_SERIRQ_NUM_BITS_20		0x000c
#define   PM_SERIRQ_NUM_BITS_21		0x0010
#define   PM_SERIRQ_NUM_BITS_22		0x0014
#define   PM_SERIRQ_NUM_BITS_23		0x0018
#define   PM_SERIRQ_NUM_BITS_24		0x001c
#define   PM_SERIRQ_MODE		BIT(6)
#define   PM_SERIRQ_ENABLE		BIT(7)
#define PM_EVT_BLK			0x60
#define   WAK_STS			BIT(15) /*AcpiPmEvtBlkx00 Pm1Status */
#define   PCIEXPWAK_STS			BIT(14)
#define   RTC_STS			BIT(10)
#define   PWRBTN_STS			BIT(8)
#define   GBL_STS			BIT(5)
#define   BM_STS			BIT(4)
#define   TIMER_STS			BIT(0)
#define   PCIEXPWAK_DIS			BIT(14) /*AcpiPmEvtBlkx02 Pm1Enable */
#define   RTC_EN			BIT(10)
#define   PWRBTN_EN			BIT(8)
#define   GBL_EN			BIT(5)
#define   TIMER_STS			BIT(0)
#define PM1_CNT_BLK			0x62
#define PM_TMR_BLK			0x64
#define PM_GPE0_BLK			0x68
#define PM_ACPI_SMI_CMD			0x6a
#define PM_ACPI_CONF			0x74
#define   PM_ACPI_DECODE_STD		BIT(0)
#define   PM_ACPI_GLOBAL_EN		BIT(1)
#define   PM_ACPI_RTC_EN_EN		BIT(2)
#define   PM_ACPI_TIMER_EN_EN		BIT(4)
#define   PM_ACPI_MASK_ARB_DIS		BIT(6)
#define   PM_ACPI_BIOS_RLS		BIT(7)
#define   PM_ACPI_PWRBTNEN_EN		BIT(8)
#define   PM_ACPI_REDUCED_HW_EN		BIT(9)
#define   PM_ACPI_S5_LPC_PIN_MODE_SEL	BIT(10)
#define   PM_ACPI_S5_LPC_PIN_MODE	BIT(11)
#define   PM_ACPI_BLOCK_PCIE_PME	BIT(24)
#define   PM_ACPI_PCIE_WAK_MASK		BIT(25)
#define   PM_ACPI_WAKE_AS_GEVENT	BIT(27)
#define   PM_ACPI_NB_PME_GEVENT		BIT(28)
#define   PM_ACPI_RTC_WAKE_EN		BIT(29)
#define PM_SPI_PAD_PU_PD		0x90
#define   PM_ESPI_CS_USE_DATA2		BIT(16)
#define PM_LPC_GATING			0xec
#define   PM_LPC_AB_NO_BYPASS_EN	BIT(2)
#define   PM_LPC_A20_EN			BIT(1)
#define   PM_LPC_ENABLE			BIT(0)

#define PM1_LIMIT			16
#define GPE0_LIMIT			32
#define TOTAL_BITS(a)			(8 * sizeof(a))

#define FCH_LEGACY_UART_DECODE		(ALINK_AHB_ADDRESS + 0x20) /* 0xfedc0020 */

/* FCH MISC Registers 0xfed80e00 */
#define GPP_CLK_CNTRL			0x00
#define   GPP_CLK0_REQ_SHIFT		0
#define   GPP_CLK1_REQ_SHIFT		2
#define   GPP_CLK4_REQ_SHIFT		4
#define   GPP_CLK2_REQ_SHIFT		6
#define   GPP_CLK3_REQ_SHIFT		8
#define   GPP_CLK5_REQ_SHIFT		10
#define   GPP_CLK6_REQ_SHIFT		12
#define     GPP_CLK_OUTPUT_COUNT	7
#define   GPP_CLK_REQ_MASK(clk_shift)	(0x3 << (clk_shift))
#define   GPP_CLK_REQ_ON(clk_shift)	(0x3 << (clk_shift))
#define   GPP_CLK_REQ_EXT(clk_shift)	(0x1 << (clk_shift))
#define   GPP_CLK_REQ_OFF(clk_shift)	(0x0 << (clk_shift))

#define MISC_CLKGATEDCNTL		0x2c
#define   ALINKCLK_GATEOFFEN		BIT(16)
#define   BLINKCLK_GATEOFFEN		BIT(17)
#define   XTAL_PAD_S3_TURNOFF_EN	BIT(20)
#define   XTAL_PAD_S5_TURNOFF_EN	BIT(21)
#define MISC_CGPLL_CONFIGURATION0	0x30
#define   USB_PHY_CMCLK_S3_DIS		BIT(8)
#define   USB_PHY_CMCLK_S0I3_DIS	BIT(9)
#define   USB_PHY_CMCLK_S5_DIS		BIT(10)
#define MISC_CLK_CNTL0			0x40 /* named MISC_CLK_CNTL1 on Picasso */
#define   BP_X48M0_OUTPUT_EN		BIT(2) /* 1=En, unlike Hudson, Kern */
#define MISC_I2C0_PAD_CTRL		0xd8
#define MISC_I2C1_PAD_CTRL		0xdc
#define MISC_I2C2_PAD_CTRL		0xe0
#define MISC_I2C3_PAD_CTRL		0xe4
#define   I2C_PAD_CTRL_NG_MASK		(BIT(0) + BIT(1) + BIT(2) + BIT(3))
#define     I2C_PAD_CTRL_NG_NORMAL	0xc
#define   I2C_PAD_CTRL_RX_SEL_MASK	(BIT(4) + BIT(5))
#define     I2C_PAD_CTRL_RX_SHIFT	4
#define     I2C_PAD_CTRL_RX_SEL_OFF	(0 << I2C_PAD_CTRL_RX_SHIFT)
#define     I2C_PAD_CTRL_RX_SEL_3_3V	(1 << I2C_PAD_CTRL_RX_SHIFT)
#define     I2C_PAD_CTRL_RX_SEL_1_8V	(3 << I2C_PAD_CTRL_RX_SHIFT)
#define   I2C_PAD_CTRL_PULLDOWN_EN	BIT(6)
#define   I2C_PAD_CTRL_FALLSLEW_MASK	(BIT(7) + BIT(8))
#define     I2C_PAD_CTRL_FALLSLEW_SHIFT	7
#define     I2C_PAD_CTRL_FALLSLEW_STD	(0 << I2C_PAD_CTRL_FALLSLEW_SHIFT)
#define     I2C_PAD_CTRL_FALLSLEW_LOW	(1 << I2C_PAD_CTRL_FALLSLEW_SHIFT)
#define   I2C_PAD_CTRL_FALLSLEW_EN	BIT(9)
#define   I2C_PAD_CTRL_SPIKE_RC_EN	BIT(10)
#define   I2C_PAD_CTRL_SPIKE_RC_SEL	BIT(11) /* 0 = 50ns, 1 = 20ns */
#define   I2C_PAD_CTRL_CAP_DOWN		BIT(12)
#define   I2C_PAD_CTRL_CAP_UP		BIT(13)
#define   I2C_PAD_CTRL_RES_DOWN		BIT(14)
#define   I2C_PAD_CTRL_RES_UP		BIT(15)
#define   I2C_PAD_CTRL_BIOS_CRT_EN	BIT(16)
#define   I2C_PAD_CTRL_SPARE0		BIT(17)
#define   I2C_PAD_CTRL_SPARE1		BIT(18)

void fch_pre_init(void);
void fch_early_init(void);
void fch_init(void *chip_info);
void fch_final(void *chip_info);

void enable_aoac_devices(void);
void wait_for_aoac_enabled(unsigned int dev);

/* Allow the board to change the default I2C pad configuration */
void mainboard_i2c_override(int bus, uint32_t *pad_settings);

#endif /* AMD_CEZANNE_SOUTHBRIDGE_H */
