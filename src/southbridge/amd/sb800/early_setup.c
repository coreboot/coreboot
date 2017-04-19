/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef _SB800_EARLY_SETUP_C_
#define _SB800_EARLY_SETUP_C_

#include <reset.h>
#include <arch/acpi.h>
#include <arch/cpu.h>
#include <southbridge/amd/common/amd_defs.h>
#include <cbmem.h>
#include "sb800.h"
#include "smbus.c"

#define SMBUS_IO_BASE 0x6000	/* Is it a temporary SMBus I/O base address? */
	 /*SIZE 0x40 */

static void pmio_write(u8 reg, u8 value)
{
	outb(reg, PM_INDEX);
	outb(value, PM_INDEX + 1);
}

static u8 pmio_read(u8 reg)
{
	outb(reg, PM_INDEX);
	return inb(PM_INDEX + 1);
}

static void sb800_acpi_init(void)
{
	pmio_write(0x60, ACPI_PM_EVT_BLK & 0xFF);
	pmio_write(0x61, ACPI_PM_EVT_BLK >> 8);
	pmio_write(0x62, ACPI_PM1_CNT_BLK & 0xFF);
	pmio_write(0x63, ACPI_PM1_CNT_BLK >> 8);
	pmio_write(0x64, ACPI_PM_TMR_BLK & 0xFF);
	pmio_write(0x65, ACPI_PM_TMR_BLK >> 8);
	pmio_write(0x68, ACPI_GPE0_BLK & 0xFF);
	pmio_write(0x69, ACPI_GPE0_BLK >> 8);

	/* CpuControl is in \_PR.CP00, 6 bytes */
	pmio_write(0x66, ACPI_CPU_CONTROL & 0xFF);
	pmio_write(0x67, ACPI_CPU_CONTROL >> 8);

	pmio_write(0x6A, 0xB0);	/* AcpiSmiCmdLo */
	pmio_write(0x6B, 0);	/* AcpiSmiCmdHi */

	pmio_write(0x6E, 0xB8);	/* AcpiPmaCntBlkLo */
	pmio_write(0x6F, 0);	/* AcpiPmaCntBlkHi */

	pmio_write(0x6C, ACPI_PMA_CNT_BLK & 0xFF);
	pmio_write(0x6D, ACPI_PMA_CNT_BLK >> 8);

	pmio_write(0x74, 1<<0 | 1<<1 | 1<<4 | 1<<2); /* AcpiDecodeEnable, When set, SB uses
					* the contents of the PM registers at
					* index 60-6B to decode ACPI I/O address.
					* AcpiSmiEn & SmiCmdEn*/
	/* RTC_En_En, TMR_En_En, GBL_EN_EN */
	outl(0x1, ACPI_PM1_CNT_BLK);		  /* set SCI_EN */
}

/* RPR 2.28 Get SB ASIC Revision.*/
static u8 get_sb800_revision(void)
{
	pci_devfn_t dev;
	u8 rev_id;
	u8 rev = 0;

	/* if (rev != 0) return rev; */

	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);

	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\n");
		/* NOT REACHED */
	}
	rev_id =  pci_read_config8(dev, 0x08);

	if (rev_id == 0x40) {
		rev = REV_SB800_A11;
	} else if (rev_id == 0x41) {
		rev = REV_SB800_A12;
	} else {
		die("It is not SB800 or SB810\r\n");
	}

	return rev;
}

