/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010-2017 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __STONEYRIDGE_H__
#define __STONEYRIDGE_H__

#include <arch/io.h>
#include <types.h>
#include <device/pci_ids.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include "chip.h"
#include <rules.h>

/* PSP at D8F0 */
#define PSP_MAILBOX_BAR			PCI_BASE_ADDRESS_4 /* BKDG: "BAR3" */
#define PSP_BAR_ENABLES			0x48
#define  PSP_MAILBOX_BAR_EN		0x10

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PM_ISA_CONTROL			0x04
#define   MMIO_EN			BIT(1)
#define PM_PCI_CTRL			0x08
#define   FORCE_SLPSTATE_RETRY		BIT(25)
#define   FORCE_STPCLK_RETRY		BIT(24)
#define PM_ACPI_MMIO_EN			0x24
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
#define PM_PMIO_DEBUG			0xd2
#define PM_MANUAL_RESET			0xd3
#define PM_HUD_SD_FLASH_CTRL		0xe7
#define PM_YANG_SD_FLASH_CTRL		0xe8
#define PM_PCIB_CFG			0xea
#define   PM_GENINT_DISABLE		BIT(0)
#define PM_LPC_GATING			0xec
#define   PM_LPC_AB_NO_BYPASS_EN	BIT(2)
#define   PM_LPC_A20_EN			BIT(1)
#define   PM_LPC_ENABLE			BIT(0)

#define SPIROM_BASE_ADDRESS_REGISTER	0xa0
#define   ROUTE_TPM_2_SPI		BIT(3)
#define   SPI_ABORT_ENABLE		BIT(2)
#define   SPI_ROM_ENABLE		BIT(1)
#define   SPI_ROM_ALT_ENABLE		BIT(0)
#define   SPI_PRESERVE_BITS		(BIT(0) | BIT(1) | BIT(2) | BIT(3))

#define LPC_PCI_CONTROL			0x40
#define   IMC_PRESENT			BIT(7)
#define   IMC_TO_HOST_SEMAPHORE		BIT(6)
#define   HOST_TO_IMC_SEMAPHORE		BIT(5)
#define   LEGACY_DMA_EN			BIT(2)

#define LPC_IO_PORT_DECODE_ENABLE	0x44
#define   DECODE_ENABLE_PARALLEL_PORT0	BIT(0)
#define   DECODE_ENABLE_PARALLEL_PORT1	BIT(1)
#define   DECODE_ENABLE_PARALLEL_PORT2	BIT(2)
#define   DECODE_ENABLE_PARALLEL_PORT3	BIT(3)
#define   DECODE_ENABLE_PARALLEL_PORT4	BIT(4)
#define   DECODE_ENABLE_PARALLEL_PORT5	BIT(5)
#define   DECODE_ENABLE_SERIAL_PORT0	BIT(6)
#define   DECODE_ENABLE_SERIAL_PORT1	BIT(7)
#define   DECODE_ENABLE_SERIAL_PORT2	BIT(8)
#define   DECODE_ENABLE_SERIAL_PORT3	BIT(9)
#define   DECODE_ENABLE_SERIAL_PORT4	BIT(10)
#define   DECODE_ENABLE_SERIAL_PORT5	BIT(11)
#define   DECODE_ENABLE_SERIAL_PORT6	BIT(12)
#define   DECODE_ENABLE_SERIAL_PORT7	BIT(13)
#define   DECODE_ENABLE_AUDIO_PORT0	BIT(14)
#define   DECODE_ENABLE_AUDIO_PORT1	BIT(15)
#define   DECODE_ENABLE_AUDIO_PORT2	BIT(16)
#define   DECODE_ENABLE_AUDIO_PORT3	BIT(17)
#define   DECODE_ENABLE_MIDI_PORT0	BIT(18)
#define   DECODE_ENABLE_MIDI_PORT1	BIT(19)
#define   DECODE_ENABLE_MIDI_PORT2	BIT(20)
#define   DECODE_ENABLE_MIDI_PORT3	BIT(21)
#define   DECODE_ENABLE_MSS_PORT0	BIT(22)
#define   DECODE_ENABLE_MSS_PORT1	BIT(23)
#define   DECODE_ENABLE_MSS_PORT2	BIT(24)
#define   DECODE_ENABLE_MSS_PORT3	BIT(25)
#define   DECODE_ENABLE_FDC_PORT0	BIT(26)
#define   DECODE_ENABLE_FDC_PORT1	BIT(27)
#define   DECODE_ENABLE_GAME_PORT	BIT(28)
#define   DECODE_ENABLE_KBC_PORT	BIT(29)
#define   DECODE_ENABLE_ACPIUC_PORT	BIT(30)
#define   DECODE_ENABLE_ADLIB_PORT	BIT(31)

