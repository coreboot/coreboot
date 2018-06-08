/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

/*
 * Part of this file is from cx700 port, part of is from cn700 port,
 * and acpi_is_wakeup_early_via_VX800() is part of Rudolf's S3 patch.
 */

#define PAYLOAD_IS_SEABIOS 0

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <lib.h>
#include <northbridge/via/vx800/vx800.h>
#include <cpu/x86/bist.h>
#include <delay.h>
#include <string.h>
/* This file contains the board-special SI value for raminit.c. */
#include "driving_clk_phase_data.c"
#include <northbridge/via/vx800/raminit.h>
#include "northbridge/via/vx800/raminit.c"
#include "wakeup.h"
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83697hf/w83697hf.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83697HF_SP1)
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

/*
 * This acpi_is_wakeup_early_via_VX800 is from Rudolf's patch on the list:
 * https://www.coreboot.org/pipermail/coreboot/2008-January/028787.html.
 */
static int acpi_is_wakeup_early_via_vx800(void)
{
	pci_devfn_t dev;
	u16 tmp, result;

	printk(BIOS_DEBUG, "In acpi_is_wakeup_early_via_vx800\n");
	/* Power management controller */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX855_LPC), 0);

	if (dev == PCI_DEV_INVALID)
		die("Power management controller not found\n");

	/* Set ACPI base address to I/O VX800_ACPI_IO_BASE. */
	pci_write_config16(dev, 0x88, VX800_ACPI_IO_BASE | 0x1);

	/* Enable ACPI access RTC signal gated with PSON. */
	pci_write_config8(dev, 0x81, 0x84);

	tmp = inw(VX800_ACPI_IO_BASE + 0x04);
	result = ((tmp & (7 << 10)) >> 10) == 1 ? 3 : 0;
	printk(BIOS_DEBUG, "         boot_mode=%04x\n", result);
	return result;
}

/* All content of this function came from the cx700 port of coreboot. */
static void enable_mainboard_devices(void)
{
	pci_devfn_t dev;
#if 0
	/*
	 * Add and close this switch, since some line cause error, some
	 * written at elsewhere (stage1 stage2).
	 */
	u8 regdata;
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX855_LPC), 0);

	/* Disable GP3. */
	pci_write_config8(dev, 0x98, 0x00);

	pci_write_config8(dev, 0x50, 0x80);	/* Disable mc97. */

	/*
	 * Martin: Disable internal KBC configuration.
	 *
	 * Internal Config is needed to decide which key can be pressed to
	 * resume from s3.
	 */
	pci_write_config8(dev, 0x51, 0x2d);

	/* This causes irq0 can not be triggerd, since bit 5 was set to 0. */
	/* pci_write_config8(dev, 0x58, 0x42); */

	/* These writing may... TODO */
	regdata = pci_read_config8(dev, 0x58);
	regdata |= 0x41;
	pci_write_config8(dev, 0x58, regdata);
	pci_write_config8(dev, 0x59, 0x80);
	pci_write_config8(dev, 0x5b, 0x01);
#endif

	printk(BIOS_DEBUG, "In enable_mainboard_devices\n");

	/* Enable P2P Bridge Header for external PCI bus. */
	dev = pci_locate_device(PCI_ID(0x1106, 0xa353), 0);
	pci_write_config8(dev, 0x4f, 0x41);

	/*
	 * "5324" already is the default value of the PCI IDE device, cancel
	 * this PCI write.
	 *
	 * [william 20080124]: Fix bug that can not boot Ubuntu at the
	 * beginning time.
	 */
#if 0
	dev = 0;
	dev = pci_locate_device(PCI_ID(0x1106, PCI_DEVICE_ID_VIA_VX855_IDE), 0);

	uint16_t values;
	values = pci_read_config16(dev, 0xBA);
	values &= ~0xffff;
	values |= 0x5324;
	pci_write_config16(dev, 0xBA, values);
