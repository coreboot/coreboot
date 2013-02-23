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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/pci_ids.h>
#include "vx800.h"

#define SMBUS_IO_BASE		0x0500	//from award bios
#define PMIO_BASE		VX800_ACPI_IO_BASE	//might as well set this while we're here

#define SMBHSTSTAT		SMBUS_IO_BASE + 0x0
#define SMBSLVSTAT		SMBUS_IO_BASE + 0x1
#define SMBHSTCTL		SMBUS_IO_BASE + 0x2
#define SMBHSTCMD		SMBUS_IO_BASE + 0x3
#define SMBXMITADD		SMBUS_IO_BASE + 0x4
#define SMBHSTDAT0		SMBUS_IO_BASE + 0x5
#define SMBHSTDAT1		SMBUS_IO_BASE + 0x6
/* Rest of these aren't currently used... */
#define SMBBLKDAT		SMBUS_IO_BASE + 0x7
#define SMBSLVCTL		SMBUS_IO_BASE + 0x8
#define SMBTRNSADD		SMBUS_IO_BASE + 0x9
#define SMBSLVDATA 		SMBUS_IO_BASE + 0xa
#define SMLINK_PIN_CTL		SMBUS_IO_BASE + 0xe
#define SMBUS_PIN_CTL		SMBUS_IO_BASE + 0xf

/* Define register settings */
#define HOST_RESET	0xff
#define READ_CMD		0x01	// 1 in the 0 bit of SMBHSTADD states to READ

#define SMBUS_TIMEOUT		(100*1000*10)

#define I2C_TRANS_CMD		0x40
#define CLOCK_SLAVE_ADDRESS	0x69

#define SMBUS_DELAY()		outb(0x80, 0x80)

#if CONFIG_DEBUG_SMBUS
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX16(x)
#endif

/* Internal functions */
static void smbus_print_error(unsigned char host_status_register, int loops)
{
//              print_err("some i2c error\n");
	/* Check if there actually was an error */
	if (host_status_register == 0x00 || host_status_register == 0x40 ||
	    host_status_register == 0x42)
		return;
	print_err("smbus_error: ");
	print_err_hex8(host_status_register);
	print_err("\n");
	if (loops >= SMBUS_TIMEOUT) {
		print_err("SMBus Timout\n");
	}
	if (host_status_register & (1 << 4)) {
		print_err("Interrup/SMI# was Failed Bus Transaction\n");
	}
	if (host_status_register & (1 << 3)) {
		print_err("Bus Error\n");
	}
	if (host_status_register & (1 << 2)) {
		print_err("Device Error\n");
	}
	if (host_status_register & (1 << 1)) {
		/* This isn't a real error... */
		print_debug("Interrupt/SMI# was Successful Completion\n");
	}
	if (host_status_register & (1 << 0)) {
		print_err("Host Busy\n");
	}
}

static void smbus_wait_until_ready(void)
{
	int loops;

	loops = 0;
	/* Yes, this is a mess, but it's the easiest way to do it */
	while (((inb(SMBHSTSTAT) & 1) == 1) && (loops <= SMBUS_TIMEOUT)) {
		SMBUS_DELAY();
		++loops;
	}
	smbus_print_error(inb(SMBHSTSTAT), loops);
}

static void smbus_reset(void)
{
	outb(HOST_RESET, SMBHSTSTAT);
}

/* Public functions */

static unsigned int get_spd_data(unsigned int dimm, unsigned int offset)
{
	unsigned int val;

	smbus_reset();
	/* clear host data port */
	outb(0x00, SMBHSTDAT0);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	/* Do some mathmatic magic */
	dimm = (DIMM0 + dimm) << 1;

	outb(dimm | 0x1, SMBXMITADD);
	outb(offset, SMBHSTCMD);
	outb(0x48, SMBHSTCTL);

	SMBUS_DELAY();

	smbus_wait_until_ready();

	val = inb(SMBHSTDAT0);
	smbus_reset();
	return val;
}

void enable_smbus(void)
{
	device_t dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX855_LPC), 0);

	if (dev == PCI_DEV_INVALID) {
		/* This won't display text if enable_smbus() is before serial init */
		die("Power Managment Controller not found\n");
	}

	/* Set clock source */
	pci_write_config8(dev, 0x94, 0x20);

	/* Write SMBus IO base to 0xd0, and enable SMBus */
	pci_write_config16(dev, 0xd0, SMBUS_IO_BASE | 1);

	/* Set to Award value */
	pci_write_config8(dev, 0xd2, 0x05);

	/* Make it work for I/O ... */
	pci_write_config16(dev, 0x04, 0x0003);

	smbus_reset();
	/* clear host data port */
	outb(0x00, SMBHSTDAT0);
	SMBUS_DELAY();
	smbus_wait_until_ready();
}

/**
 * A fixup for some systems that need time for the SMBus to "warm up". This is
 * needed on some VT823x based systems, where the SMBus spurts out bad data for
 * a short time after power on. This has been seen on the VIA Epia series and
 * Jetway J7F2-series. It reads the ID byte from SMBus, looking for
 * known-good data from a slot/address. Exits on either good data or a timeout.
 *
 * TODO: This should probably go into some global file, but one would need to
 *       be created just for it. If some other chip needs/wants it, we can
 *       worry about it then.
 *
 * @param mem_ctrl The memory controller and SMBus addresses.
 */
void smbus_fixup(const struct mem_controller *mem_ctrl)
{
	int i, ram_slots, current_slot = 0;
	u8 result = 0;

	ram_slots = ARRAY_SIZE(mem_ctrl->channel0);
	if (!ram_slots) {
		print_err("smbus_fixup() thinks there are no RAM slots!\n");
		return;
	}

	PRINT_DEBUG("Waiting for SMBus to warm up");

	/*
	 * Bad SPD data should be either 0 or 0xff, but YMMV. So we look for
	 * the ID bytes of SDRAM, DDR, DDR2, and DDR3 (and anything in between).
	 * VT8237R has only been seen on DDR and DDR2 based systems, so far.
	 */
	for (i = 0; (i < SMBUS_TIMEOUT && ((result < SPD_MEMORY_TYPE_SDRAM) ||
					   (result >
					    SPD_MEMORY_TYPE_SDRAM_DDR3)));
	     i++) {

		if (current_slot > ram_slots)
			current_slot = 0;

		result = get_spd_data(mem_ctrl->channel0[current_slot],
				      SPD_MEMORY_TYPE);
		current_slot++;
		PRINT_DEBUG(".");
	}

	if (i >= SMBUS_TIMEOUT)
		print_err("SMBus timed out while warming up\n");
	else
		PRINT_DEBUG("Done\n");
}

/* Debugging Function */
#if CONFIG_DEBUG_SMBUS
static void dump_spd_data(void)
{
	int dimm, offset, regs;
	unsigned int val;

	for (dimm = 0; dimm < 8; dimm++) {
		print_debug("SPD Data for DIMM ");
		print_debug_hex8(dimm);
		print_debug("\n");

		val = get_spd_data(dimm, 0);
		if (val == 0xff) {
			regs = 256;
		} else if (val == 0x80) {
			regs = 128;
		} else {
			print_debug("No DIMM present\n");
			regs = 0;
		}
		for (offset = 0; offset < regs; offset++) {
			print_debug("  Offset ");
			print_debug_hex8(offset);
			print_debug(" = 0x");
			print_debug_hex8(get_spd_data(dimm, offset));
			print_debug("\n");
		}
	}
}
#else
#define dump_spd_data()
#endif
