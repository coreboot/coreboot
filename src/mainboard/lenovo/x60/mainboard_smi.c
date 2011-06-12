/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include "southbridge/intel/i82801gx/nvs.h"
#include <ec/acpi/ec.h>
#include <pc80/mc146818rtc.h>
#include <ec/lenovo/h8/h8.h>
#include "dock.h"
#include "smi.h"

/* The southbridge SMI handler checks whether gnvs has a
 * valid pointer before calling the trap handler
 */
extern global_nvs_t *gnvs;

static void mainboard_smm_init(void)
{
	printk(BIOS_DEBUG, "initializing SMI\n");
	/* Enable 0x1600/0x1600 register pair */
	ec_set_bit(0x00, 0x05);
}

static void mainboard_smi_save_cmos(void)
{
	u8 val;
	u8 tmp70, tmp72, tmpcf8;

	tmp70 = inb(0x70);
	tmp72 = inb(0x72);
	tmpcf8 = inl(0xcf8);

	val = pci_read_config8(PCI_DEV(0, 2, 1), 0xf4);
	set_option("tft_brightness", &val);
	val = ec_read(H8_VOLUME_CONTROL);
	set_option("volume", &val);

	outb(tmp70, 0x70);
	outb(tmp72, 0x72);
	outb(tmpcf8, 0xcf8);
}

int mainboard_io_trap_handler(int smif)
{
	static int smm_initialized;

	if (!smm_initialized) {
		mainboard_smm_init();
		smm_initialized = 1;
	}

	switch (smif) {
	case SMI_DOCK_CONNECT:
		dlpc_init();
		if (!dock_connect()) {
			/* set dock LED to indicate status */
			ec_write(0x0c, 0x88);
		} else {
			/* blink dock LED to indicate failure */
			ec_write(0x0c, 0xc8);
		}
		break;

	case SMI_DOCK_DISCONNECT:
		dock_disconnect();
		ec_write(0x0c, 0x08);
		break;

	case SMI_SAVE_CMOS:
		mainboard_smi_save_cmos();
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}
