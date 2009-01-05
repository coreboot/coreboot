/*
 * This file is part of the coreboot project.
 *
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <device/smbus.h>
#include <device/smbus_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/device.h>
#include <console.h>
#include <io.h>
#include "ck804.h"
#include "ck804_smbus.h"

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

int smbus_wait_until_ready(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f;
		if (val == 0)
			return 0;
		outb(val, smbus_io_base + SMBHSTSTAT);
	} while (--loops);
	return -2;
}

int smbus_wait_until_done(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(smbus_io_base + SMBHSTSTAT);
		if ((val & 0xff) != 0)
			return 0;
	} while (--loops);
	return -3;
}

int do_smbus_recv_byte(unsigned smbus_io_base, unsigned device)
{
	unsigned char global_status_register, byte;

#if 0
	/* Not needed, upon write to PRTCL, the status will be auto-cleared. */
	if (smbus_wait_until_ready(smbus_io_base) < 0)
		return -2;
#endif

	/* Set the device I'm talking to. */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* Set the command/address. */
	outb(0, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* Byte data recv */
	outb(0x05, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* Poll for transaction completion. */
	if (smbus_wait_until_done(smbus_io_base) < 0)
		return -3;

	/* Lose check */
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80;

	/* Read results of transaction. */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	/* Lose check, otherwise it should be 0. */
	if (global_status_register != 0x80)
		return -1;

	return byte;
}

int do_smbus_send_byte(unsigned smbus_io_base, unsigned device,
			      unsigned char val)
{
	unsigned global_status_register;

#if 0
	/* Not needed, upon write to PRTCL, the status will be auto-cleared. */
	if (smbus_wait_until_ready(smbus_io_base) < 0)
		return -2;
#endif

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* Set the device I'm talking to. */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	outb(0, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* Set up for a byte data write. */
	outb(0x04, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* Poll for transaction completion. */
	if (smbus_wait_until_done(smbus_io_base) < 0)
		return -3;

	/* Lose check */
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80;

	if (global_status_register != 0x80)
		return -1;

	return 0;
}

int do_smbus_read_byte(unsigned smbus_io_base, unsigned device,
			      unsigned address)
{
	unsigned char global_status_register, byte;

#if 0
	/* Not needed, upon write to PRTCL, the status will be auto-cleared. */
	if (smbus_wait_until_ready(smbus_io_base) < 0)
		return -2;
#endif

	/* Set the device I'm talking to. */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* Set the command/address. */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* Byte data read */
	outb(0x07, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* Poll for transaction completion. */
	if (smbus_wait_until_done(smbus_io_base) < 0)
		return -3;

	/* Lose check */
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80;

	/* Read results of transaction. */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	/* Lose check, otherwise it should be 0. */
	if (global_status_register != 0x80)
		return -1;

	return byte;
}

int do_smbus_write_byte(unsigned smbus_io_base, unsigned device,
			       unsigned address, unsigned char val)
{
	unsigned global_status_register;

#if 0
	/* Not needed, upon write to PRTCL, the status will be auto-cleared. */
	if (smbus_wait_until_ready(smbus_io_base) < 0)
		return -2;
#endif

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* Set the device I'm talking to. */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* Set up for a byte data write. */
	outb(0x06, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* Poll for transaction completion. */
	if (smbus_wait_until_done(smbus_io_base) < 0)
		return -3;

	/* Lose check */
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80;

	if (global_status_register != 0x80)
		return -1;

	return 0;
}

int lsmbus_recv_byte(struct device * dev)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_recv_byte(res->base, device);
}

int lsmbus_send_byte(struct device * dev, u8 val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_send_byte(res->base, device, val);
}

int lsmbus_read_byte(struct device * dev, u8 address)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_read_byte(res->base, device, address);
}

int lsmbus_write_byte(struct device * dev, u8 address, u8 val)
{
	unsigned device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x20 + (pbus->link * 4));

	return do_smbus_write_byte(res->base, device, address, val);
}

struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte  = lsmbus_recv_byte,
	.send_byte  = lsmbus_send_byte,
	.read_byte  = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

struct device_operations ck804_smbus_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			 .device = PCI_DEVICE_ID_NVIDIA_CK804_SM}}},
	.phase3_chip_setup_dev	 = ck804_enable,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.ops_pci		 = &ck804_ops_pci,
	.ops_smbus_bus		 = &lops_smbus_bus,
};

