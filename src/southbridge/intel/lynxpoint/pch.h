/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H
#define SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H

/*
 * Lynx Point PCH PCI Devices:
 *
 * Bus 0:Device 31:Function 0 LPC Controller1
 * Bus 0:Device 31:Function 2 SATA Controller #1
 * Bus 0:Device 31:Function 3 SMBus Controller
 * Bus 0:Device 31:Function 5 SATA Controller #22
 * Bus 0:Device 31:Function 6 Thermal Subsystem
 * Bus 0:Device 29:Function 03 USB EHCI Controller #1
 * Bus 0:Device 26:Function 03 USB EHCI Controller #2
 * Bus 0:Device 28:Function 0 PCI Express* Port 1
 * Bus 0:Device 28:Function 1 PCI Express Port 2
 * Bus 0:Device 28:Function 2 PCI Express Port 3
 * Bus 0:Device 28:Function 3 PCI Express Port 4
 * Bus 0:Device 28:Function 4 PCI Express Port 5
 * Bus 0:Device 28:Function 5 PCI Express Port 6
 * Bus 0:Device 28:Function 6 PCI Express Port 7
 * Bus 0:Device 28:Function 7 PCI Express Port 8
 * Bus 0:Device 27:Function 0 Intel High Definition Audio Controller
 * Bus 0:Device 25:Function 0 Gigabit Ethernet Controller
 * Bus 0:Device 22:Function 0 Intel Management Engine Interface #1
 * Bus 0:Device 22:Function 1 Intel Management Engine Interface #2
 * Bus 0:Device 22:Function 2 IDE-R
 * Bus 0:Device 22:Function 3 KT
 * Bus 0:Device 20:Function 0 xHCI Controller
*/

/* PCH types */
#define PCH_TYPE_LPT		0x8c
#define PCH_TYPE_LPT_LP		0x9c

/* PCH stepping values for LPC device */
#define LPT_H_STEP_B0		0x02
#define LPT_H_STEP_C0		0x03
#define LPT_H_STEP_C1		0x04
#define LPT_H_STEP_C2		0x05
#define LPT_LP_STEP_B0		0x02
#define LPT_LP_STEP_B1		0x03
#define LPT_LP_STEP_B2		0x04

/*
 * It does not matter where we put the SMBus I/O base, as long as we
 * keep it consistent and don't interfere with other devices.  Stage2
 * will relocate this anyways.
 * Our solution is to have SMB initialization move the I/O to SMBUS_IO_BASE
 * again. But handling static BARs is a generic problem that should be
 * solved in the device allocator.
 */
#define SMBUS_IO_BASE		0x0400
#define SMBUS_SLAVE_ADDR	0x24

#if CONFIG_INTEL_LYNXPOINT_LP
#define DEFAULT_PMBASE		0x1000
#define DEFAULT_GPIOBASE	0x1400
#define DEFAULT_GPIOSIZE	0x400
#else
#define DEFAULT_PMBASE		0x500
#define DEFAULT_GPIOBASE	0x480
#define DEFAULT_GPIOSIZE	0x80
#endif

#define HPET_ADDR		0xfed00000
#define DEFAULT_RCBA		0xfed1c000

#ifndef __ACPI__

#if defined (__SMM__) && !defined(__ASSEMBLER__)
void intel_pch_finalize_smm(void);
#endif


/* State Machine configuration. */
#define RCBA_REG_SIZE_MASK 0x8000
#define   RCBA_REG_SIZE_16   0x8000
#define   RCBA_REG_SIZE_32   0x0000
#define RCBA_COMMAND_MASK  0x000f
#define   RCBA_COMMAND_SET   0x0001
#define   RCBA_COMMAND_READ  0x0002
#define   RCBA_COMMAND_RMW   0x0003
#define   RCBA_COMMAND_END   0x0007

#define RCBA_ENCODE_COMMAND(command_, reg_, mask_, or_value_) \
	{ .command = command_,   \
	  .reg = reg_,           \
	  .mask = mask_,         \
	  .or_value = or_value_  \
	}
