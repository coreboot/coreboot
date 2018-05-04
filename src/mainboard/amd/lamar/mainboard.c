/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2014 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <northbridge/amd/pi/00630F01/pci_devs.h>
#include <southbridge/amd/pi/hudson/amd_pci_int_defs.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <southbridge/amd/pi/hudson/pci_devs.h>
#include <southbridge/amd/common/amd_pci_util.h>

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 *
 * These values are used by the PCI configuration space,
 * MP Tables.  TODO: Make ACPI use these values too.
 */
const u8 mainboard_picr_data[] = {
	/* INTA# - INTH# */
	[0x00] = 0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0xFA,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerMon, SD */
	[0x10] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x0A,
	[0x18] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x28] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/22 INTA-C */
	[0x30] = 0x0B,0x0A,0x0B,0x0A,0x0B,0x0A,0x0B,0x1F,
	[0x38] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* [40..41] IDE, SATA */
	[0x40] = 0x1F,0x0F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x48] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* [50..53] GPPInt0 - 3 */
	[0x50] = 0x0A,0x0B,0x0A,0x0B,
	[0x58] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x60] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x68] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x70] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x78] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* [80..81] Northbridge devices (indices above C00/C01 range) */
	[0x80] = 0x0C,0x1F,
};

const u8 mainboard_intr_data[] = {
	/* INTA# - INTH# */
	[0x00] = 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerMon, SD */
	[0x10] = 0x09,0x1F,0x1F,0x10,0x1F,0x1F,0x1F,0x10,
	[0x18] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x28] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/20/22 INTA-C */
	[0x30] = 0x11,0x12,0x11,0x12,0x11,0x12,0x11,0x1F,
	[0x38] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* SATA */
	[0x40] = 0x11,0x13,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x48] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* [50..53] GPPInt0 - 3 */
	[0x50] = 0x10,0x11,0x12,0x13,
	[0x58] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x60] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x68] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x70] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	[0x78] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* [80..81] Northbridge devices (indices above C00/C01 range) */
	[0x80] = 0x17,0x10,
};

/*
 * This table defines the index into the picr/intr_data
 * tables for each device.  Any enabled device and slot
 * that uses hardware interrupts should have an entry
 * in this table to define its index into the FCH
 * PCI_INTR register 0xC00/0xC01.  This index will define
 * the interrupt that it should use.  Putting PIRQ_A into
 * the PIN A index for a device will tell that device to
 * use PIC IRQ 10 if it uses PIN A for its hardware INT.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	{NB_PCIE_PORT1_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* PCIe J119:  02.1 */
	{NB_PCIE_PORT3_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* PCIe J118:  03.1 */
	{NB_PCIE_PORT4_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* PCIe J120:  03.2 */
	{XHCI_DEVFN,	{PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* XHCI:       10.0 */
	{XHCI2_DEVFN,	{PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},	/* XHCI:       10.1 */
	{SATA_DEVFN,	{PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* SATA:       11.0 */
	{OHCI1_DEVFN,	{PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI1:      12.0 */
	{EHCI1_DEVFN,	{PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}},	/* EHCI1:      12.2 */
	{OHCI2_DEVFN,	{PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI2:      13.0 */
	{EHCI2_DEVFN,	{PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}},	/* EHCI2:      13.2 */
	{SMBUS_DEVFN,	{PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* SMBUS:      14.0 */
	{IDE_DEVFN,		{PIRQ_NC, PIRQ_IDE, PIRQ_NC, PIRQ_NC}},		/* IDE:        14.1 */
	{HDA_DEVFN,		{PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* HDA:        14.2 */
	{SB_PCI_PORT_DEVFN, {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},		/* PCI:        14.4 */
	{OHCI4_DEVFN,	{PIRQ_NC, PIRQ_NC, PIRQ_OHCI4, PIRQ_NC}},	/* OHCI4:      14.5 */
	{SD_DEVFN,		{PIRQ_SD, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* SD:         14.7 */
	{SB_PCIE_PORT1_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* LAN:        15.0 */
	{SB_PCIE_PORT2_DEVFN,	{PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A}},	/* mPCIe J122: 15.1 */
	{SB_PCIE_PORT3_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* mPCIe J123: 15.2 */
	{SB_PCIE_PORT4_DEVFN,	{PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A}},	/* unused      15.3 */
	{OHCI3_DEVFN,	{PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI3:      16.0 (same device as xHCI 10.0) */
	{EHCI3_DEVFN,	{PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},	/* EHCI3:      16.2 (same device as xHCI 10.1) */
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = FCH_INT_TABLE_SIZE /* FIXME sizeof(mainboard_pirq_data) / sizeof(struct pirq_struct) */;
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

/*************************************************
 * enable the dedicated function in lamar board.
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