void sb800_clk_output_48Mhz(void)
{
	/* AcpiMMioDecodeEn */
	u8 reg8;
	reg8 = pmio_read(0x24);
	reg8 |= 1;
	reg8 &= ~(1 << 1);
	pmio_write(0x24, reg8);

	*(volatile u32 *)(AMD_SB_ACPI_MMIO_ADDR+0xE00+0x40) &= ~((1 << 0) | (1 << 2)); /* 48Mhz */
	*(volatile u32 *)(AMD_SB_ACPI_MMIO_ADDR+0xE00+0x40) |= 1 << 1; /* 48Mhz */
}
/***************************************
* Legacy devices are mapped to LPC space.
*	Serial port 0
*	KBC Port
*	ACPI Micro-controller port
*	LPC ROM size
*	This function does not change port 0x80 decoding.
*	Console output through any port besides 0x3f8 is unsupported.
*	If you use FWH ROMs, you have to setup IDSEL.
***************************************/
static void sb800_lpc_init(void)
{
	u8 reg8;
	pci_devfn_t dev;

	//dev = pci_locate_device(PCI_ID(0x1002, 0x4385), 0);	/* SMBUS controller */
	dev = PCI_DEV(0, 0x14, 0);
	/* NOTE: Set BootTimerDisable, otherwise it would keep rebooting!!
	 * This bit has no meaning if debug strap is not enabled. So if the
	 * board keeps rebooting and the code fails to reach here, we could
	 * disable the debug strap first. */
	reg8 = pmio_read(0x44+3);
	reg8 |= 1 << 7;
	pmio_write(0x44+3, reg8);

	/* Enable lpc controller */
	reg8 = pmio_read(0xEC);
	reg8 |= 1 << 0;
	pmio_write(0xEC, reg8);

	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439d), 0);	/* LPC Controller */
	/* Decode port 0x3f8-0x3ff (Serial 0) */
	//#warning Serial port decode on LPC is hardcoded to 0x3f8
	reg8 = pci_read_config8(dev, 0x44);
	reg8 |= 1 << 6;
	pci_write_config8(dev, 0x44, reg8);

	/* Decode port 0x60 & 0x64 (PS/2 keyboard) and port 0x62 & 0x66 (ACPI)*/
	reg8 = pci_read_config8(dev, 0x47);
	reg8 |= (1 << 5) | (1 << 6);
	pci_write_config8(dev, 0x47, reg8);

	/* SuperIO, LPC ROM */
	reg8 = pci_read_config8(dev, 0x48);
	/* Decode ports 0x2e-0x2f, 0x4e-0x4f (SuperI/O configuration) */
	reg8 |= (1 << 1) | (1 << 0);
	/* Decode variable LPC ROM address ranges 1&2 (see register 0x68-0x6b, 0x6c-0x6f) */
	reg8 |= (1 << 3) | (1 << 4);
	/* Decode port 0x70-0x73 (RTC) */
	reg8 |= 1 << 6;
	pci_write_config8(dev, 0x48, reg8);
}

/* what is its usage? */
u32 get_sbdn(u32 bus)
{
	pci_devfn_t dev;

	/* Find the device. */
	dev = PCI_DEV(bus, 0x14, 0);//pci_locate_device_on_bus(PCI_ID(0x1002, 0x4385), bus);
	return (dev >> 15) & 0x1f;
}

static u8 dual_core(void)
{
	return (pci_read_config32(PCI_DEV(0, 0x18, 3), 0xE8) & (0x3<<12)) != 0;
}

/*
 * RPR 2.6 C-state and VID/FID change for the K8 platform.
 */
static void enable_fid_change_on_sb(u32 sbbusn, u32 sbdn)
{
	u8 byte;
	byte = pmio_read(0x80);
	if (dual_core())
		byte |= 1 << 2 | 1 << 1;
	byte |= 1 << 3;
	byte |= 1 << 4;
	byte &= ~(1 << 7);
	pmio_write(0x80, byte);

	byte = pmio_read(0x7E);
	byte |= 1 << 6;
	byte &= ~(1 << 2);
	pmio_write(0x7E, byte);

	pmio_write(0x94, 0x01);

	byte = pmio_read(0x89);
	byte |= 1 << 4;
	pmio_write(0x89, byte);

	byte = pmio_read(0x9b);
	byte &= ~(7 << 4);
	byte |= 1 << 4;
	pmio_write(0x9b, byte);

	pmio_write(0x99, 0x10);

	pmio_write(0x9A, 0x00);
	pmio_write(0x96, 0x10);
	pmio_write(0x97, 0x00);

	byte = pmio_read(0x81);
	byte &= ~(1 << 1);
	pmio_write(0x81, byte);
}

void hard_reset(void)
{
	set_bios_reset();

	/* full reset */
	outb(0x0a, 0x0cf9);
	outb(0x0e, 0x0cf9);
}

void soft_reset(void)
{
	set_bios_reset();
	/* link reset */
	outb(0x06, 0x0cf9);
}