#endif
}

/*
 * Most content of this function came from the cx700 port of coreboot.
 * Turn on the shadow of E-seg.
 */
static void enable_shadow_ram(void)
{
	uint8_t shadowreg;

	/*
	 * Changed the value from 0x2a to 0x3f. "read only" may block "write"?
	 * and maybe in C-seg "write" will be needed?
	 */
	pci_write_config8(PCI_DEV(0, 0, 3), 0x80, 0xff);

	/* 0xf0000-0xfffff - ACPI tables */
	shadowreg = pci_read_config8(PCI_DEV(0, 0, 3), 0x83);
	shadowreg |= 0x30;
	pci_write_config8(PCI_DEV(0, 0, 3), 0x83, shadowreg);

	/* 0xe0000-0xeffff - elfload? */
	/*
	 * In s3 resume process, wakeup.c, I use E-seg to hold the code
	 * (which can not locate in the area to be covered) that will copy
	 * 0-A-seg and F-seg from TOP-mem back to their normal location.
	 */
	pci_write_config8(PCI_DEV(0, 0, 3), 0x82, 0xff);

#if 0
	/* Enable shadow RAM as normal DRAM */
	/* 0xc0000-0xcffff - VGA BIOS */
	pci_write_config8(PCI_DEV(0, 0, 3), 0x80, 0x2a);
	pci_write_config8(PCI_DEV(0, 0, 7), 0x61, 0x00);
	/* 0xd0000-0xdffff - ?? */
	/* pci_write_config8(PCI_DEV(0, 0, 3), 0x81, 0xff); */
	/* pci_write_config8(PCI_DEV(0, 0, 7), 0x62, 0xff); */

	/* Do it again for the vlink controller. */
	shadowreg = pci_read_config8(PCI_DEV(0, 0, 7), 0x63);
	shadowreg |= 0x30;
	pci_write_config8(PCI_DEV(0, 0, 7), 0x63, shadowreg);
#endif
}

/*
 * Added this table 2008-11-28.
 * This table contains the value needed to be set before begin to init DRAM.
 * Note: REV_Bx should be checked for changes when porting a new board!
 */
