/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "superiotool.h"

#define DEVICE_ID_VT82C686_REG	0xe0
#define DEVICE_REV_VT82C686_REG	0xe1

#define DEVICE_ID_VT1211_REG    0x20
#define DEVICE_REV_VT1211_REG   0x21

static const struct superio_registers reg_table[] = {
	{0x3c00, "VT82C686A/VT82C686B", {
		{EOT}}},
	{0x3c01, "VT1211", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2e,0x2f,EOT},
			{0x3c,0x01,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,EOT}},
		{0x0, "Floppy Disk Controller (FDC)",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,EOT},
			{0x00,0x03,0xf0,0x06,0x01,0x00,0x00,EOT}},
		{0x1, "Parallel Port (PP)",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x03,0x03,0x78,0x05,0x00,0x00,EOT}},
		{0x2, "Serial Port 1 (UART1)",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "Serial Port 2 (UART2)",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x00,EOT}},
		{0x6, "MIDI",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x03,0x30,0x00,EOT}},
		{0x7, "Game Port (GMP)",
			{0x30,0x60,0x61,EOT},
			{0x00,0x02,0x00,EOT}},
		{0x8, "GPIO",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0xe9,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0x9, "Watch Dog (WDG)",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0xea,0x00,0x00,0x00,EOT}},
		{0xa, "Wake-up Control (WUC)",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0xeb,0x00,0x00,EOT}},
		{0xb, "Hardware Monitor (HM)",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0xec,0x00,0x00,EOT}},
		{0xc, "Very Fast IR (VFIR)",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x00,0xe8,0x00,0x00,0x06,0x00,EOT}},
		{0xd, "Flash ROM (ROM)",
			{0x30,0xf0,EOT},
			{0x01,0x00,EOT}},
		{EOT}}},
	{EOT}
};

static uint8_t vt82c686_conf = 0;

static int enter_conf_mode_via_vt82c686(void)
{
	struct pci_dev *dev;

	dev = pci_dev_find(0x1106, 0x0686);
	if (!dev) {
		if (verbose)
			printf("  PCI device 1106:0686 not found.\n");
		return 1;
	}

	vt82c686_conf = pci_read_byte(dev, 0x85);
	if (verbose)
		printf("  Super I/O %sabled, Super I/O configuration %sabled\n",
		       (vt82c686_conf & (1 << 0)) ? "en" : "dis",
		       (vt82c686_conf & (1 << 1)) ? "en" : "dis");

	/* If the Super I/O is not enabled, skip it. */
	if (!(vt82c686_conf & (1 << 0)))
		return 1;

	/* Enable Super I/O configuration mode. */
	pci_write_byte(dev, 0x85, vt82c686_conf | (1 << 1));

	return 0;
}

static void exit_conf_mode_via_vt82c686(void)
{
	struct pci_dev *dev;

	dev = pci_dev_find(0x1106, 0x0686);
	if (!dev) {
		printf("Bug: PCI device 1106:0686 not found during shutdown.\n"
		       "Please report to coreboot@coreboot.org.\n");
		return;
	}

	/* Restore (disable?) Super I/O configuration mode setting. */
	pci_write_byte(dev, 0x85, vt82c686_conf);
}

void probe_idregs_via(uint16_t port)
{
	uint16_t id;
	uint8_t devid;
	uint8_t rev;

	if (port == 0x3f0) {
		probing_for("VIA", "(init=vt82c686) ", port);
		if (enter_conf_mode_via_vt82c686())
			return;

		devid = regval(port, DEVICE_ID_VT82C686_REG);
		rev = regval(port, DEVICE_REV_VT82C686_REG);
		id = devid << 8;

		if (superio_unknown(reg_table, id)) {
			if (verbose)
				printf(NOTFOUND "id=0x%02x, rev=0x%02x\n", devid, rev);
		} else {
			printf("Found VIA %s (id=0x%02x, rev=0x%02x) at 0x%x\n",
			       get_superio_name(reg_table, id), devid, rev, port);
			chip_found = 1;
		}
		exit_conf_mode_via_vt82c686();
		if (chip_found)
			return;
	} else {
		probing_for("VIA", "(init=0x87,0x87) ", port);
		enter_conf_mode_winbond_fintek_ite_8787(port);

	        devid = regval(port, DEVICE_ID_VT1211_REG);
		rev = regval(port, DEVICE_REV_VT1211_REG);
		id = (devid << 8) | 1;

		if (superio_unknown(reg_table, id)) {
			if (verbose)
				printf(NOTFOUND "id=0x%02x, rev=0x%02x\n", devid, rev);
		} else {
			printf("Found VIA %s (id=0x%02x, rev=0x%02x) at 0x%x\n",
			       get_superio_name(reg_table, id), devid, rev, port);
			chip_found = 1;
			dump_superio("VIA", reg_table, port, id, LDN_SEL);
		}
	}

	exit_conf_mode_winbond_fintek_ite_8787(port);
}

void print_via_chips(void)
{
	print_vendor_chips("VIA", reg_table);
}