#define LPC_IO_OR_MEM_DECODE_ENABLE	0x48
#define   LPC_WIDEIO2_ENABLE		BIT(25)
#define   LPC_WIDEIO1_ENABLE		BIT(24)
#define   DECODE_IO_PORT_ENABLE6	BIT(23)
#define   DECODE_IO_PORT_ENABLE5	BIT(22)
#define   DECODE_IO_PORT_ENABLE4	BIT(21)
#define   DECODE_MEM_PORT_ENABLE1	BIT(20)
#define   DECODE_IO_PORT_ENABLE3	BIT(19)
#define   DECODE_IO_PORT_ENABLE2	BIT(18)
#define   DECODE_IO_PORT_ENABLE1	BIT(17)
#define   DECODE_IO_PORT_ENABLE0	BIT(16)
#define   LPC_SYNC_TIMEOUT_COUNT_ENABLE	BIT(7)
#define   LPC_DECODE_RTC_IO_ENABLE	BIT(6)
#define   DECODE_MEM_PORT_ENABLE0	BIT(5)
#define   LPC_WIDEIO0_ENABLE		BIT(2)
#define   DECODE_ALTERNATE_SIO_ENABLE	BIT(1)
#define   DECODE_SIO_ENABLE		BIT(0)
/* Assuming word access to higher word (register 0x4a) */
#define LPC_IO_OR_MEM_DEC_EN_HIGH	0x4a
#define   LPC_WIDEIO2_ENABLE_H		BIT(9)
#define   LPC_WIDEIO1_ENABLE_H		BIT(8)
#define   DECODE_IO_PORT_ENABLE6_H	BIT(7)
#define   DECODE_IO_PORT_ENABLE5_H	BIT(6)
#define   DECODE_IO_PORT_ENABLE4_H	BIT(5)
#define   DECODE_IO_PORT_ENABLE3_H	BIT(3)
#define   DECODE_IO_PORT_ENABLE2_H	BIT(2)
#define   DECODE_IO_PORT_ENABLE1_H	BIT(1)
#define   DECODE_IO_PORT_ENABLE0_H	BIT(0)

#define LPC_MEM_PORT1			0x4c
#define LPC_MEM_PORT0			0x60

/*
 * Register 0x64 is 32-bit, composed by two 16-bit sub-registers.
 * For ease of access, each sub-register is declared separetely.
 */
#define LPC_WIDEIO_GENERIC_PORT		0x64
#define LPC_WIDEIO1_GENERIC_PORT	0x66
#define ROM_ADDRESS_RANGE1_START	0x68
#define ROM_ADDRESS_RANGE1_END		0x6a
#define ROM_ADDRESS_RANGE2_START	0x6c
#define ROM_ADDRESS_RANGE2_END		0x6e

#define LPC_ALT_WIDEIO_RANGE_ENABLE	0x74
#define   LPC_ALT_WIDEIO2_ENABLE	BIT(3)
#define   LPC_ALT_WIDEIO1_ENABLE	BIT(2)
#define   LPC_ALT_WIDEIO0_ENABLE	BIT(0)

#define LPC_MISC_CONTROL_BITS		0x78
#define   LPC_NOHOG			BIT(0)

#define LPC_TRUSTED_PLATFORM_MODULE	0x7c
#define   TPM_12_EN			BIT(0)
#define   TPM_LEGACY_EN			BIT(2)

#define LPC_WIDEIO2_GENERIC_PORT	0x90

/*
 * LPC register 0xb8 is DWORD, here there are definitions for byte
 * access. For example, bits 31-24 are accessed through byte access
 * at register 0xbb ().
 */
#define LPC_ROM_DMA_EC_HOST_CONTROL	0xb8
#define   SPI_FROM_HOST_PREFETCH_EN	BIT(24)
#define   SPI_FROM_USB_PREFETCH_EN	BIT(23)

#define LPC_HOST_CONTROL		0xbb
#define   IMC_PAGE_FROM_HOST_EN		BIT(0)
#define   IMC_PORT_FROM_HOST_EN		BIT(3)