#define RCBA_SET_REG_32(reg_, value_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_32|RCBA_COMMAND_SET, reg_, 0, value_)
#define RCBA_READ_REG_32(reg_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_32|RCBA_COMMAND_READ, reg_, 0, 0)
#define RCBA_RMW_REG_32(reg_, mask_, or_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_32|RCBA_COMMAND_RMW, reg_, mask_, or_)
#define RCBA_SET_REG_16(reg_, value_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_16|RCBA_COMMAND_SET, reg_, 0, value_)
#define RCBA_READ_REG_16(reg_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_16|RCBA_COMMAND_READ, reg_, 0, 0)
#define RCBA_RMW_REG_16(reg_, mask_, or_) \
	RCBA_ENCODE_COMMAND(RCBA_REG_SIZE_16|RCBA_COMMAND_RMW, reg_, mask_, or_)
#define RCBA_END_CONFIG \
	RCBA_ENCODE_COMMAND(RCBA_COMMAND_END, 0, 0, 0)

struct rcba_config_instruction
{
	u16 command;
	u16 reg;
	u32 mask;
	u32 or_value;
};

#if !defined(__ASSEMBLER__)
void pch_config_rcba(const struct rcba_config_instruction *rcba_config);
int pch_silicon_revision(void);
int pch_silicon_type(void);
int pch_is_lp(void);
u16 get_pmbase(void);
u16 get_gpiobase(void);

/* Power Management register handling in pmutil.c */
/* PM1_CNT */
void enable_pm1_control(u32 mask);
void disable_pm1_control(u32 mask);
/* PM1 */
u16 clear_pm1_status(void);
void enable_pm1(u16 events);
u32 clear_smi_status(void);
/* SMI */
void enable_smi(u32 mask);
void disable_smi(u32 mask);
/* ALT_GP_SMI */
u32 clear_alt_smi_status(void);
void enable_alt_smi(u32 mask);
/* TCO */
u32 clear_tco_status(void);
void enable_tco_sci(void);
/* GPE0 */
u32 clear_gpe_status(void);
void clear_gpe_enable(void);
void enable_all_gpe(u32 set1, u32 set2, u32 set3, u32 set4);
void disable_all_gpe(void);
void enable_gpe(u32 mask);
void disable_gpe(u32 mask);
/*
 * get GPIO pin value
 */
int get_gpio(int gpio_num);
/*
 * Get a number comprised of multiple GPIO values. gpio_num_array points to
 * the array of gpio pin numbers to scan, terminated by -1.
 */
unsigned get_gpios(const int *gpio_num_array);
/*
 * Set GPIO pin value.
 */
void set_gpio(int gpio_num, int value);
/* Return non-zero if gpio is set to native function. 0 otherwise. */
int gpio_is_native(int gpio_num);

#if !defined(__PRE_RAM__) && !defined(__SMM__)
#include <device/device.h>
#include <arch/acpi.h>
#include "chip.h"
void pch_enable(device_t dev);
void pch_disable_devfn(device_t dev);
u32 pch_iobp_read(u32 address);
void pch_iobp_write(u32 address, u32 data);
void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue);
#if CONFIG_ELOG
void pch_log_state(void);
#endif
void acpi_create_intel_hpet(acpi_hpet_t * hpet);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);

/* These helpers are for performing SMM relocation. */
void southbridge_trigger_smi(void);
void southbridge_clear_smi_status(void);
/* The initialization of the southbridge is split into 2 compoments. One is
 * for clearing the state in the SMM registers. The other is for enabling
 * SMIs. They are split so that other work between the 2 actions. */
void southbridge_smm_clear_state(void);
void southbridge_smm_enable_smi(void);
#else
void enable_smbus(void);
void enable_usb_bar(void);
int smbus_read_byte(unsigned device, unsigned address);
int early_spi_read(u32 offset, u32 size, u8 *buffer);
int early_pch_init(const void *gpio_map,
                   const struct rcba_config_instruction *rcba_config);
#endif /* !__PRE_RAM__ && !__SMM__ */
#endif /* __ASSEMBLER__ */

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

