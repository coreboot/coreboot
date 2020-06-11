/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_SB_H__
#define __PICASSO_SB_H__

#include <types.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>

/*
 * AcpiMmio Region
 *  - fixed addresses offset from 0xfed80000
 */

/* SMBus controller registers:  0xfed80000 or D14F0 */
#define SMB_UART_CONFIG			0xfc
#define   SMB_UART3_1_8M		BIT(31) /* defaults are 0 = 48MHz */
#define   SMB_UART2_1_8M		BIT(30)
#define   SMB_UART1_1_8M		BIT(29)
#define   SMB_UART0_1_8M		BIT(28)
#define   SMB_UART_1_8M_SHIFT		28

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PM_DECODE_EN			0x00
#define   SMBUS_ASF_IO_EN		BIT(4)
#define   CF9_IO_EN			BIT(1)
#define   LEGACY_IO_EN			BIT(0)
#define SMB_ASF_IO_BASE			0x01 /* part of PM_DECODE_EN in PPR */
#define PM_ISA_CONTROL			0x04
#define   MMIO_EN			BIT(1)
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
#define PM_LPC_GATING			0xec
#define   PM_LPC_AB_NO_BYPASS_EN	BIT(2)
#define   PM_LPC_A20_EN			BIT(1)
#define   PM_LPC_ENABLE			BIT(0)
#define PM_USB_ENABLE			0xef
#define   PM_USB_ALL_CONTROLLERS	0x7f

/* SMBUS MMIO offsets 0xfed80a00 */
#define SMBHSTSTAT			0x0
#define   SMBHST_STAT_FAILED		BIT(4)
#define   SMBHST_STAT_COLLISION		BIT(3)
#define   SMBHST_STAT_ERROR		BIT(2)
#define   SMBHST_STAT_INTERRUPT		BIT(1)
#define   SMBHST_STAT_BUSY		BIT(0)
#define   SMBHST_STAT_CLEAR		0xff
#define   SMBHST_STAT_NOERROR		BIT(1)
#define   SMBHST_STAT_VAL_BITS		0x1f
#define   SMBHST_STAT_ERROR_BITS	0x1c

#define SMBSLVSTAT			0x1
#define   SMBSLV_STAT_ALERT		0x20
#define   SMBSLV_STAT_SHADOW2		0x10
#define   SMBSLV_STAT_SHADOW1		0x08
#define   SMBSLV_STAT_SLV_STS		0x04
#define   SMBSLV_STAT_SLV_INIT		0x02
#define   SMBSLV_STAT_SLV_BUSY		0x01
#define   SMBSLV_STAT_CLEAR		0x1f

#define SMBHSTCTRL			0x2
#define   SMBHST_CTRL_RST		0x80
#define   SMBHST_CTRL_STRT		0x40
#define   SMBHST_CTRL_QCK_RW		0x00
#define   SMBHST_CTRL_BTE_RW		0x04
#define   SMBHST_CTRL_BDT_RW		0x08
#define   SMBHST_CTRL_WDT_RW		0x0c
#define   SMBHST_CTRL_BLK_RW		0x14
#define   SMBHST_CTRL_MODE_BITS		0x1c
#define   SMBHST_CTRL_KILL		0x02
#define   SMBHST_CTRL_IEN		0x01

#define SMBHSTCMD			0x3
#define SMBHSTADDR			0x4
#define SMBHSTDAT0			0x5
#define SMBHSTDAT1			0x6
#define SMBHSTBLKDAT			0x7
#define SMBSLVCTRL			0x8
#define SMBSLVCMD_SHADOW		0x9
#define SMBSLVEVT			0xa
#define SMBSLVDAT			0xc
#define SMBTIMING			0xe

/* FCH MISC Registers 0xfed80e00 */
#define GPP_CLK_CNTRL			0x00
#define   GPP_CLK0_REQ_SHL		0
#define   GPP_CLK1_REQ_SHL		2
#define   GFX_CLK0_REQ_SHL		4
#define   GPP_CLK2_REQ_SHL		6
#define   GPP_CLK3_REQ_SHL		8
#define   GFX_CLK1_REQ_SHL		10
#define   GPP_CLK_REQ_MAP_MASK0		(3 << GPP_CLK0_REQ_SHL)
#define   GPP_CLK_REQ_MAP_CLK0		(1 << GPP_CLK0_REQ_SHL)
#define   GPP_CLK_REQ_MAP_MASK1		(3 << GPP_CLK1_REQ_SHL)
#define   GPP_CLK_REQ_MAP_CLK1		(1 << GPP_CLK1_REQ_SHL)
#define   GFX_CLK_REQ_MAP_MASK0		(3 << GFX_CLK0_REQ_SHL)
#define   GFX_CLK_REQ_MAP_CLK0		(1 << GFX_CLK0_REQ_SHL)
#define   GPP_CLK_REQ_MAP_MASK2		(3 << GPP_CLK2_REQ_SHL)
#define   GPP_CLK_REQ_MAP_CLK2		(1 << GPP_CLK2_REQ_SHL)
#define   GPP_CLK_REQ_MAP_MASK3		(3 << GPP_CLK3_REQ_SHL)
#define   GPP_CLK_REQ_MAP_CLK3		(1 << GPP_CLK3_REQ_SHL)
#define   GFX_CLK_REQ_MAP_MASK1		(3 << GPP_CLK1_REQ_SHL)
#define   GFX_CLK_REQ_MAP_CLK1		(1 << GPP_CLK1_REQ_SHL)
#define MISC_CGPLL_CONFIG1		0x08
#define   CG1PLL_SPREAD_SPECTRUM_ENABLE	BIT(0)
#define MISC_CLK_CNTL1			0x40
#define   BP_X48M0_OUTPUT_EN		BIT(2) /* 1=En, unlike Hudson, Kern */
#define   OSCOUT1_CLK_OUTPUT_ENB	BIT(2)  /* 0 = Enabled, 1 = Disabled */
#define   OSCOUT2_CLK_OUTPUT_ENB	BIT(7)  /* 0 = Enabled, 1 = Disabled */
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

