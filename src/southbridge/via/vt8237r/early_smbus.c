/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <spd.h>
#include <stdlib.h>
#include "vt8237r.h"

/**
 * Print an error, should it occur. If no error, just exit.
 *
 * @param host_status The data returned on the host status register after
 *		      a transaction is processed.
 * @param loops The number of times a transaction was attempted.
 */
static void smbus_print_error(u8 host_status, int loops)
{
	/* Check if there actually was an error. */
	if ((host_status == 0x00 || host_status == 0x40 ||
	     host_status == 0x42) && (loops < SMBUS_TIMEOUT))
		return;

	if (loops >= SMBUS_TIMEOUT)
		print_err("SMBus timeout\n");
	if (host_status & (1 << 4))
		print_err("Interrupt/SMI# was Failed Bus Transaction\n");
	if (host_status & (1 << 3))
		print_err("Bus error\n");
	if (host_status & (1 << 2))
		print_err("Device error\n");
	if (host_status & (1 << 1))
		print_debug("Interrupt/SMI# completed successfully\n");
	if (host_status & (1 << 0))
		print_err("Host busy\n");
}

/**
 * Wait for the SMBus to become ready to process the next transaction.
 */
static void smbus_wait_until_ready(void)
{
	int loops;

	PRINT_DEBUG("Waiting until SMBus ready\n");

	/* Loop up to SMBUS_TIMEOUT times, waiting for bit 0 of the
	 * SMBus Host Status register to go to 0, indicating the operation
	 * was completed successfully. I don't remember why I did it this way,
	 * but I think it was because ROMCC was running low on registers */
	loops = 0;
	while ((inb(SMBHSTSTAT) & 1) == 1 && loops < SMBUS_TIMEOUT)
		++loops;

	smbus_print_error(inb(SMBHSTSTAT), loops);
}

/**
 * Reset and take ownership of the SMBus.
 */
static void smbus_reset(void)
{
	outb(HOST_RESET, SMBHSTSTAT);

	/* Datasheet says we have to read it to take ownership of SMBus. */
	inb(SMBHSTSTAT);

	PRINT_DEBUG("After reset status: ");
	PRINT_DEBUG_HEX16(inb(SMBHSTSTAT));
	PRINT_DEBUG("\n");
}

static u8 smbus_transaction(u8 dimm, u8 offset, u8 data, int do_read)
{
	u8 val;

	PRINT_DEBUG(do_read ? "RD: " : "WR: ");
	PRINT_DEBUG("ADDR ");
	PRINT_DEBUG_HEX16(dimm);
	PRINT_DEBUG(" OFFSET ");
	PRINT_DEBUG_HEX16(offset);
	PRINT_DEBUG("\n");

	smbus_reset();

	/* Clear host data port. */
	outb(do_read ? 0x00 : data, SMBHSTDAT0);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	/* Actual addr to reg format. */
	dimm = (dimm << 1);
	dimm |= !!do_read;
	outb(dimm, SMBXMITADD);
	outb(offset, SMBHSTCMD);

	/* Start transaction, byte data read. */
	outb(0x48, SMBHSTCTL);
	SMBUS_DELAY();
	smbus_wait_until_ready();

	val = inb(SMBHSTDAT0);
	PRINT_DEBUG("Read: ");
	PRINT_DEBUG_HEX16(val);
	PRINT_DEBUG("\n");

	/* Probably don't have to do this, but it can't hurt. */
	smbus_reset();

	return val;
}

/**
 * Read a byte from the SMBus.
 *
 * @param dimm The address location of the DIMM on the SMBus.
 * @param offset The offset the data is located at.
 */
u8 smbus_read_byte(u8 dimm, u8 offset)
{
	return smbus_transaction(dimm, offset, 0, 1);
}

void smbus_write_byte(u8 dimm, u8 offset, u8 data)
{
	smbus_transaction(dimm, offset, data, 0);
}

#define PSONREADY_TIMEOUT 0x7fffffff

static device_t get_vt8237_lpc(void)
{
	device_t dev;

	/* Power management controller */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VT8237R_LPC), 0);
	if (dev != PCI_DEV_INVALID)
		return dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237S_LPC), 0);
	if (dev != PCI_DEV_INVALID)
		return dev;

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237A_LPC), 0);
	return dev;
}

/**
 * Enable the SMBus on VT8237R-based systems.
 */