/* SPI Controller */
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
#define   SPI_FIFO_PTR_CLR		BIT(20)
#define   SPI_ARB_ENABLE		BIT(19)
#define   EXEC_OPCODE			BIT(16)
#define SPI_CNTRL1			0x0c
#define SPI_CMD_CODE			0x45
#define SPI_CMD_TRIGGER		0x47
#define   SPI_CMD_TRIGGER_EXECUTE	(BIT(7))
#define SPI_TX_BYTE_COUNT		0x48
#define SPI_RX_BYTE_COUNT		0x4B
#define SPI_STATUS			0x4c
#define   SPI_DONE_BYTE_COUNT_SHIFT	0
#define   SPI_DONE_BYTE_COUNT_MASK	0xff
#define   SPI_FIFO_WR_PTR_SHIFT		8
#define   SPI_FIFO_WR_PTR_MASK		0x7f
#define   SPI_FIFO_RD_PTR_SHIFT		16
#define   SPI_FIFO_RD_PTR_MASK		0x7f
#define SPI_FIFO			0x80
#define   SPI_FIFO_DEPTH		(0xc7 - SPI_FIFO)

#define SPI100_SPEED_CONFIG		0x22
/* Use SPI_SPEED_16M-SPI_SPEED_66M below for the southbridge */
#define   SPI_CNTRL1_SPEED_MASK		(BIT(15) | BIT(14) | BIT(13) | BIT(12))
#define   SPI_NORM_SPEED_SH		12
#define   SPI_FAST_SPEED_SH		8

#define SPI100_ENABLE			0x20
#define   SPI_USE_SPI100		BIT(0)

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

#define MISC_MISC_CLK_CNTL_1		0x40
#define   OSCOUT1_CLK_OUTPUT_ENB	BIT(2)  /* 0 = Enabled, 1 = Disabled */

/* IO 0xcf9 - Reset control port*/
#define   FULL_RST			BIT(3)
#define   RST_CMD			BIT(2)
#define   SYS_RST			BIT(1)

/* PMx10 - Power Reset Config */
#define PWR_RESET_CFG			0x10
#define   TOGGLE_ALL_PWR_GOOD		BIT(1)

#define XHCI_PM_INDIRECT_INDEX		0x48
#define XHCI_PM_INDIRECT_DATA		0x4C
#define   XHCI_OVER_CURRENT_CONTROL	0x30
#define EHCI_OVER_CURRENT_CONTROL	0x70

#define USB_OC0				0
#define USB_OC1				1
#define USB_OC2				2
#define USB_OC3				3
#define USB_OC4				4
#define USB_OC5				5
#define USB_OC6				6
#define USB_OC7				7
#define USB_OC_DISABLE			0xf
#define USB_OC_DISABLE_ALL		0xffff

#define OC_PORT0_SHIFT		0
#define OC_PORT1_SHIFT		4
#define OC_PORT2_SHIFT		8
#define OC_PORT3_SHIFT		12

#define WIDEIO_RANGE_ERROR		-1
#define TOTAL_WIDEIO_PORTS		3

#if ENV_BOOTBLOCK
#define GPIO_TABLE_BOOTBLOCK		1
#else
#define GPIO_TABLE_BOOTBLOCK		0
#endif

/* FCH AOAC Registers 0xfed81e00 */
#define FCH_AOAC_D3_CONTROL_CLK_GEN	0x40
#define FCH_AOAC_D3_CONTROL_I2C0	0x4A
#define FCH_AOAC_D3_CONTROL_I2C1	0x4C
#define FCH_AOAC_D3_CONTROL_I2C2	0x4E
#define FCH_AOAC_D3_CONTROL_I2C3	0x50
#define FCH_AOAC_D3_CONTROL_UART0	0x56
#define FCH_AOAC_D3_CONTROL_UART1	0x58
#define FCH_AOAC_D3_CONTROL_AMBA	0x62
#define FCH_AOAC_D3_CONTROL_USB2	0x64
#define FCH_AOAC_D3_CONTROL_USB3	0x6E
/* Bit definitions for all FCH_AOAC_D3_CONTROL_* Registers */
#define   FCH_AOAC_TARGET_DEVICE_STATE (BIT(0) + BIT(1))
#define   FCH_AOAC_DEVICE_STATE		BIT(2)
#define   FCH_AOAC_PWR_ON_DEV		BIT(3)
#define   FCH_AOAC_SW_PWR_ON_RSTB	BIT(4)
#define   FCH_AOAC_SW_REF_CLK_OK	BIT(5)
#define   FCH_AOAC_SW_RST_B		BIT(6)
#define   FCH_AOAC_IS_SW_CONTROL	BIT(7)