/* FCH AOAC Registers 0xfed81e00 */
#define AOAC_DEV_D3_CTL(device)		(0x40 + device * 2)
#define AOAC_DEV_D3_STATE(device)	(AOAC_DEV_D3_CTL(device) + 1)

#define FCH_AOAC_DEV_CLK_GEN		0
#define FCH_AOAC_DEV_I2C2		7
#define FCH_AOAC_DEV_I2C3		8
#define FCH_AOAC_DEV_I2C4		9
#define FCH_AOAC_DEV_UART0		11
#define FCH_AOAC_DEV_UART1		12
#define FCH_AOAC_DEV_UART2		16
#define FCH_AOAC_DEV_AMBA		17
#define FCH_AOAC_DEV_UART3		26
#define FCH_AOAC_DEV_ESPI		27

/* Bit definitions for Device D3 Control AOACx0000[40...7E] step 2 */
#define   FCH_AOAC_TARGET_DEVICE_STATE (BIT(0) + BIT(1))
#define     FCH_AOAC_D0_UNINITIALIZED	0
#define     FCH_AOAC_D0_INITIALIZED	1
#define     FCH_AOAC_D1_2_3_WARM	2
#define     FCH_AOAC_D3_COLD		3
#define   FCH_AOAC_DEVICE_STATE		BIT(2)
#define   FCH_AOAC_PWR_ON_DEV		BIT(3)
#define   FCH_AOAC_SW_PWR_ON_RSTB	BIT(4)
#define   FCH_AOAC_SW_REF_CLK_OK	BIT(5)
#define   FCH_AOAC_SW_RST_B		BIT(6)
#define   FCH_AOAC_IS_SW_CONTROL	BIT(7)

/* Bit definitions for Device D3 State AOACx0000[41...7f] step 2 */
#define   FCH_AOAC_PWR_RST_STATE	BIT(0)
#define   FCH_AOAC_RST_CLK_OK_STATE	BIT(1)
#define   FCH_AOAC_RST_B_STATE		BIT(2)
#define   FCH_AOAC_DEV_OFF_GATING_STATE	BIT(3)
#define   FCH_AOAC_D3COLD		BIT(4)
#define   FCH_AOAC_CLK_OK_STATE		BIT(5)
#define   FCH_AOAC_STAT0		BIT(6)
#define   FCH_AOAC_STAT1		BIT(7)

#define FCH_UART_LEGACY_DECODE		0xfedc0020
#define   FCH_LEGACY_3F8_SH		3
#define   FCH_LEGACY_2F8_SH		1
#define   FCH_LEGACY_3E8_SH		2

#define PM1_LIMIT			16
#define GPE0_LIMIT			28
#define TOTAL_BITS(a)			(8 * sizeof(a))

/* SATA Controller D11F0 */
#define SATA_MISC_CONTROL_REG		0x40
#define SATA_MISC_SUBCLASS_WREN		BIT(0)
/* Register in AHCIBaseAddress (BAR5 at D11F0x24) */
#define SATA_CAPABILITIES_REG		0xfc
#define SATA_CAPABILITY_SPM		BIT(12)

/* IO 0xcf9 - Reset control port*/
#define   FULL_RST			BIT(3)
#define   RST_CMD			BIT(2)
#define   SYS_RST			BIT(1)

/* IO 0xf0 NCP Error */
#define   NCP_WARM_BOOT			BIT(7) /* Write-once */

typedef struct aoac_devs {
	unsigned int :7;
	unsigned int ic2e:1; /* 7: I2C2 */
	unsigned int ic3e:1; /* 8: I2C3 */
	unsigned int ic4e:1; /* 9: I2C4 */
	unsigned int :1;
	unsigned int ut0e:1; /* 11: UART0 */
	unsigned int ut1e:1; /* 12: UART1 */
	unsigned int :3;
	unsigned int ut2e:1; /* 16: UART2 */
	unsigned int :9;
	unsigned int ut3e:1; /* 26: UART3 */
	unsigned int espi:1; /* 27: ESPI */
	unsigned int :4;
} __packed aoac_devs_t;

struct soc_power_reg {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t gpe0_sts;
	uint32_t gpe0_en;
	uint16_t wake_from;
};

void enable_aoac_devices(void);
bool is_aoac_device_enabled(int dev);
void power_on_aoac_device(int dev);
void power_off_aoac_device(int dev);
void wait_for_aoac_enabled(int dev);
void sb_clk_output_48Mhz(void);
void sb_enable(struct device *dev);
void southbridge_final(void *chip_info);
void southbridge_init(void *chip_info);
void fch_pre_init(void);
void fch_early_init(void);
void set_uart_config(int idx);
void clear_uart_legacy_config(void);

/* Initialize all the i2c buses that are marked with early init. */
void i2c_soc_early_init(void);

/* Initialize all the i2c buses that are not marked with early init. */
void i2c_soc_init(void);

/* Allow the board to change the default I2C pad configuration */
void mainboard_i2c_override(int bus, uint32_t *pad_settings);

#endif /* __PICASSO_SB_H__ */