void enable_smbus(void)
{
	device_t dev;
	int loops;

	/* Power management controller */
	dev = get_vt8237_lpc();
	if (dev == PCI_DEV_INVALID)
		die("Power management controller not found\n");

	/* Make sure the RTC power well is up before touching smbus. */
	loops = 0;
	while (!(pci_read_config8(dev, VT8237R_PSON) & (1<<6))
	       && loops < PSONREADY_TIMEOUT)
		++loops;

	/*
	 * 7 = SMBus Clock from RTC 32.768KHz
	 * 5 = Internal PLL reset from susp
	 */
	pci_write_config8(dev, VT8237R_POWER_WELL, 0xa0);

	/* Enable SMBus. */
	pci_write_config16(dev, VT8237R_SMBUS_IO_BASE_REG,
			   VT8237R_SMBUS_IO_BASE | 0x1);

	/* SMBus Host Configuration, enable. */
	pci_write_config8(dev, VT8237R_SMBUS_HOST_CONF, 0x01);

	/* Make it work for I/O. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);

	smbus_reset();

	/* Reset the internal pointer. */
	inb(SMBHSTCTL);
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
 * @param ctrl The memory controller and SMBus addresses.
 */
void smbus_fixup(const struct mem_controller *ctrl)
{
	int i, ram_slots, current_slot = 0;
	u8 result = 0;

	ram_slots = ARRAY_SIZE(ctrl->channel0);
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
	   (result > SPD_MEMORY_TYPE_SDRAM_DDR3))); i++) {

		if (current_slot > ram_slots)
			current_slot = 0;

		result = smbus_read_byte(ctrl->channel0[current_slot],
					 SPD_MEMORY_TYPE);
		current_slot++;
		PRINT_DEBUG(".");
	}

	if (i >= SMBUS_TIMEOUT)
		print_err("SMBus timed out while warming up\n");
	else
		PRINT_DEBUG("Done\n");
}

/* FIXME: Better separate the NB and SB, will be done once it works. */

void vt8237_sb_enable_fid_vid(void)
{
	device_t dev, devctl;
	u16 devid;

	/* Power management controller */
	dev = get_vt8237_lpc();
	if (dev == PCI_DEV_INVALID)
		return;

	devid = pci_read_config16(dev, PCI_DEVICE_ID);

	/* generic setup */

	/* Set ACPI base address to I/O VT8237R_ACPI_IO_BASE. */
	pci_write_config16(dev, 0x88, VT8237R_ACPI_IO_BASE | 0x1);

	/* Enable ACPI accessm RTC signal gated with PSON. */
	pci_write_config8(dev, 0x81, 0x84);

	/* chipset-specific parts */

	/* VLINK: FIXME: can we drop the devid check and just look for the VLINK device? */
	if (devid == PCI_DEVICE_ID_VIA_VT8237S_LPC ||
	    devid == PCI_DEVICE_ID_VIA_VT8237A_LPC) {
		devctl = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
					   PCI_DEVICE_ID_VIA_VT8237_VLINK), 0);

		if (devctl != PCI_DEV_INVALID) {
			/* So the chip knows we are on AMD. */
			pci_write_config8(devctl, 0x7c, 0x7f);
		}
	}

	if (devid == PCI_DEVICE_ID_VIA_VT8237S_LPC) {
		/*
		 * Allow SLP# signal to assert LDTSTOP_L.
		 * Will work for C3 and for FID/VID change.
		 */

		outb(0xff, VT8237R_ACPI_IO_BASE + 0x50);

		/* Reduce further the STPCLK/LDTSTP signal to 5us. */
		pci_write_config8(dev, 0xec, 0x4);

		return;
	}

	/* VT8237R and VT8237A */

	/*
	 * Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, VT8237R_ACPI_IO_BASE + 0x11);
}

void enable_rom_decode(void)
{
	device_t dev;

	/* Power management controller */
	dev = get_vt8237_lpc();
	if (dev == PCI_DEV_INVALID)
		return;

	/* ROM decode last 1MB FFC00000 - FFFFFFFF. */
	pci_write_config8(dev, 0x41, 0x7f);
}

#if CONFIG_HAVE_ACPI_RESUME
static int acpi_is_wakeup_early(void) {
	device_t dev;
	u16 tmp;

	print_debug("IN TEST WAKEUP\n");

	/* Power management controller */
	dev = get_vt8237_lpc();
	if (dev == PCI_DEV_INVALID)
		die("Power management controller not found\n");

	/* Set ACPI base address to I/O VT8237R_ACPI_IO_BASE. */
	pci_write_config16(dev, 0x88, VT8237R_ACPI_IO_BASE | 0x1);

	/* Enable ACPI accessm RTC signal gated with PSON. */
	pci_write_config8(dev, 0x81, 0x84);

	tmp = inw(VT8237R_ACPI_IO_BASE + 0x04);

	print_debug_hex8(tmp);
	return ((tmp & (7 << 10)) >> 10) == 1 ? 3 : 0 ;
}
#endif

#if defined(__GNUC__)
void vt8237_early_spi_init(void)
{
	device_t dev;
	volatile u16 *spireg;
	u32 tmp;

	/* Bus Control and Power Management */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VT8237S_LPC), 0);

	if (dev == PCI_DEV_INVALID)
		die("SB not found\n");

	/* Put SPI base 20 d0 fe. */
	tmp = pci_read_config32(dev, 0xbc);
	pci_write_config32(dev, 0xbc,
			   (VT8237S_SPI_MEM_BASE >> 8) | (tmp & 0xFF000000));

	/* Set SPI clock to 33MHz. */
	spireg = (u16 *) (VT8237S_SPI_MEM_BASE + 0x6c);
	(*spireg) &= 0xff00;
}
#endif