static const struct VIA_PCI_REG_INIT_TABLE mNbStage1InitTbl[] = {
	/* VT3409 no PCI-E */
	{ 0x00, 0xFF, NB_APIC_REG(0x61), 0xFF, 0x0E },	// Set Exxxxxxx as pcie mmio config range
	{ 0x00, 0xFF, NB_APIC_REG(0x60), 0xF4, 0x0B },	// Support extended cfg address of pcie
	// { 0x00, 0xFF, NB_APIC_REG(0x42), 0xF9, 0x02 }, // APIC Interrupt((BT_INTR)) Control
	// Set ROMSIP value by software

	/*
	{ 0x00, 0xFF, NB_HOST_REG(0x70), 0x77, 0x33 }, // 2x Host Adr Strobe/Pad Pullup Driving = 3
	{ 0x00, 0xFF, NB_HOST_REG(0x71), 0x77, 0x33 }, // 2x Host Adr Strobe/Pad Pulldown Driving = 3
	{ 0x00, 0xFF, NB_HOST_REG(0x72), 0x77, 0x33 }, // 4x Host Dat Strobe/Pad Pullup Driving = 3
	{ 0x00, 0xFF, NB_HOST_REG(0x73), 0x77, 0x33 }, // 4x Host Dat Strobe/Pad Pulldown Driving = 3
	{ 0x00, 0xFF, NB_HOST_REG(0x74), 0xFF, 0x21 }, // Memory I/F timing ctrl
	{ 0x00, 0xFF, NB_HOST_REG(0x74), 0xFF, 0xE1 }, // Memory I/F timing ctrl
	{ 0x00, 0xFF, NB_HOST_REG(0x75), 0xFF, 0x18 }, // AGTL+ I/O Circuit
	{ 0x00, 0xFF, NB_HOST_REG(0x76), 0xFB, 0x0C }, // AGTL+ Compensation Status
	{ 0x00, 0xFF, NB_HOST_REG(0x78), 0xFF, 0x33 }, // 2X AGTL+ Auto Compensation Offset
	{ 0x00, 0xFF, NB_HOST_REG(0x79), 0xFF, 0x33 }, // 4X AGTL+ Auto Compensation Offset
	{ 0x00, 0xFF, NB_HOST_REG(0x7A), 0x3F, 0x72 }, // AGTL Compensation Status
	{ 0x00, 0xFF, NB_HOST_REG(0x7A), 0x3F, 0x77 }, // AGTL Compensation Status
	{ 0x00, 0xFF, NB_HOST_REG(0x7B), 0xFF, 0x44 }, // Input Host Address / Host Strobe Delay Control for HA Group
	{ 0x00, 0xFF, NB_HOST_REG(0x7B), 0xFF, 0x22 }, // Input Host Address / Host Strobe Delay Control for HA Group
	{ 0x00, 0xFF, NB_HOST_REG(0x7C), 0xFF, 0x00 }, // Output Delay Control of PAD for HA Group
	{ 0x00, 0xFF, NB_HOST_REG(0x7D), 0xFF, 0xAA }, // Host Address / Address Clock Output Delay Control (Only for P4 Bus)
	{ 0x00, 0xFF, NB_HOST_REG(0x7E), 0xFF, 0x10 }, // Host Address CKG Rising / Falling Time Control (Only for P4 Bus)
	{ 0x00, 0xFF, NB_HOST_REG(0x7E), 0xFF, 0x40 }, // Host Address CKG Rising / Falling Time Control (Only for P4 Bus)
	{ 0x00, 0xFF, NB_HOST_REG(0x7F), 0xFF, 0x10 }, // Host Address CKG Rising / Falling Time Control (Only for P4 Bus)
	{ 0x00, 0xFF, NB_HOST_REG(0x7F), 0xFF, 0x40 }, // Host Address CKG Rising / Falling Time Control (Only for P4 Bus)
	{ 0x00, 0xFF, NB_HOST_REG(0x80), 0x3F, 0x44 }, // Host Data Receiving Strobe Delay Ctrl 1
	{ 0x00, 0xFF, NB_HOST_REG(0x81), 0xFF, 0x44 }, // Host Data Receiving Strobe Delay Ctrl 2
	{ 0x00, 0xFF, NB_HOST_REG(0x82), 0xFF, 0x00 }, // Output Delay of PAD for HDSTB
	{ 0x00, 0xFF, NB_HOST_REG(0x83), 0xFF, 0x00 }, // Output Delay of PAD for HD
	{ 0x00, 0xFF, NB_HOST_REG(0x84), 0xFF, 0x44 }, // Host Data / Strobe CKG Control (Group 0)
	{ 0x00, 0xFF, NB_HOST_REG(0x85), 0xFF, 0x44 }, // Host Data / Strobe CKG Control (Group 1)
	{ 0x00, 0xFF, NB_HOST_REG(0x86), 0xFF, 0x44 }, // Host Data / Strobe CKG Control (Group 2)
	{ 0x00, 0xFF, NB_HOST_REG(0x87), 0xFF, 0x44 }, // Host Data / Strobe CKG Control (Group 3)
	*/

	// CPU Host Bus Control
	{ 0x00, 0xFF, NB_HOST_REG(0x50), 0x1F, 0x08 },	// Request phase ctrl: Dynamic Defer Snoop Stall Count = 8
	// { 0x00, 0xFF, NB_HOST_REG(0x51), 0xFF, 0x7F },	// CPU I/F Ctrl-1: Disable Fast DRDY and RAW
	{ 0x00, 0xFF, NB_HOST_REG(0x51), 0xFF, 0x7C },	// CPU I/F Ctrl-1: Disable Fast DRDY and RAW
	{ 0x00, 0xFF, NB_HOST_REG(0x52), 0xCB, 0xCB },	// CPU I/F Ctrl-2: Enable all for performance
	// { 0x00, 0xFF, NB_HOST_REG(0x53), 0xFF, 0x88 },	// Arbitration: Host/Master Occupancy timer = 8*4 HCLK
	{ 0x00, 0xFF, NB_HOST_REG(0x53), 0xFF, 0x44 },	// Arbitration: Host/Master Occupancy timer = 4*4 HCLK
	{ 0x00, 0xFF, NB_HOST_REG(0x54), 0x1E, 0x1C },	// Misc Ctrl: Enable 8QW burst Mem Access
	// { 0x00, 0xFF, NB_HOST_REG(0x55), 0x06, 0x06 },	// Miscellaneous Control 2
	{ 0x00, 0xFF, NB_HOST_REG(0x55), 0x06, 0x04 },	// Miscellaneous Control 2
	{ 0x00, 0xFF, NB_HOST_REG(0x56), 0xF7, 0x63 },	// Write Policy 1
	// { 0x00, 0xFF, NB_HOST_REG(0x59), 0x3D, 0x01 },	// CPU Miscellaneous Control 1, enable Lowest-Priority IPL
	// { 0x00, 0xFF, NB_HOST_REG(0x5c), 0xFF, 0x00 },	// CPU Miscellaneous Control 2
	{ 0x00, 0xFF, NB_HOST_REG(0x5D), 0xFF, 0xA2 },	// Write Policy
	{ 0x00, 0xFF, NB_HOST_REG(0x5E), 0xFF, 0x88 },	// Bandwidth Timer
	{ 0x00, 0xFF, NB_HOST_REG(0x5F), 0x46, 0x46 },	// CPU Misc Ctrl
	// { 0x00, 0xFF, NB_HOST_REG(0x90), 0xFF, 0x0B },	// CPU Miscellaneous Control 3
	// { 0x00, 0xFF, NB_HOST_REG(0x96), 0x0B, 0x0B },	// CPU Miscellaneous Control 2
	{ 0x00, 0xFF, NB_HOST_REG(0x96), 0x0B, 0x0A },	// CPU Miscellaneous Control 2
	{ 0x00, 0xFF, NB_HOST_REG(0x98), 0xC1, 0x41 },	// CPU Miscellaneous Control 3
	{ 0x00, 0xFF, NB_HOST_REG(0x99), 0x0E, 0x06 },	// CPU Miscellaneous Control 4

	// Set APIC and SMRAM
	{ 0x00, 0xFF, NB_HOST_REG(0x97), 0xFF, 0x00 },	// APIC Related Control
	{ 0x00, 0xFF, NB_DRAMC_REG(0x86), 0xD6, 0x29 },	// SMM and APIC Decoding: enable APIC, MSI and SMRAM A-Seg
	{ 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }	// End of the table
};

