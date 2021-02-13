/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <southbridge/amd/agesa/hudson/pci_devs.h>
#include <southbridge/amd/agesa/hudson/amd_pci_int_defs.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <northbridge/amd/agesa/family16kb/pci_devs.h>

static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	/* INTA# - INTH# */
	[0x00] = 0x03,0x04,0x05,0x07,0x1F,0x1F,0x1F,0x1F,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x5A,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerMon, SD */
	[0x10] = 0x1F,0x1F,0x1F,0x03,0x1F,0x1F,0x1F,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/22 INTA-B */
	[0x30] = 0x05,0x04,0x05,0x04,0x05,0x04,0x1F,0x1F,
	/* RSVD, SATA */
	[0x40] = 0x1F, 0x07
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	/* INTA# - INTH# */
	[0x00] = 0x10,0x11,0x12,0x13,0x1F,0x1F,0x1F,0x1F,
	/* Misc-nil,0,1,2, INT from Serial irq */
	[0x08] = 0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerMon, SD */
	[0x10] = 0x09,0x1F,0x1F,0x10,0x1F,0x1F,0x1F,0x1F,
	/* IMC INT0 - 5 */
	[0x20] = 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
	/* USB Devs 18/19/22 INTA-B */
	[0x30] = 0x12,0x11,0x12,0x11,0x12,0x11,0x1F,0x1F,
	/* RSVD, SATA */
	[0x40] = 0x1F, 0x13
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
	/* {PCI_devfn,	{PIN A, PIN B, PIN C, PIN D}}, */
	{GFX_DEVFN,	{PIRQ_A, PIRQ_NC, PIRQ_NC, PIRQ_NC}},			/* VGA:		01.0 */
	{ACTL_DEVFN,{PIRQ_NC, PIRQ_B, PIRQ_NC, PIRQ_NC}},			/* Audio:	01.1 */
	{NB_PCIE_PORT1_DEVFN,	{PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},	/* x4 PCIe:	02.1 */
	{NB_PCIE_PORT5_DEVFN,	{PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A}},	/* Edge:	02.5 */
	{XHCI_DEVFN,	{PIRQ_C, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* XHCI:	10.0 */
	{SATA_DEVFN,	{PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* SATA:	11.0 */
	{OHCI1_DEVFN,	{PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI1:	12.0 */
	{EHCI1_DEVFN,	{PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}},	/* EHCI1:	12.2 */
	{OHCI2_DEVFN,	{PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI2:	13.0 */
	{EHCI2_DEVFN,	{PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}},	/* EHCI2:	13.2 */
	{OHCI3_DEVFN,	{PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	/* OHCI3:	16.0 */
	{EHCI3_DEVFN,	{PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},	/* EHCI3:	16.2 */
	{HDA_DEVFN,		{PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},		/* HDA:		14.2 */
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

/**********************************************
 * enable the dedicated function in mainboard.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