void sb800_pci_port80(void)
{
	u8 byte;
	pci_devfn_t dev;

	/* P2P Bridge */
	dev = PCI_DEV(0, 0x14, 4);//pci_locate_device(PCI_ID(0x1002, 0x4384), 0);

	/* Chip Control: Enable subtractive decoding */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 5;
	pci_write_config8(dev, 0x40, byte);

	/* Misc Control: Enable subtractive decoding if 0x40 bit 5 is set */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 7;
	pci_write_config8(dev, 0x4B, byte);

	/* The same IO Base and IO Limit here is meaningful because we set the
	 * bridge to be subtractive. During early setup stage, we have to make
	 * sure that data can go through port 0x80.
	 */
	/* IO Base: 0xf000 */
	byte = pci_read_config8(dev, 0x1C);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1C, byte);

	/* IO Limit: 0xf000 */
	byte = pci_read_config8(dev, 0x1D);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1D, byte);

	/* PCI Command: Enable IO response */
	byte = pci_read_config8(dev, 0x04);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x04, byte);

	/* LPC controller */
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);

	byte = pci_read_config8(dev, 0x4A);
	byte &= ~(1 << 5);	/* disable lpc port 80 */
	pci_write_config8(dev, 0x4A, byte);
}

#define BIT0	(1 << 0)
#define BIT1	(1 << 1)
#define BIT2	(1 << 2)
#define BIT3	(1 << 3)
#define BIT4	(1 << 4)
#define BIT5	(1 << 5)
#define BIT6	(1 << 6)
#define BIT7	(1 << 7)

struct pm_entry {
	u8	port;
	u8	mask;
	u8	bit;
};
struct pm_entry const pm_table[] =
{
	{0x5D, 0x00, BIT0},
	{0xD2, 0xCF, BIT4 + BIT5},
	{0x12, 0x00, BIT0},
	{0x28, 0xFF, BIT0},
	{0x44 + 3, 0x7F, BIT7},
	{0x48, 0xFF, BIT0},
	{0x00, 0xFF, 0x0E},
	{0x00 + 2, 0xFF, 0x40},
	{0x00 + 3, 0xFF, 0x08},
	{0x34, 0xEF, BIT0 + BIT1},
	{0xEC, 0xFD, BIT1},
	{0x5B, 0xF9, BIT1 + BIT2},
	{0x08, 0xFE, BIT2 + BIT4},
	{0x08 + 1, 0xFF, BIT0},
	{0x54, 0x00, BIT4 + BIT7},
	{0x04 + 3, 0xFD, BIT1},
	{0x74, 0xF6, BIT0 + BIT3},
	{0xF0, ~BIT2, 0x00},
	{0xF8,     0x00, 0x6C},
	{0xF8 + 1, 0x00, 0x27},
	{0xF8 + 2, 0x00, 0x00},
	{0xC4, 0xFE, 0x14},
	{0xC0 + 2, 0xBF, 0x40},
	{0xBE, 0xDD, BIT5},
	// HPET workaround
	{0x54 + 3, 0xFC, BIT0 + BIT1},
	{0x54 + 2, 0x7F, BIT7},
	{0x54 + 2, 0x7F, 0x00},
	{0xC4, ~(BIT2 + BIT4), BIT2 + BIT4},
	{0xC0, 0, 0xF9},
	{0xC0 + 1, 0x04, 0x03},
	{0xC2, 0x20, 0x58},
	{0xC2 + 1, 0, 0x40},
	{0xC2, ~(BIT4), BIT4},
	{0x74, 0x00, BIT0 + BIT1 + BIT2 + BIT4},
	{0xDE + 1, ~(BIT0 + BIT1), BIT0 + BIT1},
	{0xDE, ~BIT4, BIT4},
	{0xBA, ~BIT3, BIT3},
	{0xBA + 1, ~BIT6, BIT6},
	{0xBC, ~BIT1, BIT1},
	{0xED, ~(BIT0 + BIT1), 0},
	{0xDC, 0x7C, BIT0},
//  {0xFF, 0xFF, 0xFF},
};