/* PCI Configuration Space (D30:F0): PCI2PCI */
#define PSTS	0x06
#define SMLT	0x1b
#define SECSTS	0x1e
#define INTR	0x3c
#define BCTRL	0x3e
#define   SBR	(1 << 6)
#define   SEE	(1 << 1)
#define   PERE	(1 << 0)

/* Power Management Control and Status */
#define PCH_PCS			0x84
#define  PCH_PCS_PS_D3HOT	3

#define PCH_EHCI1_DEV		PCI_DEV(0, 0x1d, 0)
#define PCH_EHCI2_DEV		PCI_DEV(0, 0x1a, 0)
#define PCH_XHCI_DEV		PCI_DEV(0, 0x14, 0)
#define PCH_ME_DEV		PCI_DEV(0, 0x16, 0)
#define PCH_PCIE_DEV_SLOT	28

/* PCI Configuration Space (D31:F0): LPC */
#define PCH_LPC_DEV		PCI_DEV(0, 0x1f, 0)
#define SERIRQ_CNTL		0x64

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4
#define PMIR			0xac
#define  PMIR_CF9LOCK		(1 << 31)
#define  PMIR_CF9GR		(1 << 20)

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#define PMBASE			0x40
#define ACPI_CNTL		0x44
#define   ACPI_EN		(1 << 7)
#define BIOS_CNTL		0xDC
#define GPIO_BASE		0x48 /* LPC GPIO Base Address Register */
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */
#define GPIO_ROUT		0xb8

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6A
#define PIRQH_ROUT		0x6B

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define  CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define  CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define  MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define  KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define  GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define  GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define  FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define  LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define  COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define  COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[2:0] */
#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */
#define LGMR			0x98 /* LPC Generic Memory Range */

/* PCI Configuration Space (D31:F1): IDE */
#define PCH_IDE_DEV		PCI_DEV(0, 0x1f, 1)
#define PCH_SATA_DEV		PCI_DEV(0, 0x1f, 2)
#define PCH_SATA2_DEV		PCI_DEV(0, 0x1f, 5)
#define INTR_LN			0x3c
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define   IDE_DECODE_ENABLE	(1 << 15)
#define   IDE_SITRE		(1 << 14)
#define   IDE_ISP_5_CLOCKS	(0 << 12)
#define   IDE_ISP_4_CLOCKS	(1 << 12)
#define   IDE_ISP_3_CLOCKS	(2 << 12)
#define   IDE_RCT_4_CLOCKS	(0 <<  8)
#define   IDE_RCT_3_CLOCKS	(1 <<  8)
#define   IDE_RCT_2_CLOCKS	(2 <<  8)
#define   IDE_RCT_1_CLOCKS	(3 <<  8)
#define   IDE_DTE1		(1 <<  7)
#define   IDE_PPE1		(1 <<  6)
#define   IDE_IE1		(1 <<  5)
#define   IDE_TIME1		(1 <<  4)
#define   IDE_DTE0		(1 <<  3)
#define   IDE_PPE0		(1 <<  2)
#define   IDE_IE0		(1 <<  1)
#define   IDE_TIME0		(1 <<  0)
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

#define IDE_SDMA_CNT		0x48	/* Synchronous DMA control */
#define   IDE_SSDE1		(1 <<  3)
#define   IDE_SSDE0		(1 <<  2)
#define   IDE_PSDE1		(1 <<  1)
#define   IDE_PSDE0		(1 <<  0)

#define IDE_SDMA_TIM		0x4a

#define IDE_CONFIG		0x54	/* IDE I/O Configuration Register */
#define   SIG_MODE_SEC_NORMAL	(0 << 18)
#define   SIG_MODE_SEC_TRISTATE	(1 << 18)
#define   SIG_MODE_SEC_DRIVELOW	(2 << 18)
#define   SIG_MODE_PRI_NORMAL	(0 << 16)
#define   SIG_MODE_PRI_TRISTATE	(1 << 16)
#define   SIG_MODE_PRI_DRIVELOW	(2 << 16)
#define   FAST_SCB1		(1 << 15)
#define   FAST_SCB0		(1 << 14)
#define   FAST_PCB1		(1 << 13)
#define   FAST_PCB0		(1 << 12)
#define   SCB1			(1 <<  3)
#define   SCB0			(1 <<  2)
#define   PCB1			(1 <<  1)
#define   PCB0			(1 <<  0)

