/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_STONEYRIDGE_SOUTHBRIDGE_H
#define AMD_STONEYRIDGE_SOUTHBRIDGE_H

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
#define   FORCE_STPCLK_RETRY		BIT(24)

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
#define PM_PCIB_CFG			0xea
#define   PM_GENINT_DISABLE		BIT(0)
#define PM_LPC_GATING			0xec
#define   PM_LPC_AB_NO_BYPASS_EN	BIT(2)
#define   PM_LPC_A20_EN			BIT(1)
#define   PM_LPC_ENABLE			BIT(0)
#define PM_USB_ENABLE			0xef
#define   PM_USB_ALL_CONTROLLERS	0x7f

/* FCH MISC Registers 0xfed80e00 */
#define GPP_CLK_CNTRL			0x00
#define   GPP_CLK2_REQ_MAP_SHIFT	8
#define   GPP_CLK2_REQ_MAP_MASK		(0xf << GPP_CLK2_REQ_MAP_SHIFT)
#define   GPP_CLK2_REQ_MAP_CLK_REQ2	3
#define   GPP_CLK0_REQ_MAP_SHIFT	0
#define   GPP_CLK0_REQ_MAP_MASK		(0xf << GPP_CLK0_REQ_MAP_SHIFT)
#define   GPP_CLK0_REQ_MAP_CLK_REQ0	1
#define MISC_CGPLL_CONFIG1		0x08
#define   CG1PLL_SPREAD_SPECTRUM_ENABLE	BIT(0)
#define MISC_CGPLL_CONFIG3		0x10
#define   CG1PLL_REFDIV_SHIFT		0
#define   CG1PLL_REFDIV_MASK		(0x3ff << CG1PLL_REFDIV_SHIFT)
#define   CG1PLL_FBDIV_SHIFT		10
#define   CG1PLL_FBDIV_MASK		(0xfff << CG1PLL_FBDIV_SHIFT)
#define MISC_CGPLL_CONFIG4		0x14
#define   SS_STEP_SIZE_DSFRAC_SHIFT	0
#define   SS_STEP_SIZE_DSFRAC_MASK	(0xffff << SS_STEP_SIZE_DSFRAC_SHIFT)
#define   SS_AMOUNT_DSFRAC_SHIFT	16
#define   SS_AMOUNT_DSFRAC_MASK		(0xffff << SS_AMOUNT_DSFRAC_SHIFT)
#define MISC_CGPLL_CONFIG5		0x18
#define   SS_AMOUNT_NFRAC_SLIP_SHIFT	8
#define   SS_AMOUNT_NFRAC_SLIP_MASK	(0xf << SS_AMOUNT_NFRAC_SLIP_SHIFT)
#define MISC_CGPLL_CONFIG6		0x1c
#define   CG1PLL_LF_MODE_SHIFT		9
#define   CG1PLL_LF_MODE_MASK		(0x1ff << CG1PLL_LF_MODE_SHIFT)
#define MISC_CLK_CNTL1			0x40
#define   CG1PLL_FBDIV_TEST		BIT(26)
#define   OSCOUT1_CLK_OUTPUT_ENB	BIT(2)  /* 0 = Enabled, 1 = Disabled */
#define   OSCOUT2_CLK_OUTPUT_ENB	BIT(7)  /* 0 = Enabled, 1 = Disabled */

/* XHCI_PM Registers:  0xfed81c00 */
#define XHCI_PM_INDIRECT_INDEX		0x48
#define XHCI_PM_INDIRECT_DATA		0x4c
#define   XHCI_OVER_CURRENT_CONTROL	0x30
#define     USB_OC0			0
#define     USB_OC1			1
#define     USB_OC2			2
#define     USB_OC3			3
#define     USB_OC4			4
#define     USB_OC5			5
#define     USB_OC6			6
#define     USB_OC7			7
#define     USB_OC_DISABLE		0xf
#define     USB_OC_DISABLE_ALL		0xffff
#define     OC_PORT0_SHIFT		0
#define     OC_PORT1_SHIFT		4
#define     OC_PORT2_SHIFT		8
#define     OC_PORT3_SHIFT		12

#define EHCI_OVER_CURRENT_CONTROL	0x70
#define EHCI_HUB_CONFIG4		0x90
#define   DEBUG_PORT_SELECT_SHIFT	  16
#define   DEBUG_PORT_ENABLE		  BIT(18)
#define   DEBUG_PORT_MASK		(BIT(16) | BIT(17) | BIT(18))

#define PM1_LIMIT			16
#define GPE0_LIMIT			28
#define TOTAL_BITS(a)			(8 * sizeof(a))

/* SATA Controller D11F0 */
#define SATA_MISC_CONTROL_REG		0x40
#define SATA_MISC_SUBCLASS_WREN		BIT(0)
/* Register in AHCIBaseAddress (BAR5 at D11F0x24) */
#define SATA_CAPABILITIES_REG		0xfc
#define SATA_CAPABILITY_SPM		BIT(12)

/* Platform Security Processor D8F0 */
void soc_enable_psp_early(void);

#define PSP_MAILBOX_BAR			PCI_BASE_ADDRESS_4 /* BKDG: "BAR3" */

#define PSP_BAR_ENABLES			0x48
#define  BAR3HIDE			BIT(12) /* Bit to hide BAR3 addr */
#define  PSP_MAILBOX_BAR_EN		BIT(4)

typedef struct aoac_devs {
	unsigned int :5;
	unsigned int ic0e:1; /* 5: I2C0 */
	unsigned int ic1e:1; /* 6: I2C1 */
	unsigned int ic2e:1; /* 7: I2C2 */
	unsigned int ic3e:1; /* 8: I2C3 */
	unsigned int :2;
	unsigned int ut0e:1; /* 11: UART0 */
	unsigned int ut1e:1; /* 12: UART1 */
	unsigned int :2;
	unsigned int st_e:1; /* 15: SATA */
	unsigned int :2;
	unsigned int ehce:1; /* 18: EHCI */
	unsigned int :4;
	unsigned int xhce:1; /* 23: xHCI */
	unsigned int sd_e:1; /* 24: SDIO */
	unsigned int :2;
	unsigned int espi:1; /* 27: ESPI */
	unsigned int :4;
} __packed aoac_devs_t;

#define XHCI_FW_SIG_OFFSET			0xc
#define XHCI_FW_ADDR_OFFSET			0x6
#define XHCI_FW_SIZE_OFFSET			0x8
#define XHCI_FW_BOOTRAM_SIZE			0x8000

void bootblock_fch_early_init(void);
void bootblock_fch_init(void);
void fch_init(void *chip_info);
void fch_final(void *chip_info);

void enable_aoac_devices(void);
void fch_clk_output_48Mhz(u32 osc);

void set_uart_config(unsigned int idx);

/*
 * Call the mainboard to get the USB Over Current Map. The mainboard
 * returns the map and 0 on Success or -1 on error or no map. There is
 * a default weak function in usb.c if the mainboard doesn't have any
 * over current support.
 */
int mainboard_get_xhci_oc_map(uint16_t *usb_oc_map);
int mainboard_get_ehci_oc_map(uint16_t *usb_oc_map);

#endif /* AMD_STONEYRIDGE_SOUTHBRIDGE_H */