void sb800_lpc_port80(void)
{
	u8 byte;
	pci_devfn_t dev;

	/* Enable LPC controller */
	byte = pmio_read(0xEC);
	byte |= 1 << 0;
	pmio_write(0xEC, byte);

	/* Enable port 80 LPC decode in pci function 3 configuration space. */
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	byte = pci_read_config8(dev, 0x4a);
	byte |= 1 << 5;		/* enable port 80 */
	pci_write_config8(dev, 0x4a, byte);
}

/* sbDevicesPorInitTable */
static void sb800_devices_por_init(void)
{
	pci_devfn_t dev;
	u8 byte;

	printk(BIOS_INFO, "sb800_devices_por_init()\n");
	/* SMBus Device, BDF:0-20-0 */
	printk(BIOS_INFO, "sb800_devices_por_init(): SMBus Device, BDF:0-20-0\n");
	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);

	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\n");
		/* NOT REACHED */
	}
	printk(BIOS_INFO, "SMBus controller enabled, sb revision is A%x\n",
		    get_sb800_revision());

	/* sbPorAtStartOfTblCfg */
	/* rpr 4.1.Set A-Link bridge access address.
	 * This is an I/O address. The I/O address must be on 16-byte boundary.  */
	//pci_write_config32(dev, 0xf0, AB_INDX);
	pmio_write(0xE0, AB_INDX & 0xFF);
	pmio_write(0xE1, (AB_INDX >> 8) & 0xFF);
	pmio_write(0xE2, (AB_INDX >> 16) & 0xFF);
	pmio_write(0xE3, (AB_INDX >> 24) & 0xFF);

	/* To enable AB/BIF DMA access, a specific register inside the BIF register space needs to be configured first. */
	/* 4.2:Enables the SB800 to send transactions upstream over A-Link Express interface. */
	axcfg_reg(0x04, 1 << 2, 1 << 2);
	//axindxc_reg(0x21, 0xff, 0);

	/* 4.15:Enabling Non-Posted Memory Write for the K8 Platform */
	axindxc_reg(0x10, 1 << 9, 1 << 9);
	/* END of sbPorAtStartOfTblCfg */

	/* sbDevicesPorInitTables */
	/* set smbus iobase */
	//pci_write_config32(dev, 0x90, SMBUS_IO_BASE | 1);
	/* The base address of SMBUS is set in a different way with sb700. */
	byte = (SMBUS_IO_BASE & 0xFF) | 1;
	pmio_write(0x2c, byte & 0xFF);
	pmio_write(0x2d, SMBUS_IO_BASE >> 8);

	/* AcpiMMioDecodeEn */
	byte = pmio_read(0x24);
	byte |= 1;
	byte &= ~(1 << 1);
	pmio_write(0x24, byte);
	/* enable smbus controller interface */
	//byte = pci_read_config8(dev, 0xd2);
	//byte |= (1 << 0);
	//pci_write_config8(dev, 0xd2, byte);

	/* KB2RstEnable */
	//pci_write_config8(dev, 0x40, 0x44);

	/* Enable ISA Address 0-960K decoding */
	//pci_write_config8(dev, 0x48, 0x0f);

	/* Enable ISA  Address 0xC0000-0xDFFFF decode */
	//pci_write_config8(dev, 0x49, 0xff);

	/* Enable decode cycles to IO C50, C51, C52 GPM controls. */
	//byte = pci_read_config8(dev, 0x41);
	//byte &= 0x80;
	//byte |= 0x33;
	//pci_write_config8(dev, 0x41, byte);

	/* Legacy DMA Prefetch Enhancement, CIM masked it. */
	/* pci_write_config8(dev, 0x43, 0x1); */

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* IDE Device, BDF:0-20-1 */
	printk(BIOS_INFO, "sb800_devices_por_init(): IDE Device, BDF:0-20-1\n");
	dev = PCI_DEV(0, 0x14, 1);//pci_locate_device(PCI_ID(0x1002, 0x439C), 0);
	/* Disable prefetch */
	byte = pci_read_config8(dev, 0x63);
	byte |= 0x1;
	pci_write_config8(dev, 0x63, byte);

	/* LPC Device, BDF:0-20-3 */
	printk(BIOS_INFO, "sb800_devices_por_init(): LPC Device, BDF:0-20-3\n");
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	/* DMA enable */
	pci_write_config8(dev, 0x40, 0x04);

	/* LPC Sync Timeout */
	pci_write_config8(dev, 0x49, 0xFF);

	/* Set LPC ROM size, it has been done in sb800_lpc_init().
	 * enable LPC ROM range, 0xfff8: 512KB, 0xfff0: 1MB;
	 * enable LPC ROM range, 0xfff8: 512KB, 0xfff0: 1MB
	 * pci_write_config16(dev, 0x68, 0x000e)
	 * pci_write_config16(dev, 0x6c, 0xfff0);*/

	/* Enable Tpm12_en and Tpm_legacy. I don't know what is its usage and copied from CIM. */
	pci_write_config8(dev, 0x7C, 0x05);

	/* P2P Bridge, BDF:0-20-4, the configuration of the registers in this dev are copied from CIM,
	 */
	printk(BIOS_INFO, "sb800_devices_por_init(): P2P Bridge, BDF:0-20-4\n");
	dev = PCI_DEV(0, 0x14, 4);//pci_locate_device(PCI_ID(0x1002, 0x4384), 0);

	/* Arbiter enable. */
	pci_write_config8(dev, 0x43, 0xff);

	/* Set PCDMA request into height priority list. */
	/* pci_write_config8(dev, 0x49, 0x1); */

	pci_write_config8(dev, 0x40, 0x26);

	pci_write_config8(dev, 0x0d, 0x40);
	pci_write_config8(dev, 0x1b, 0x40);
	/* Enable PCIB_DUAL_EN_UP will fix potential problem with PCI cards. */
	pci_write_config8(dev, 0x50, 0x01);

	/* SATA Device, BDF:0-17-0, Non-Raid-5 SATA controller */
	printk(BIOS_INFO, "sb800_devices_por_init(): SATA Device, BDF:0-18-0\n");
	dev = PCI_DEV(0, 0x11, 0);//pci_locate_device(PCI_ID(0x1002, 0x4390), 0);

	/*PHY Global Control*/
	pci_write_config16(dev, 0x86, 0x2C00);
}