#define SATA_SIRI		0xa0 /* SATA Indexed Register Index */
#define SATA_SIRD		0xa4 /* SATA Indexed Register Data */
#define SATA_SP			0xd0 /* Scratchpad */

/* SATA IOBP Registers */
#define SATA_IOBP_SP0G3IR	0xea000151
#define SATA_IOBP_SP1G3IR	0xea000051
#define SATA_IOBP_SP0DTLE_DATA	0xea002550
#define SATA_IOBP_SP0DTLE_EDGE	0xea002554
#define SATA_IOBP_SP1DTLE_DATA	0xea002750
#define SATA_IOBP_SP1DTLE_EDGE	0xea002754

#define SATA_DTLE_MASK		0xF
#define SATA_DTLE_DATA_SHIFT	24
#define SATA_DTLE_EDGE_SHIFT	16

/* USB Registers */
#define EHCI_PWR_CNTL_STS	0x54
#define  EHCI_PWR_STS_MASK	0x3
#define  EHCI_PWR_STS_SET_D0	0x0
#define  EHCI_PWR_STS_SET_D3	0x3

/* Serial IO IOBP Registers */
#define SIO_IOBP_PORTCTRL0	0xcb000000	/* SDIO D23:F0 */
#define  SIO_IOBP_PORTCTRL0_ACPI_IRQ_EN		(1 << 5)
#define  SIO_IOBP_PORTCTRL0_PCI_CONF_DIS	(1 << 4)
#define SIO_IOBP_PORTCTRL1	0xcb000014	/* SDIO D23:F0 */
#define  SIO_IOBP_PORTCTRL1_SNOOP_SELECT(x)	(((x) & 3) << 13)
#define SIO_IOBP_GPIODF		0xcb000154
#define  SIO_IOBP_GPIODF_SDIO_IDLE_DET_EN	(1 << 4)
#define  SIO_IOBP_GPIODF_DMA_IDLE_DET_EN	(1 << 3)
#define  SIO_IOBP_GPIODF_UART_IDLE_DET_EN	(1 << 2)
#define  SIO_IOBP_GPIODF_I2C_IDLE_DET_EN	(1 << 1)
#define  SIO_IOBP_GPIODF_SPI_IDLE_DET_EN	(1 << 0)
#define SIO_IOBP_PORTCTRL2	0xcb000240	/* DMA D21:F0 */
#define SIO_IOBP_PORTCTRL3	0xcb000248	/* I2C0 D21:F1 */
#define SIO_IOBP_PORTCTRL4	0xcb000250	/* I2C1 D21:F2 */
#define SIO_IOBP_PORTCTRL5	0xcb000258	/* SPI0 D21:F3 */
#define SIO_IOBP_PORTCTRL6	0xcb000260	/* SPI1 D21:F4 */
#define SIO_IOBP_PORTCTRL7	0xcb000268	/* UART0 D21:F5 */
#define SIO_IOBP_PORTCTRL8	0xcb000270	/* UART1 D21:F6 */
#define SIO_IOBP_PORTCTRLX(x)	(0xcb000240 + ((x) * 8))
/* PORTCTRL 2-8 have the same layout */
#define  SIO_IOBP_PORTCTRL_ACPI_IRQ_EN		(1 << 21)
#define  SIO_IOBP_PORTCTRL_PCI_CONF_DIS		(1 << 20)
#define  SIO_IOBP_PORTCTRL_SNOOP_SELECT(x)	(((x) & 3) << 18)
#define  SIO_IOBP_PORTCTRL_INT_PIN(x)		(((x) & 0xf) << 2)
#define  SIO_IOBP_PORTCTRL_PM_CAP_PRSNT		(1 << 1)
#define SIO_IOBP_FUNCDIS0	0xce00aa07	/* DMA D21:F0 */
#define SIO_IOBP_FUNCDIS1	0xce00aa47	/* I2C0 D21:F1 */
#define SIO_IOBP_FUNCDIS2	0xce00aa87	/* I2C1 D21:F2 */
#define SIO_IOBP_FUNCDIS3	0xce00aac7	/* SPI0 D21:F3 */
#define SIO_IOBP_FUNCDIS4	0xce00ab07	/* SPI1 D21:F4 */
#define SIO_IOBP_FUNCDIS5	0xce00ab47	/* UART0 D21:F5 */
#define SIO_IOBP_FUNCDIS6	0xce00ab87	/* UART1 D21:F6 */
#define SIO_IOBP_FUNCDIS7	0xce00ae07	/* SDIO D23:F0 */
#define  SIO_IOBP_FUNCDIS_DIS			(1 << 8)

