/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <fmap.h>
#include <types.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/pch.h>
#include "onboard.h"

static unsigned int search(char *p, u8 *a, unsigned int lengthp,
			   unsigned int lengtha)
{
	int i, j;

	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++)
			;
		if (i >= lengthp)
			return j;
	}
	return lengtha;
}

static unsigned char get_hex_digit(u8 *offset)
{
	unsigned char retval = 0;

	retval = *offset - '0';
	if (retval > 0x09) {
		retval = *offset - 'A' + 0x0A;
		if (retval > 0x0F)
			retval = *offset - 'a' + 0x0a;
	}
	if (retval > 0x0F) {
		printk(BIOS_ERR, "Invalid Hex digit found: %c - 0x%02x\n", *offset, *offset);
		retval = 0;
	}

	return retval;
}

static int get_mac_address(u32 *high_dword, u32 *low_dword,
			   u8 *search_address, u32 search_length)
{
	char key[] = "ethernet_mac";
	unsigned int offset;
	int i;

	offset = search(key, search_address, sizeof(key) - 1, search_length);
	if (offset == search_length) {
		printk(BIOS_ERR, "Could not locate '%s' in VPD\n", key);
		return 0;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

	offset += sizeof(key);	/* move to next character */
	*high_dword = 0;

	/* Fetch the MAC address and put the octets in the correct order to
	 * be programmed.
	 *
	 * From RTL8105E_Series_EEPROM-Less_App_Note_1.1
	 * If the MAC address is 001122334455h:
	 * Write 33221100h to I/O register offset 0x00 via double word access
	 * Write 00005544h to I/O register offset 0x04 via double word access
	 */

	for (i = 0; i < 4; i++) {
		*high_dword |= (get_hex_digit(search_address + offset)
				<< (4 + (i * 8)));
		*high_dword |= (get_hex_digit(search_address + offset + 1)
				<< (i * 8));
		offset += 3;
	}

	*low_dword = 0;
	for (i = 0; i < 2; i++) {
		*low_dword |= (get_hex_digit(search_address + offset)
			       << (4 + (i * 8)));
		*low_dword |= (get_hex_digit(search_address + offset + 1)
			       << (i * 8));
		offset += 3;
	}

	return *high_dword | *low_dword;
}

static void program_mac_address(u16 io_base)
{
	void *search_address = NULL;
	size_t search_length = -1;

	/* Default MAC Address of A0:00:BA:D0:0B:AD */
	u32 high_dword = 0xD0BA00A0;	/* high dword of mac address */
	u32 low_dword = 0x0000AD0B;	/* low word of mac address as a dword */

	if (CONFIG(VPD)) {
		struct region_device rdev;

		if (fmap_locate_area_as_rdev("RO_VPD", &rdev) == 0) {
			search_address = rdev_mmap_full(&rdev);

			if (search_address != NULL)
				search_length = region_device_sz(&rdev);
		}
	} else {
		search_address = cbfs_map("vpd.bin", &search_length);
	}

	if (search_address == NULL)
		printk(BIOS_ERR, "LAN: VPD not found.\n");
	else
		get_mac_address(&high_dword, &low_dword, search_address,
				search_length);

	if (io_base) {
		printk(BIOS_DEBUG, "Realtek NIC io_base = 0x%04x\n", io_base);
		printk(BIOS_DEBUG, "Programming MAC Address\n");

		/* Disable register protection */
		outb(0xc0, io_base + 0x50);
		outl(high_dword, io_base);
		outl(low_dword, io_base + 0x04);
		outb(0x60, io_base + 54);
		/* Enable register protection again */
		outb(0x00, io_base + 0x50);
	}
}

void lan_init(void)
{
	u16 io_base = 0;
	struct device *ethernet_dev = NULL;

	/* Get NIC's IO base address */
	ethernet_dev = dev_find_device(JECHT_NIC_VENDOR_ID,
				       JECHT_NIC_DEVICE_ID, 0);
	if (ethernet_dev != NULL) {
		io_base = pci_read_config16(ethernet_dev, 0x10) & 0xfffe;

		/*
		 * Battery life time - LAN PCIe should enter ASPM L1 to save
		 * power when LAN connection is idle.
		 * enable CLKREQ: LAN pci config space 0x81h=01
		 */
		pci_write_config8(ethernet_dev, 0x81, 0x01);
	}

	if (io_base) {
		/* Program MAC address based on VPD data */
		program_mac_address(io_base);

		/*
		 * Program NIC LEDS
		 *
		 * RTL8105E Series EEPROM-Less Application Note,
		 * Section 5.6 LED Mode Configuration
		 *
		 * Step1: Write C0h to I/O register 0x50 via byte access to
		 *        disable 'register protection'
		 * Step2: Write xx001111b to I/O register 0x52 via byte access
		 *        (bit7 is LEDS1 and bit6 is LEDS0)
		 * Step3: Write 0x00 to I/O register 0x50 via byte access to
		 *        enable 'register protection'
		 */
		outb(0xc0, io_base + 0x50);	/* Disable protection */
		outb((JECHT_NIC_LED_MODE << 6) | 0x0f, io_base + 0x52);
		outb(0x00, io_base + 0x50);	/* Enable register protection */
	}
}