/* sbPmioPorInitTable, Pre-initializing PMIO register space
* The power management (PM) block is resident in the PCI/LPC/ISA bridge.
* The PM regs are accessed via IO mapped regs 0xcd6 and 0xcd7.
* The index address is first programmed into IO reg 0xcd6.
* Read or write values are accessed through IO reg 0xcd7.
*/
#if 0
static void sb800_pmio_por_init(void)
{
	u8 byte, i;

	printk(BIOS_INFO, "sb800_pmio_por_init()\n");

	byte = pmio_read(0xD2);
	byte |= 3 << 4;
	pmio_write(0xD2, byte);

	byte = pmio_read(0x5D);
	byte &= 3;
	byte |= 1;
	pmio_write(0x5D, byte);

	/* Watch Dog Timer Control
	 * Set watchdog time base to 0xfec000f0 to avoid SCSI card boot failure.
	 * But I don't find WDT is enabled in SMBUS 0x41 bit3 in CIM.
	 */
	pmio_write(0x6c, 0xf0);
	pmio_write(0x6d, 0x00);
	pmio_write(0x6e, 0xc0);
	pmio_write(0x6f, 0xfe);

	/* rpr2.15: Enabling Spread Spectrum */
	byte = pmio_read(0x42);
	byte |= 1 << 7;
	pmio_write(0x42, byte);
	/* TODO: Check if it is necessary. IDE reset */
	byte = pmio_read(0xB2);
	byte |= 1 << 0;
	pmio_write(0xB2, byte);

	for (i = 0; i < sizeof(pm_table)/sizeof(struct pm_entry); i++) {
		byte = pmio_read(pm_table[i].port);
		byte &= pm_table[i].mask;
		byte |= pm_table[i].bit;
		pmio_write(pm_table[i].port, byte);
	}
	pmio_write(0x00, 0x0E);
	pmio_write(0x01, 0x00);
	pmio_write(0x02, 0x4F);
	pmio_write(0x03, 0x4A);
}
#endif