/* Serial IO Devices */
#define SIO_ID_SDMA		0 /* D21:F0 */
#define SIO_ID_I2C0		1 /* D21:F1 */
#define SIO_ID_I2C1		2 /* D21:F2 */
#define SIO_ID_SPI0		3 /* D21:F3 */
#define SIO_ID_SPI1		4 /* D21:F4 */
#define SIO_ID_UART0		5 /* D21:F5 */
#define SIO_ID_UART1		6 /* D21:F6 */
#define SIO_ID_SDIO		7 /* D23:F0 */

#define SIO_REG_PPR_CLOCK		0x800
#define  SIO_REG_PPR_CLOCK_EN		 (1 << 0)
#define SIO_REG_PPR_RST			0x804
#define  SIO_REG_PPR_RST_ASSERT		 0x3
#define SIO_REG_PPR_GEN			0x808
#define  SIO_REG_PPR_GEN_LTR_MODE_MASK	 (1 << 2)
#define  SIO_REG_PPR_GEN_VOLTAGE_MASK	 (1 << 3)
#define  SIO_REG_PPR_GEN_VOLTAGE(x)	 ((x & 1) << 3)
#define SIO_REG_AUTO_LTR		0x814

#define SIO_REG_SDIO_PPR_GEN		0x1008
#define SIO_REG_SDIO_PPR_SW_LTR		0x1010
#define SIO_REG_SDIO_PPR_CMD12		0x3c
#define  SIO_REG_SDIO_PPR_CMD12_B30	 (1 << 30)

#define SIO_PIN_INTA 1 /* IRQ5 in ACPI mode */
#define SIO_PIN_INTB 2 /* IRQ6 in ACPI mode */
#define SIO_PIN_INTC 3 /* IRQ7 in ACPI mode */
#define SIO_PIN_INTD 4 /* IRQ13 in ACPI mode */

/* PCI Configuration Space (D31:F3): SMBus */
#define PCH_SMBUS_DEV		PCI_DEV(0, 0x1f, 3)
#define SMB_BASE		0x20
#define HOSTC			0x40
#define SMB_RCV_SLVA		0x09

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* SMBus I/O bits. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf

#define SMBUS_TIMEOUT		(10 * 1000 * 100)


/* Southbridge IO BARs */

#define GPIOBASE		0x48

#define PMBASE		0x40

/* Root Complex Register Block */
#define RCBA		0xf0

#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBA + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBA + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBA + x))

#define RCBA_AND_OR(bits, x, and, or) \
        RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)

#define VCH		0x0000	/* 32bit */
#define VCAP1		0x0004	/* 32bit */
#define VCAP2		0x0008	/* 32bit */
#define PVC		0x000c	/* 16bit */
#define PVS		0x000e	/* 16bit */

#define V0CAP		0x0010	/* 32bit */
#define V0CTL		0x0014	/* 32bit */
#define V0STS		0x001a	/* 16bit */

#define V1CAP		0x001c	/* 32bit */
#define V1CTL		0x0020	/* 32bit */
#define V1STS		0x0026	/* 16bit */

#define RCTCL		0x0100	/* 32bit */
#define ESD		0x0104	/* 32bit */
#define ULD		0x0110	/* 32bit */
#define ULBA		0x0118	/* 64bit */

