/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_STONEYRIDGE_SOUTHBRIDGE_H
#define AMD_STONEYRIDGE_SOUTHBRIDGE_H

#include <types.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include "chip.h"

/*
 * AcpiMmio Region
 *  - fixed addresses offset from 0xfed80000
 */

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PM_DECODE_EN			0x00
#define   CF9_IO_EN			BIT(1)
#define   LEGACY_IO_EN			BIT(0)
#define PM_ISA_CONTROL			0x04
#define   MMIO_EN			BIT(1)
#define PM_PCI_CTRL			0x08
#define   FORCE_SLPSTATE_RETRY		BIT(25)
#define   FORCE_STPCLK_RETRY		BIT(24)

#define SMB_ASF_IO_BASE			0x01 /* part of PM_DECODE_EN */

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

#define PM_RTC_SHADOW			0x5b	/* state when power resumes */
#define   PM_S5_AT_POWER_RECOVERY	0x04	/* S5 */
#define   PM_RESTORE_S0_IF_PREV_S0	0x07	/* S0 if previously at S0 */

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
#define PM_RST_CTRL1			0xbe
#define   SLPTYPE_CONTROL_EN		BIT(5)
#define PM_RST_STATUS			0xc0
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

/* FCH AOAC device offsets for AOAC_DEV_D3_CTL/AOAC_DEV_D3_STATE */
#define FCH_AOAC_DEV_CLK_GEN		0
#define FCH_AOAC_DEV_I2C0		5
#define FCH_AOAC_DEV_I2C1		6
#define FCH_AOAC_DEV_I2C2		7
#define FCH_AOAC_DEV_I2C3		8
#define FCH_AOAC_DEV_UART0		11
#define FCH_AOAC_DEV_UART1		12
#define FCH_AOAC_DEV_AMBA		17
#define FCH_AOAC_DEV_USB2		18
#define FCH_AOAC_DEV_USB3		23

#define PM1_LIMIT			16
#define GPE0_LIMIT			28
#define TOTAL_BITS(a)			(8 * sizeof(a))

/* SATA Controller D11F0 */
#define SATA_MISC_CONTROL_REG		0x40
#define SATA_MISC_SUBCLASS_WREN		BIT(0)
/* Register in AHCIBaseAddress (BAR5 at D11F0x24) */
#define SATA_CAPABILITIES_REG		0xfc
#define SATA_CAPABILITY_SPM		BIT(12)

#define SPI_CNTRL0			0x00
#define   SPI_BUSY			BIT(31)
#define   SPI_READ_MODE_MASK		(BIT(30) | BIT(29) | BIT(18))
/* Nominal is 16.7MHz on older devices, 33MHz on newer */
#define   SPI_READ_MODE_NOM		0x00000000
#define   SPI_READ_MODE_DUAL112		(          BIT(29)          )
#define   SPI_READ_MODE_QUAD114		(          BIT(29) | BIT(18))
#define   SPI_READ_MODE_DUAL122		(BIT(30)                    )
#define   SPI_READ_MODE_QUAD144		(BIT(30) |           BIT(18))
#define   SPI_READ_MODE_NORMAL66	(BIT(30) | BIT(29)          )
#define   SPI_READ_MODE_FAST		(BIT(30) | BIT(29) | BIT(18))
#define   SPI_ACCESS_MAC_ROM_EN		BIT(22)
#define   SPI_FIFO_PTR_CLR		BIT(20)
#define   SPI_ARB_ENABLE		BIT(19)
#define   EXEC_OPCODE			BIT(16)

#define SPI100_ENABLE			0x20
#define   SPI_USE_SPI100		BIT(0)

/* Use SPI_SPEED_16M-SPI_SPEED_66M below for the southbridge */
#define SPI100_SPEED_CONFIG		0x22
#define   SPI_SPEED_66M			(0x0)
#define   SPI_SPEED_33M			(                  BIT(0))
#define   SPI_SPEED_22M			(         BIT(1)         )
#define   SPI_SPEED_16M			(         BIT(1) | BIT(0))
#define   SPI_SPEED_100M		(BIT(2)                  )
#define   SPI_SPEED_800K		(BIT(2) |          BIT(0))
#define   SPI_NORM_SPEED_NEW_SH		12
#define   SPI_FAST_SPEED_NEW_SH		8
#define   SPI_ALT_SPEED_NEW_SH		4
#define   SPI_TPM_SPEED_NEW_SH		0

#define SPI100_HOST_PREF_CONFIG		0x2c
#define   SPI_RD4DW_EN_HOST		BIT(15)

/* Platform Security Processor D8F0 */
void soc_enable_psp_early(void);

#define PSP_MAILBOX_BAR			PCI_BASE_ADDRESS_4 /* BKDG: "BAR3" */
#define PSP_MAILBOX_OFFSET		0x70 /* offset from BAR3 value */

#define PSP_BAR_ENABLES			0x48
#define  PSP_MAILBOX_BAR_EN		0x10

#define MSR_CU_CBBCFG			0xc00110a2 /* PSP Pvt Blk Base Addr */
#define   BAR3HIDE			BIT(12) /* Bit to hide BAR3 addr */

/* IO 0xcf9 - Reset control port*/
#define   FULL_RST			BIT(3)
#define   RST_CMD			BIT(2)
#define   SYS_RST			BIT(1)

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

void enable_aoac_devices(void);
void sb_clk_output_48Mhz(u32 osc);
void sb_enable(struct device *dev);
void southbridge_final(void *chip_info);
void southbridge_init(void *chip_info);
void sb_read_mode(u32 mode);
void sb_set_spi100(u16 norm, u16 fast, u16 alt, u16 tpm);
void bootblock_fch_early_init(void);
void bootblock_fch_init(void);

/*
 * Call the mainboard to get the USB Over Current Map. The mainboard
 * returns the map and 0 on Success or -1 on error or no map. There is
 * a default weak function in usb.c if the mainboard doesn't have any
 * over current support.
 */
int mainboard_get_xhci_oc_map(uint16_t *usb_oc_map);
int mainboard_get_ehci_oc_map(uint16_t *usb_oc_map);

/* Initialize all the i2c buses that are marked with early init. */
void i2c_soc_early_init(void);

/* Initialize all the i2c buses that are not marked with early init. */
void i2c_soc_init(void);

#endif /* AMD_STONEYRIDGE_SOUTHBRIDGE_H */