/*
* Add any south bridge setting.
*/
static void sb800_pci_cfg(void)
{
	pci_devfn_t dev;
	u8 byte;

	/* SMBus Device, BDF:0-20-0 */
	dev = PCI_DEV(0, 0x14, 0);//pci_locate_device(PCI_ID(0x1002, 0x4385), 0);
	/* Enable watchdog decode timer */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 3);
	pci_write_config8(dev, 0x41, byte);

	/* rpr 7.4. Set to 1 to reset USB on the software (such as IO-64 or IO-CF9 cycles)
	 * generated PCIRST#. */
	byte = pmio_read(0xF0);
	byte |= (1 << 2);
	pmio_write(0xF0, byte);

	/* IDE Device, BDF:0-20-1 */
	dev = PCI_DEV(0, 0x14, 1);//pci_locate_device(PCI_ID(0x1002, 0x439C), 0);
	/* Enable IDE Explicit prefetch, 0x63[0] clear */
	byte = pci_read_config8(dev, 0x63);
	byte &= 0xfe;
	pci_write_config8(dev, 0x63, byte);

	/* LPC Device, BDF:0-20-3 */
	/* The code below is ported from old chipset. It is not
	 * Mentioned in RPR. But I keep them. The registers and the
	 * comments are compatible. */
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	/* Enabling LPC DMA function. */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);
	/* Disabling LPC TimeOut. 0x48[7] clear. */
	byte = pci_read_config8(dev, 0x48);
	byte &= 0x7f;
	pci_write_config8(dev, 0x48, byte);
	/* Disabling LPC MSI Capability, 0x78[1] clear. */
	byte = pci_read_config8(dev, 0x78);
	byte &= 0xfd;
	pci_write_config8(dev, 0x78, byte);

	/* SATA Device, BDF:0-17-0, Non-Raid-5 SATA controller */
	dev = PCI_DEV(0, 0x11, 0);//pci_locate_device(PCI_ID(0x1002, 0x4390), 0);
	/* rpr7.12 SATA MSI and D3 Power State Capability. */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x40, byte);
	if (get_sb800_revision() <= 0x12)
		pci_write_config8(dev, 0x34, 0x70); /* set 0x61 to 0x70 if S1 is not supported. */
	else
		pci_write_config8(dev, 0x34, 0x50); /* set 0x61 to 0x50 if S1 is not supported. */
	byte &= ~(1 << 0);
	pci_write_config8(dev, 0x40, byte);
}

/*
*/
static void sb800_por_init(void)
{
	/* sbDevicesPorInitTable + sbK8PorInitTable */
	sb800_devices_por_init();

	/* sbPmioPorInitTable + sbK8PmioPorInitTable */
	//sb800_pmio_por_init();
}

/*
* It should be called during early POST after memory detection and BIOS shadowing but before PCI bus enumeration.
*/
static void sb800_before_pci_init(void)
{
	sb800_pci_cfg();
}

/*
* This function should be called after enable_sb800_smbus().
*/
static void sb800_early_setup(void)
{
	printk(BIOS_INFO, "sb800_early_setup()\n");
	sb800_por_init();
	sb800_acpi_init();
}

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos)
{
	int i;
	printk(BIOS_DEBUG, "Writing %x of size %d to nvram pos: %d\n", dword, size, nvram_pos);

	for (i = 0; i < size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
		nvram_pos++;
	}

	return nvram_pos;
}

int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos)
{
	u32 data = *old_dword;
	int i;
	for (i = 0; i < size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		data &= ~(0xff << (i * 8));
		data |= inb(BIOSRAM_DATA) << (i *8);
		nvram_pos++;
	}
	*old_dword = data;
	printk(BIOS_DEBUG, "Loading %x of size %d to nvram pos:%d\n", *old_dword, size,
		nvram_pos-size);
	return nvram_pos;
}

int acpi_get_sleep_type(void)
{
	u16 tmp;
	tmp = inw(ACPI_PM1_CNT_BLK);
	return ((tmp & (7 << 10)) >> 10);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	uint32_t xdata = 0;
	int xnvram_pos = 0xfc, xi;
	if (acpi_get_sleep_type() != 3)
		return 0;
	for (xi = 0; xi < 4; xi++) {
		outb(xnvram_pos, BIOSRAM_INDEX);
		xdata &= ~(0xff << (xi * 8));
		xdata |= inb(BIOSRAM_DATA) << (xi *8);
		xnvram_pos++;
	}
	return xdata;
}

#endif