#define RP1D		0x0120	/* 32bit */
#define RP1BA		0x0128	/* 64bit */
#define RP2D		0x0130	/* 32bit */
#define RP2BA		0x0138	/* 64bit */
#define RP3D		0x0140	/* 32bit */
#define RP3BA		0x0148	/* 64bit */
#define RP4D		0x0150	/* 32bit */
#define RP4BA		0x0158	/* 64bit */
#define HDD		0x0160	/* 32bit */
#define HDBA		0x0168	/* 64bit */
#define RP5D		0x0170	/* 32bit */
#define RP5BA		0x0178	/* 64bit */
#define RP6D		0x0180	/* 32bit */
#define RP6BA		0x0188	/* 64bit */

#define RPC		0x0400	/* 32bit */
#define RPFN		0x0404	/* 32bit */

/* Root Port configuratinon space hide */
#define RPFN_HIDE(port)         (1 << (((port) * 4) + 3))
/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg,port)    (((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port,func)   (((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)       (7 << ((port) * 4))

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define TCTL		0x3000	/*  8bit */

#define NOINT		0
#define INTA		1
#define INTB		2
#define INTC		3
#define INTD		4

#define DIR_IDR		12	/* Interrupt D Pin Offset */
#define DIR_ICR		8	/* Interrupt C Pin Offset */
#define DIR_IBR		4	/* Interrupt B Pin Offset */
#define DIR_IAR		0	/* Interrupt A Pin Offset */

#define PIRQA		0
#define PIRQB		1
#define PIRQC		2
#define PIRQD		3
#define PIRQE		4
#define PIRQF		5
#define PIRQG		6
#define PIRQH		7

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_READY	0x0001
#define  IOBPS_TX_MASK	0x0006
#define  IOBPS_MASK     0xff00
#define  IOBPS_READ     0x0600
#define  IOBPS_WRITE	0x0700
#define IOBPU		0x233a
#define  IOBPU_MAGIC	0xf000

#define D31IP		0x3100	/* 32bit */
#define D31IP_TTIP	24	/* Thermal Throttle Pin */
#define D31IP_SIP2	20	/* SATA Pin 2 */
#define D31IP_SMIP	12	/* SMBUS Pin */
#define D31IP_SIP	8	/* SATA Pin */
#define D30IP		0x3104	/* 32bit */
#define D30IP_PIP	0	/* PCI Bridge Pin */
#define D29IP		0x3108	/* 32bit */
#define D29IP_E1P	0	/* EHCI #1 Pin */
#define D28IP		0x310c	/* 32bit */
#define D28IP_P8IP	28	/* PCI Express Port 8 */
#define D28IP_P7IP	24	/* PCI Express Port 7 */
#define D28IP_P6IP	20	/* PCI Express Port 6 */
#define D28IP_P5IP	16	/* PCI Express Port 5 */
#define D28IP_P4IP	12	/* PCI Express Port 4 */
#define D28IP_P3IP	8	/* PCI Express Port 3 */
#define D28IP_P2IP	4	/* PCI Express Port 2 */
#define D28IP_P1IP	0	/* PCI Express Port 1 */
#define D27IP		0x3110	/* 32bit */
#define D27IP_ZIP	0	/* HD Audio Pin */
#define D26IP		0x3114	/* 32bit */
#define D26IP_E2P	0	/* EHCI #2 Pin */
#define D25IP		0x3118	/* 32bit */
#define D25IP_LIP	0	/* GbE LAN Pin */
#define D22IP		0x3124	/* 32bit */
#define D22IP_KTIP	12	/* KT Pin */
#define D22IP_IDERIP	8	/* IDE-R Pin */
#define D22IP_MEI2IP	4	/* MEI #2 Pin */
#define D22IP_MEI1IP	0	/* MEI #1 Pin */
#define D20IP		0x3128	/* 32bit */
#define D20IP_XHCI	0	/* XHCI Pin */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D23IR		0x3158	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define D21IR		0x3164	/* 16bit */
#define D19IR		0x3168	/* 16bit */
#define ACPIIRQEN	0x31e0	/* 32bit */
#define OIC		0x31fe	/* 16bit */
#define PMSYNC_CONFIG	0x33c4	/* 32bit */
#define PMSYNC_CONFIG2	0x33cc	/* 32bit */
#define SOFT_RESET_CTRL 0x38f4
#define SOFT_RESET_DATA 0x38f8

#define DIR_ROUTE(a,b,c,d) \
  (((d) << DIR_IDR) | ((c) << DIR_ICR) | \
  ((b) << DIR_IBR) | ((a) << DIR_IAR))

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define PCH_DISABLE_GBE		(1 << 5)
#define FD		0x3418	/* 32bit */
#define DISPBDF		0x3424  /* 16bit */
#define FD2		0x3428	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable 1 RCBA 0x3418 */
#define PCH_DISABLE_ALWAYS	(1 << 0)
#define PCH_DISABLE_ADSPD	(1 << 1)
#define PCH_DISABLE_SATA1	(1 << 2)
#define PCH_DISABLE_SMBUS	(1 << 3)
#define PCH_DISABLE_HD_AUDIO	(1 << 4)
#define PCH_DISABLE_EHCI2	(1 << 13)
#define PCH_DISABLE_LPC		(1 << 14)
#define PCH_DISABLE_EHCI1	(1 << 15)
#define PCH_DISABLE_PCIE(x)	(1 << (16 + x))
#define PCH_DISABLE_THERMAL	(1 << 24)
#define PCH_DISABLE_SATA2	(1 << 25)
#define PCH_DISABLE_XHCI	(1 << 27)

/* Function Disable 2 RCBA 0x3428 */
#define PCH_DISABLE_KT		(1 << 4)
#define PCH_DISABLE_IDER	(1 << 3)
#define PCH_DISABLE_MEI2	(1 << 2)
#define PCH_DISABLE_MEI1	(1 << 1)
#define PCH_ENABLE_DBDF		(1 << 0)

/* ICH7 PMBASE */
#define PM1_STS		0x00
#define   WAK_STS	(1 << 15)
#define   PCIEXPWAK_STS	(1 << 14)
#define   PRBTNOR_STS	(1 << 11)
#define   RTC_STS	(1 << 10)
#define   PWRBTN_STS	(1 << 8)
#define   GBL_STS	(1 << 5)
#define   BM_STS	(1 << 4)
#define   TMROF_STS	(1 << 0)
#define PM1_EN		0x02
#define   PCIEXPWAK_DIS	(1 << 14)
#define   RTC_EN	(1 << 10)
#define   PWRBTN_EN	(1 << 8)
#define   GBL_EN	(1 << 5)
#define   TMROF_EN	(1 << 0)
#define PM1_CNT		0x04
#define   SLP_EN	(1 << 13)
#define   SLP_TYP	(7 << 10)
#define    SLP_TYP_S0	0
#define    SLP_TYP_S1	1
#define    SLP_TYP_S3	5
#define    SLP_TYP_S4	6
#define    SLP_TYP_S5	7
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x50 // mobile only
#define GPE0_STS	0x20
#define   PME_B0_STS	(1 << 13)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   PCI_EXP_STS	(1 << 9)
#define   RI_STS	(1 << 8)
#define   SMB_WAK_STS	(1 << 7)
#define   TCOSCI_STS	(1 << 6)
#define   SWGPE_STS	(1 << 2)
#define   HOT_PLUG_STS	(1 << 1)
#define GPE0_STS_2	0x24
#define GPE0_EN		0x28
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define   TCOSCI_EN	(1 << 6)
#define GPE0_EN_2	0x2c
#define SMI_EN		0x30
#define   INTEL_USB2_EN	 (1 << 18) // Intel-Specific USB2 SMI logic
#define   LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define   PERIODIC_EN	 (1 << 14) // SMI on PERIODIC_STS in SMI_STS
#define   TCO_EN	 (1 << 13) // Enable TCO Logic (BIOSWE et al)
#define   MCSMI_EN	 (1 << 11) // Trap microcontroller range access
#define   BIOS_RLS	 (1 <<  7) // asserts SCI on bit set
#define   SWSMI_TMR_EN	 (1 <<  6) // start software smi timer on bit set
#define   APMC_EN	 (1 <<  5) // Writes to APM_CNT cause SMI#
#define   SLP_SMI_EN	 (1 <<  4) // Write to SLP_EN in PM1_CNT asserts SMI#
#define   LEGACY_USB_EN  (1 <<  3) // Legacy USB circuit SMI logic
#define   BIOS_EN	 (1 <<  2) // Assert SMI# on setting GBL_RLS bit
#define   EOS		 (1 <<  1) // End of SMI (deassert SMI#)
#define   GBL_SMI_EN	 (1 <<  0) // SMI# generation at all?
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a
#define GPE_CNTL	0x42
#define DEVACT_STS	0x44
#define SS_CNT		0x50
#define C3_RES		0x54
#define TCO1_STS	0x64
#define   DMISCI_STS	(1 << 9)
#define TCO2_STS	0x66
#define ALT_GP_SMI_EN2	0x5c
#define ALT_GP_SMI_STS2	0x5e

/* Lynxpoint LP */
#define LP_GPE0_STS_1	0x80	/* GPIO 0-31 */
#define LP_GPE0_STS_2	0x84	/* GPIO 32-63 */
#define LP_GPE0_STS_3	0x88	/* GPIO 64-94 */
#define LP_GPE0_STS_4	0x8c	/* Standard GPE */
#define LP_GPE0_EN_1	0x90
#define LP_GPE0_EN_2	0x94
#define LP_GPE0_EN_3	0x98
#define LP_GPE0_EN_4	0x9c

/*
 * SPI Opcode Menu setup for SPIBAR lockdown
 * should support most common flash chips.
 */

#define SPIBAR_OFFSET 0x3800
#define SPIBAR8(x) RCBA8(x + SPIBAR_OFFSET)
#define SPIBAR16(x) RCBA16(x + SPIBAR_OFFSET)
#define SPIBAR32(x) RCBA32(x + SPIBAR_OFFSET)

/* Reigsters within the SPIBAR */
#define SSFC 0x91
#define FDOC 0xb0
#define FDOD 0xb4

#define SPI_OPMENU_0 0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0 0x01 /* Write, no address */

#define SPI_OPMENU_1 0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1 0x03 /* Write, address required */

#define SPI_OPMENU_2 0x03 /* READ: Read Data */
#define SPI_OPTYPE_2 0x02 /* Read, address required */

#define SPI_OPMENU_3 0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3 0x00 /* Read, no address */

#define SPI_OPMENU_4 0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4 0x03 /* Write, address required */

#define SPI_OPMENU_5 0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5 0x00 /* Read, no address */

#define SPI_OPMENU_6 0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6 0x03 /* Write, address required */

#define SPI_OPMENU_7 0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7 0x02 /* Read, address required */

#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 << 8) | SPI_OPMENU_4)
#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 << 8) | SPI_OPMENU_0)

#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 << 8) |  \
		    (SPI_OPTYPE_3 << 6) | (SPI_OPTYPE_2 << 4) |	  \
		    (SPI_OPTYPE_1 << 2) | (SPI_OPTYPE_0))

#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */

#define SPIBAR_HSFS                 0x3804   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_SCIP           (1 << 5) /* SPI Cycle In Progress */
#define  SPIBAR_HSFS_AEL            (1 << 2) /* SPI Access Error Log */
#define  SPIBAR_HSFS_FCERR          (1 << 1) /* SPI Flash Cycle Error */
#define  SPIBAR_HSFS_FDONE          (1 << 0) /* SPI Flash Cycle Done */
#define SPIBAR_HSFC                 0x3806   /* SPI hardware sequence control */
#define  SPIBAR_HSFC_BYTE_COUNT(c)  (((c - 1) & 0x3f) << 8)
#define  SPIBAR_HSFC_CYCLE_READ     (0 << 1) /* Read cycle */
#define  SPIBAR_HSFC_CYCLE_WRITE    (2 << 1) /* Write cycle */
#define  SPIBAR_HSFC_CYCLE_ERASE    (3 << 1) /* Erase cycle */
#define  SPIBAR_HSFC_GO             (1 << 0) /* GO: start SPI transaction */
#define SPIBAR_FADDR                0x3808   /* SPI flash address */
#define SPIBAR_FDATA(n)             (0x3810 + (4 * n)) /* SPI flash data */

#endif /* __ACPI__ */
#endif /* SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H */
