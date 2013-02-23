/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "vt8237r.h"
#include "chip.h"

/*
 * Datasheet: http://www.via.com.tw/en/downloads/datasheets/chipsets/
 *		VT8237R_SouthBridge_Revision2.06_Lead-Free.zip
 */

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 7
void writeback(struct device *dev, u16 where, u8 what)
{
	u8 regval;

	pci_write_config8(dev, where, what);
	regval = pci_read_config8(dev, where);

	if (regval != what) {
		print_debug("Writeback to ");
		print_debug_hex8(where);
		print_debug(" failed ");
		print_debug_hex8(regval);
		print_debug("\n");
	}
}
#else
void writeback(struct device *dev, u16 where, u8 what)
{
	pci_write_config8(dev, where, what);
}
#endif

void dump_south(device_t dev)
{
	int i, j;

	for (i = 0; i < 256; i += 16) {
		printk(BIOS_DEBUG, "%02x: ", i);
		for (j = 0; j < 16; j++)
			printk(BIOS_DEBUG, "%02x ", pci_read_config8(dev, i + j));
		printk(BIOS_DEBUG, "\n");
	}
}

static void vt8237r_enable(struct device *dev)
{
	u16 vid, did;
	struct southbridge_via_vt8237r_config *sb =
	    (struct southbridge_via_vt8237r_config *)dev->chip_info;

	if (dev->path.type == DEVICE_PATH_PCI) {
		vid = pci_read_config16(dev, PCI_VENDOR_ID);
		did = pci_read_config16(dev, PCI_DEVICE_ID);
		if (vid == PCI_VENDOR_ID_VIA &&
			(did == PCI_DEVICE_ID_VIA_VT8237R_LPC ||
			 did == PCI_DEVICE_ID_VIA_VT8237A_LPC ||
			 did == PCI_DEVICE_ID_VIA_VT8237S_LPC)) {
			pci_write_config8(dev, 0x50, sb->fn_ctrl_lo);
			pci_write_config8(dev, 0x51, sb->fn_ctrl_hi);
		}
	}

	/* TODO: If SATA is disabled, move IDE to fn0 to conform PCI specs. */
}

struct chip_operations southbridge_via_vt8237r_ops = {
	CHIP_NAME("VIA VT8237R Southbridge")
	.enable_dev = vt8237r_enable,
};