#define FCH_AOAC_D3_STATE_CLK_GEN	0x41
#define FCH_AOAC_D3_STATE_I2C0		0x4B
#define FCH_AOAC_D3_STATE_I2C1		0x4D
#define FCH_AOAC_D3_STATE_I2C2		0x4F
#define FCH_AOAC_D3_STATE_I2C3		0x51
#define FCH_AOAC_D3_STATE_UART0		0x57
#define FCH_AOAC_D3_STATE_UART1		0x59
#define FCH_AOAC_D3_STATE_AMBA		0x63
#define FCH_AOAC_D3_STATE_USB2		0x65
#define FCH_AOAC_D3_STATE_USB3		0x6F
/* Bit definitions for all FCH_AOAC_D3_STATE_* Registers */
#define   FCH_AOAC_PWR_RST_STATE	BIT(0)
#define   FCH_AOAC_RST_CLK_OK_STATE	BIT(1)
#define   FCH_AOAC_RST_B_STATE		BIT(2)
#define   FCH_AOAC_DEV_OFF_GATING_STATE	BIT(3)
#define   FCH_AOAC_D3COLD		BIT(4)
#define   FCH_AOAC_CLK_OK_STATE		BIT(5)
#define   FCH_AOAC_STAT0		BIT(6)
#define   FCH_AOAC_STAT1		BIT(7)

struct stoneyridge_aoac {
	int enable;
	int status;
};

void enable_aoac_devices(void);
void sb_enable_rom(void);
void configure_stoneyridge_i2c(void);
void sb_clk_output_48Mhz(void);
void sb_disable_4dw_burst(void);
void sb_enable(device_t dev);
void southbridge_final(void *chip_info);
void southbridge_init(void *chip_info);
void sb_lpc_port80(void);
void sb_lpc_decode(void);
void sb_acpi_mmio_decode(void);
void sb_pci_port80(void);
void sb_read_mode(u32 mode);
void sb_set_spi100(u16 norm, u16 fast, u16 alt, u16 tpm);
void sb_tpm_decode(void);
void sb_tpm_decode_spi(void);
void lpc_wideio_512_window(uint16_t base);
void lpc_wideio_16_window(uint16_t base);
u8 pm_read8(u8 reg);
u16 pm_read16(u8 reg);
u32 pm_read32(u8 reg);
void pm_write8(u8 reg, u8 value);
void pm_write16(u8 reg, u16 value);
void pm_write32(u8 reg, u32 value);
uint8_t smi_read8(uint8_t offset);
uint16_t smi_read16(uint8_t offset);
uint32_t smi_read32(uint8_t offset);
void smi_write8(uint8_t offset, uint8_t value);
void smi_write16(uint8_t offset, uint16_t value);
void smi_write32(uint8_t offset, uint32_t value);
uint8_t biosram_read8(uint8_t offset);
void biosram_write8(uint8_t offset, uint8_t value);
uint16_t biosram_read16(uint8_t offset);
void biosram_write16(uint8_t offset, uint16_t value);
uint32_t biosram_read32(uint8_t offset);
void biosram_write32(uint8_t offset, uint32_t value);
uint16_t pm_acpi_pm_cnt_blk(void);
uint16_t pm_acpi_pm_evt_blk(void);
void xhci_pm_write8(uint8_t reg, uint8_t value);
uint8_t xhci_pm_read8(uint8_t reg);
void xhci_pm_write16(uint8_t reg, uint16_t value);
uint16_t xhci_pm_read16(uint8_t reg);
void xhci_pm_write32(uint8_t reg, uint32_t value);
uint32_t xhci_pm_read32(uint8_t reg);
void bootblock_fch_early_init(void);
/**
 * @brief Save the UMA bize returned by AGESA
 *
 * @param size = in bytes
 *
 * @return none
 */
void save_uma_size(uint32_t size);
/**
 * @brief Save the UMA base address returned by AGESA
 *
 * @param base = 64bit base address
 *
 * @return none
 */
void save_uma_base(uint64_t base);
/**
 * @brief Get the saved UMA size
 *
 * @param none
 *
 * @return size in bytes
 */
uint32_t get_uma_size(void);
/**
 * @brief Get the saved UMA base
 *
 * @param none
 *
 * @return 64bit base address
 */
uint64_t get_uma_base(void);
/**
 * @brief Find the size of a particular wide IO
 *
 * @param index = index of desired wide IO
 *
 * @return size of desired wide IO
 */
uint16_t sb_wideio_size(int index);
/**
 * @brief Identify if any LPC wide IO is covering the IO range
 *
 * @param start = start of IO range
 * @param size = size of IO range
 *
 * @return Index of wide IO covering the range or error
 */
int sb_find_wideio_range(uint16_t start, uint16_t size);
/**
 * @brief Program a LPC wide IO to support an IO range
 *
 * @param start = start of range to be routed through wide IO
 * @param size = size of range to be routed through wide IO
 *
 * @return Index of wide IO register used or error
 */
int sb_set_wideio_range(uint16_t start, uint16_t size);

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

#endif /* __STONEYRIDGE_H__ */