#define USE_VCP     1		/* 0 means "use DVP". */
#define USE_COM1    1
#define USE_COM2    0

#define gCom1Base   0x3f8
#define gCom2Base   0x2f8

#if 0
static void EmbedComInit(void)
{
	u8 ByteVal;
	u16 ComBase;

	/* Enable NB multiple function control. */
	ByteVal = pci_read_config8(PCI_DEV(0, 0, 0), 0x4f);
	ByteVal = ByteVal | 0x01;
	pci_write_config8(PCI_DEV(0, 0, 0), 0x4f, ByteVal);

	/* VGA enable. */
	ByteVal = pci_read_config8(PCI_DEV(0, 0, 3), 0xA1);
	ByteVal = ByteVal | 0x80;
	pci_write_config8(PCI_DEV(0, 0, 3), 0xA1, ByteVal);

	ByteVal = pci_read_config8(PCI_DEV(0, 0, 3), 0xA7);
	ByteVal = ByteVal | 0x08;
	pci_write_config8(PCI_DEV(0, 0, 3), 0xA7, ByteVal);

	/* Enable P2P IO/mem. */
	ByteVal = pci_read_config8(PCI_DEV(0, 1, 0), 0x4);
	ByteVal = ByteVal | 0x07;
	pci_write_config8(PCI_DEV(0, 1, 0), 0x4, ByteVal);

	/* Turn on graphic chip I/O port port access. */
	ByteVal = inb(0x3C3);
	ByteVal = ByteVal | 0x01;
	outb(ByteVal, 0x3C3);

	/* Turn off graphic chip register protection. */
	outb(0x10, 0x3C4);
	ByteVal = inb(0x3C5);
	ByteVal = ByteVal | 0x01;
	outb(ByteVal, 0x3C5);

	/* South module pad share enable 0x3C5.78[7]. */
	outb(0x78, 0x3C4);
	ByteVal = inb(0x3C5);
	ByteVal = ByteVal | 0x80;
	outb(ByteVal, 0x3C5);

	/* Enable UART function multiplex with DVP or VCP pad D17F0Rx46[7,6]. */
	ByteVal = pci_read_config8(PCI_DEV(0, 17, 0), 0x46);
	if (USE_VCP == 1)
		ByteVal = (ByteVal & 0x3F) | 0x40; /* Multiplex with VCP. */
	else
		ByteVal = (ByteVal & 0x3F) | 0xC0; /* Multiplex with DVP. */
	pci_write_config8(PCI_DEV(0, 17, 0), 0x46, ByteVal);

	/* Enable embedded COM1 and COM2 D17F0RxB0[5,4]. */
	ByteVal = pci_read_config8(PCI_DEV(0, 17, 0), 0xB0);
	ByteVal = ByteVal & 0xcf;
	/* Multiplex with VCP. */
	if (USE_COM1 == 1)
		ByteVal = ByteVal | 0x10;
	if (USE_COM2 == 1)
		ByteVal = ByteVal | 0x20;
	pci_write_config8(PCI_DEV(0, 17, 0), 0xB0, ByteVal);

	if (USE_COM1 == 1)
		ComBase = gCom1Base;
	else
		ComBase = gCom2Base;

//noharddrive

	/* Set embedded COM1 I/O base = 0x3E8 (D17F0RB4, ByteVal = 0xFD) */
	if (USE_COM1 == 1) {
		ByteVal = (u8) ((gCom1Base >> 3) | 0x80);
		pci_write_config8(PCI_DEV(0, 17, 0), 0xB4, ByteVal);
		ByteVal = pci_read_config8(PCI_DEV(0, 17, 0), 0xb2);
		ByteVal = (ByteVal & 0xf0) | 0x04;
		pci_write_config8(PCI_DEV(0, 17, 0), 0xB2, ByteVal);
	}

	/* Set embedded COM2 I/O base = 0x2E8 (D17F0RB5, ByteVal = 0xDD). */
	if (USE_COM2 == 1) {
		ByteVal = (u8) ((gCom2Base >> 3) | 0x80);
		pci_write_config8(PCI_DEV(0, 17, 0), 0xB5, ByteVal);
		ByteVal = pci_read_config8(PCI_DEV(0, 17, 0), 0xb2);
		ByteVal = (ByteVal & 0x0f) | 0x30;
		pci_write_config8(PCI_DEV(0, 17, 0), 0xB2, ByteVal);
	}
	/* No port 80 biger then 0x10. */

	/* Disable interrupt. */
	ByteVal = inb(ComBase + 3);
	outb(ByteVal & 0x7F, ComBase + 3);
	outb(0x00, ComBase + 1);

	/* Set BAUD rate. */
	ByteVal = inb(ComBase + 3);
	outb(ByteVal | 0x80, ComBase + 3);
	outb(0x01, ComBase);
	outb(0x00, ComBase + 1);

	/* Set frame format. */
	ByteVal = inb(ComBase + 3);
	outb(ByteVal & 0x3F, ComBase + 3);
	outb(0x03, ComBase + 3);
	outb(0x00, ComBase + 2);
	outb(0x00, ComBase + 4);

	/* SOutput("Embedded COM output\n"); */
	/* while (1); */
}
#endif

