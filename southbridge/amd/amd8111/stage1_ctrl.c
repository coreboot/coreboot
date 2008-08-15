/*
 * AMD 8111 "southbridge"
 * This file is part of the coreboot project.
 * Copyright (C) 2005-7 YingHai Lu
 * Copyright (C) 2005 Marc Jones, AMD
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "amd8111.h"


/* by yhlu 2005.10 */
/** 
 * Get the device fn for the 8111. 
 * @param bus the bus on which to search
 * @return The device number, in the range 0-31
 */
static u32 get_sbdn(unsigned bus)
{
        u32 dev;

        /* Find the device.
         * There can only be one 8111 on a hypertransport chain/bus.
         */

        pci_conf1_find_on_bus(bus, PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_PCI, &dev);

	/* this makes no sense. At all. I wonder if this is an ancient bug. >> 15? */
#warning shift right 15? makes no sense. 

        return (dev>>15) & 0x1f;

}

/** 
 * Get the device fn for the 8111. 
 * @param bus the bus on which to search
 * @return The device number, in the range 0-31
 */
static void enable_cf9_x(unsigned sbbusn, unsigned sbdn)
{
	u32 dev;
	u8 byte;

	dev = PCI_BDF(sbbusn, sbdn+1, 3); //ACPI
	/* enable cf9 */
	byte = pci_conf1_read_config8(dev, 0x41);
	byte |= (1<<6) | (1<<5);
	pci_conf1+write_config8(dev, 0x41, byte);
}

/** 
 * Enable "cf9". cf9 is a commonly used 8-bit IO address for reset, overlapping the 32-bit cf8 config address. 
 */
static void enable_cf9(void)
{
	u32 sblk = get_sblk();
	u32 sbbusn = get_sbbusn(sblk);
	u32 sbdn = get_sbdn(sbbusn);

	enable_cf9_x(sbbusn, sbdn);
}

/** 
 * Perform a hard reset. Set the "bios reset" indicator to be used by subsequent coreboot to know we 
 * came out of a coreboot-initiated reset. 
 * @return Never returns. 
 */
static void hard_reset(void)
{
        set_bios_reset();
        /* reset */
        enable_cf9();
        outb(0x0e, 0x0cf9); // make sure cf9 is enabled
}

/** 
 * Enable a FID change on the southbridge. 
 * @param sbbusn south bridge bus number
 * @param sbdn southbridge device numer 
 */
static void enable_fid_change_on_sb(u16 sbbusn, u16 sbdn)
{
	u32 dev;

	dev = PCI_BDF(sbbusn, sbdn+1, 3); // ACPI

	pci_conf1_write_config8(dev, 0x74, 4);

	/* set VFSMAF ( VID/FID System Management Action Field) to 2 */
	pci_conf1_write_config32(dev, 0x70, 2<<12);

}

/** 
 * Initiate a soft reset given a bus and device number. Indicate via set_bios_reset that we did so. 
 * @param sbbusn south bridge bus number
 * @param sbdn southbridge device numer 
 * @return never
 */
static void soft_reset_x(unsigned sbbusn, unsigned sbdn)
{
        u32 dev;

	dev = PCI_BDF(sbbusn, sbdn+1, 0); //ISA

        /* Reset */
        set_bios_reset();
        pci_conf1_write_config8(dev, 0x47, 1);

}

/** 
 * Initiate a soft reset by finding the southbridge and calling soft_reset_x
 * @return never
 */
static void soft_reset(void)
{

	unsigned sblk = get_sblk();
	unsigned sbbusn = get_sbbusn(sblk);
	unsigned sbdn = get_sbdn(sbbusn);

	return soft_reset_x(sbbusn, sbdn);

}
