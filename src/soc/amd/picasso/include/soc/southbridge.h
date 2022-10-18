/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_SOUTHBRIDGE_H
#define AMD_PICASSO_SOUTHBRIDGE_H

#include <types.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>

/*
 * AcpiMmio Region
 *  - fixed addresses offset from 0xfed80000
 */

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PM_PCI_CTRL			0x08
#define   FORCE_SLPSTATE_RETRY		BIT(25)

#define PWR_RESET_CFG			0x10
#define   TOGGLE_ALL_PWR_GOOD		BIT(1)

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
#define PM_CPU_CTRL			0x66
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
#define   PM_ACPI_BLOCK_PCIE_PME	BIT(24)
#define   PM_ACPI_PCIE_WAK_MASK		BIT(25)
#define   PM_ACPI_WAKE_AS_GEVENT	BIT(27)
#define   PM_ACPI_NB_PME_GEVENT		BIT(28)
#define   PM_ACPI_RTC_WAKE_EN		BIT(29)
#define PM_LPC_GATING			0xec
#define   PM_LPC_AB_NO_BYPASS_EN	BIT(2)
#define   PM_LPC_A20_EN			BIT(1)
#define   PM_LPC_ENABLE			BIT(0)

#define PM1_LIMIT			16
#define GPE0_LIMIT			32
#define TOTAL_BITS(a)			(8 * sizeof(a))

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

#define MISC_CGPLL_CONFIG1		0x08
#define   CG1PLL_SPREAD_SPECTRUM_ENABLE	BIT(0)
#define MISC_CLK_CNTL1			0x40
#define   BP_X48M0_OUTPUT_EN		BIT(2) /* 1=En, unlike Hudson, Kern */

#define FCH_LEGACY_UART_DECODE		(ALINK_AHB_ADDRESS + 0x20) /* 0xfedc0020 */

/* SATA Controller D11F0 */
#define SATA_MISC_CONTROL_REG		0x40
#define SATA_MISC_SUBCLASS_WREN		BIT(0)
/* Register in AHCIBaseAddress (BAR5 at D11F0x24) */
#define SATA_CAPABILITIES_REG		0xfc
#define SATA_CAPABILITY_SPM		BIT(12)

void fch_pre_init(void);
void fch_early_init(void);
void fch_init(void *chip_info);
void fch_final(void *chip_info);

#endif /* AMD_PICASSO_SOUTHBRIDGE_H */