/* cache_as_ram.inc jumps to here. */
void main(unsigned long bist)
{
	u16 boot_mode;
	u8 rambits, Data8, Data;
	pci_devfn_t device;
	/* device_t dev; */

	/*
	 * Enable multifunction for northbridge. These 4 lines (until
	 * console_init()) are the same with epia-cn port.
	 */
	pci_write_config8(PCI_DEV(0, 0, 0), 0x4f, 0x01);
	/* EmbedComInit(); */
	winbond_set_clksel_48(SUPERIO_DEV);
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	/* enable_vx800_serial(); */

	/*
	 * 1. D15F0
	 * a) RxBAh = 71h
	 * b) RxBBh = 05h
	 * c) RxBEh = 71h
	 * d) RxBFh = 05h
	 *
	 * 2. D17F0
	 * a) RxA0h = 06h
	 * b) RxA1h = 11h
	 * c) RxA2h = 27h
	 * d) RxA3h = 32h
	 * e) Rx79h = 40h
	 * f) Rx72h = 27h
	 * g) Rx73h = 32h
	*/

	pci_write_config16(PCI_DEV(0, 0xf, 0), 0xBA,
			   PCI_DEVICE_ID_VIA_VX855_IDE);
	pci_write_config16(PCI_DEV(0, 0xf, 0), 0xBE,
			   PCI_DEVICE_ID_VIA_VX855_IDE);
	pci_write_config16(PCI_DEV(0, 0x11, 0), 0xA0, PCI_VENDOR_ID_VIA);
	pci_write_config16(PCI_DEV(0, 0x11, 0), 0xA2,
			   PCI_DEVICE_ID_VIA_VX855_LPC);
	Data8 = pci_read_config8(PCI_DEV(0, 0x11, 0), 0x79);
	Data8 &= ~0x40;
	Data8 |= 0x40;
	pci_write_config8(PCI_DEV(0, 0x11, 0), 0x79, Data8);
	pci_write_config16(PCI_DEV(0, 0x11, 0), 0x72,
			   PCI_DEVICE_ID_VIA_VX855_LPC);

	/*
	 * There are two function definitions of console_init(), while the
	 * src/arch/x86/lib is the right one.
	 */
	console_init();

	/* Decide if this is a s3 wakeup or a normal boot. */
	boot_mode = acpi_is_wakeup_early_via_vx800();

	/*
	 * 2008-11-27 Add this, to transfer "cpu restart" to "cold boot".
	 * When this boot is not a S3 resume, and PCI registers had been
	 * written, then this must be a CPU restart (result of OS reboot cmd),
	 * so we need a real "cold boot".
	 */
	if ((boot_mode != 3)
	    && (pci_read_config8(PCI_DEV(0, 0, 3), 0x80) != 0)) {
		outb(6, 0xcf9);
	}

	/* x86 cold boot I/O cmd. */
	/* These 2 lines are the same with epia-cn port. */
	enable_smbus();

	/* This fix does help vx800!, but vx855 doesn't need this. */
	/* smbus_fixup(ctrl.channel0, ARRAY_SIZE(ctrl.channel0)); */

	/* Halt if there was a built-in self test failure. */
	report_bist_failure(bist);

	printk(BIOS_DEBUG, "Enabling mainboard devices\n");
	enable_mainboard_devices();

	/*
	 * Get NB chip revision from D0F4RxF6, revision will be used in
	 * via_pci_inittable.
	 */
	device = PCI_DEV(0, 0, 4);
	Data = pci_read_config8(device, 0xf6);
	printk(BIOS_DEBUG, "NB chip revision = %02x\n", Data);

	/* Make NB ready before DRAM init. */
	via_pci_inittable(Data, mNbStage1InitTbl);

	/*
	 * When resume from s3, DRAM init is skipped, so need to recovery
	 * any PCI register related to DRAM init. d0f3 didn't lose its power
	 * during whole s3 time, so any register not belonging to d0f3 needs
	 * to be recovered.
	 */
#if 1
	if (boot_mode == 3) {
		u8 i;
		u8 ramregs[] = { 0x43, 0x42, 0x41, 0x40 };
		DRAM_SYS_ATTR DramAttr;

		printk(BIOS_DEBUG, "This is an S3 wakeup\n");

		memset(&DramAttr, 0, sizeof(DRAM_SYS_ATTR));
		/*
		 * Step 1: DRAM detection; DDR1 or DDR2; Get SPD Data;
		 * Rank Presence; 64 or 128bit; Unbuffered or registered;
		 * 1T or 2T.
		 */
		DRAMDetect(&DramAttr);

		/*
		 * Begin to get RAM size, 43,42 41 40 contains the end
		 * address of last rank in DDR2 slot.
		 */
		device = PCI_DEV(0, 0, 3);
		for (rambits = 0, i = 0; i < ARRAY_SIZE(ramregs); i++) {
			rambits = pci_read_config8(device, ramregs[i]);
			if (rambits != 0)
				break;
		}

		DRAMDRDYSetting(&DramAttr);

		Data = 0x80;	/* This value is same with DevInit.c. */
		pci_write_config8(PCI_DEV(0, 0, 4), 0xa3, Data);
		pci_write_config8(PCI_DEV(0, 17, 7), 0x60, rambits << 2);
		Data = pci_read_config8(MEMCTRL, 0x88);
		pci_write_config8(PCI_DEV(0, 17, 7), 0xE5, Data);

		/* Just copy this function from draminit to here! */
		DRAMRegFinalValue(&DramAttr);

		/* Just copy this function from draminit to here! */
		SetUMARam();

		printk(BIOS_DEBUG, "Resume from S3, RAM init was ignored\n");
	} else {
		ddr2_ram_setup();
		ram_check(0, 640 * 1024);
	}
#endif

	/* ddr2_ram_setup(); */
	/* This line is the same with cx700 port. */
	enable_shadow_ram();

	/*
	 * For coreboot most time of S3 resume is the same as normal boot,
	 * so some memory area under 1M become dirty, so before this happen,
	 * I need to backup the content of mem to top-mem.
	 *
	 * I will reserve the 1M top-men in LBIO table in coreboot_table.c
	 * and recovery the content of 1M-mem in wakeup.c.
	 */
#if PAYLOAD_IS_SEABIOS == 1
	if (boot_mode == 3) {
		/* An idea of Libo.Feng at amd.com in
		 * https://www.coreboot.org/pipermail/coreboot/2008-December/043111.html
		 *
		 * I want move the 1M data, I have to set some MTRRs myself.
		 * Setting MTRR before back memory save s3 resume time about
		 * 0.14 seconds.
		 *
		 * !!! Since CAR stack uses cache, and we are using cache
		 * here, we must be careful:
		 *
		 * 1. during this MTRR code, must no function call (after
		 *    this MTRR, I think it should be OK to use function).
		 * 2. Before stack switch, no use variable that have value
		 *    set before this.
		 * 3. Due to 2, take care of "cpu_reset", I directlly set it
		 *    to ZERO.
		 */
		u32 memtop = *(u32 *) WAKE_MEM_INFO;
		u32 memtop1 = *(u32 *) WAKE_MEM_INFO - 0x100000;
		u32 memtop2 = *(u32 *) WAKE_MEM_INFO - 0x200000;
		u32 memtop3 = *(u32 *) WAKE_MEM_INFO - 64 * 1024 - 0x100000;
		u32 memtop4 =
		    *(u32 *) WAKE_MEM_INFO - 64 * 1024 - 0x100000 + 0xe0000;
#if 0
		__asm__ volatile (
			"movl $0x204, %%ecx\n\t"
			"xorl %%edx, %%edx\n\t"
			"movl %0,%%eax\n\t"
			"orl $(0 | 6), %%eax\n\t"
			"wrmsr\n\t"

			"movl $0x205, %%ecx\n\t"
			"xorl %%edx, %%edx\n\t"
			"movl $0x100000,%%eax\n\t"
			"decl %%eax\n\t"
			"notl %%eax\n\t"
			"orl $(0 | 0x800), %%eax\n\t"
			"wrmsr\n\t"
			::"g"(memtop2)
		);

		__asm__ volatile (
			"movl $0x206, %%ecx\n\t"
			"xorl %%edx, %%edx\n\t"
			"movl %0,%%eax\n\t"
			"orl $(0 | 6), %%eax\n\t"
			"wrmsr\n\t"

			"movl $0x207, %%ecx\n\t"
			"xorl %%edx, %%edx\n\t"
			"movl $0x100000,%%eax\n\t"
			"decl %%eax\n\t"
			"notl %%eax\n\t"
			"orl $(0 | 0x800), %%eax\n\t"
			"wrmsr\n\t"
			::"g"(memtop1)
		);

		__asm__ volatile (
			"movl $0x208, %ecx\n\t"
			"xorl %edx, %edx\n\t"
			"movl $0,%eax\n\t"
			"orl $(0 | 6), %eax\n\t"
			"wrmsr\n\t"

			"movl $0x209, %ecx\n\t"
			"xorl %edx, %edx\n\t"
			"movl $0x100000,%eax\n\t"
			"decl %eax\n\t"
			"notl %eax\n\t"
			"orl $(0 | 0x800), %eax\n\t"
			"wrmsr\n\t"
		);
#endif

		/*
		 * WAKE_MEM_INFO is inited in get_set_top_available_mem()
		 * in tables.c these two memcpy() not not be enabled if set
		 * the MTRR around this two lines.
		 */
#if 0
		__asm__ volatile (
			"movl $0, %%esi\n\t"
			"movl %0, %%edi\n\t"
			"movl $0xa0000, %%ecx\n\t"
			"shrl $2, %%ecx\n\t"
			"rep movsd\n\t"
			::"g"(memtop3)
		);

		__asm__ volatile (
			"movl $0xe0000, %%esi\n\t"
			"movl %0, %%edi\n\t"
			"movl $0x20000, %%ecx\n\t"
			"shrl $2, %%ecx\n\t"
			"rep movsd\n\t"
			::"g"(memtop4)
		);
#endif
		/* This can have function call, because no variable used before this. */
		printk(BIOS_DEBUG, "Copy memory to high memory to protect s3 wakeup vector code\n");
		memcpy((unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024 -
				 0x100000), (unsigned char *)0, 0xa0000);
		memcpy((unsigned char *)((*(u32 *) WAKE_MEM_INFO) - 64 * 1024 -
		 0x100000 + 0xe0000), (unsigned char *)0xe0000, 0x20000);

		/* Restore the MTRR previously modified. */
#if 0
		__asm__ volatile (
			"wbinvd\n\t"
			"xorl %edx, %edx\n\t"
			"xorl %eax, %eax\n\t"
			"movl $0x204, %ecx\n\t"
			"wrmsr\n\t"
			"movl $0x205, %ecx\n\t"
			"wrmsr\n\t"
			"movl $0x206, %ecx\n\t"
			"wrmsr\n\t"
			"movl $0x207, %ecx\n\t"
			"wrmsr\n\t"
			"movl $0x208, %ecx\n\t"
			"wrmsr\n\t"
			"movl $0x209, %ecx\n\t"
			"wrmsr\n\t"
		);
#endif
	}
#endif
}