/* This #if is special. ROMCC chokes on the (rom == NULL) comparison.
 * Since the whole function is only called for one target and that target
 * is compiled with GCC, hide the function from ROMCC and be happy.
 */
#if defined(__GNUC__)
/*
 * Offset 0x58:
 * 31:20 	reserved
 * 19:16	4 bit position in shadow EEPROM
 * 15:0		data to write
 *
 * Offset 0x5c:
 * 31:28	reserved
 * 27 		ERDBG - enable read from 0x5c
 * 26		reserved
 * 25		SEELD
 * 24		SEEPR - write 1 when done updating, wait until SEELD is
 * 			set to 1, sticky
 *		cleared by reset, if it is 1 writing is disabled
 * 19:16	4 bit position in shadow EEPROM
 * 15:0		data from shadow EEPROM
 *
 * After PCIRESET SEELD and SEEPR must be 1 and 1.
 */

/* 1 = needs PCI reset, 0 don't reset, network initialized. */

/* FIXME: Maybe close the debug register after use? */

#define LAN_TIMEOUT 0x7FFFFFFF

int vt8237_early_network_init(struct vt8237_network_rom *rom)
{
	struct vt8237_network_rom n;
	int i, loops;
	device_t dev;
	u32 tmp;
	u8 status;
	u16 *rom_write;
	unsigned int checksum;

	/* Network adapter */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_8233_7), 0);
	if (dev == PCI_DEV_INVALID) {
		print_err("Network is disabled, please enable\n");
		return 0;
	}

	tmp = pci_read_config32(dev, 0x5c);
	tmp |= 0x08000000;	/* Enable ERDBG. */
	pci_write_config32(dev, 0x5c, tmp);

	status = ((pci_read_config32(dev, 0x5c) >> 24) & 0x3);

	/* Network controller OK, EEPROM loaded. */
	if (status == 3)
		return 0;

	if (rom == NULL) {
		print_err("No config data specified, using default MAC!\n");
		n.mac_address[0] = 0x0;
		n.mac_address[1] = 0x0;
		n.mac_address[2] = 0xde;
		n.mac_address[3] = 0xad;
		n.mac_address[4] = 0xbe;
		n.mac_address[5] = 0xef;
		n.phy_addr = 0x1;
		n.res1 = 0x0;
		n.sub_sid = 0x102;
		n.sub_vid = 0x1106;
		n.pid = 0x3065;
		n.vid = 0x1106;
		n.pmcc = 0x1f;
		n.data_sel = 0x10;
		n.pmu_data_reg = 0x0;
		n.aux_curr = 0x0;
		n.reserved = 0x0;
		n.min_gnt = 0x3;
		n.max_lat = 0x8;
		n.bcr0 = 0x9;
		n.bcr1 = 0xe;
		n.cfg_a = 0x3;
		n.cfg_b = 0x0;
		n.cfg_c = 0x40;
		n.cfg_d = 0x82;
		n.checksum = 0x0;
		rom = &n;
	}

	rom_write = (u16 *) rom;
	checksum = 0;
	/* Write all data except checksum and second to last byte. */
	tmp &= 0xff000000;	/* Leave reserved bits in. */
	for (i = 0; i < 15; i++) {
		pci_write_config32(dev, 0x58, tmp | (i << 16) | rom_write[i]);
		/* Lame code FIXME */
		checksum += rom_write[i] & 0xff;
		/* checksum %= 256; */
		checksum += (rom_write[i] >> 8) & 0xff;
		/* checksum %= 256; */
	}

	checksum += (rom_write[15] & 0xff);
	checksum = ~(checksum & 0xff);
	tmp |= (((checksum & 0xff) << 8) | rom_write[15]);

	/* Write last byte and checksum. */
	pci_write_config32(dev, 0x58, (15 << 16) | tmp);

	tmp = pci_read_config32(dev, 0x5c);
	pci_write_config32(dev, 0x5c, tmp | 0x01000000); /* Toggle SEEPR. */

	/* Yes, this is a mess, but it's the easiest way to do it. */
	/* XXX not so messy, but an explanation of the hack would have been better */
	loops = 0;
	while ((((pci_read_config32(dev, 0x5c) >> 25) & 1) == 0)
	       && (loops < LAN_TIMEOUT)) {
		++loops;
	}

	if (loops >= LAN_TIMEOUT) {
		print_err("Timeout - LAN controller didn't accept config\n");
		return 0;
	}

	/* We are done, config will be used after PCIRST#. */
	return 1;
}
#endif
