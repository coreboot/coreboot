/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2, or (at your
 * option) any later version, of the License.
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

#include <types.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/device.h>
#include <device/pnp.h>
#include <string.h>
#include "odm_oem/lenovo/lenovo.h"

const char tablet_numbers[][5] = {
	/* X60t. */
	"6363", "6364", "6365", "6366",
	"6367", "6368", "7762", "7763",
	"7764", "7767", "7768", "7769",
	/* X201t. */
	"0053", "0831", "2985", "3093",
	"3113", "3144", "3239", "4184",
	"7448", "7449", "7450", "7453",
	"2263", "2266",
};

int
drivers_lenovo_is_wacom_present(void)
{
#if IS_ENABLED(CONFIG_DIGITIZER_PRESENT)
	printk (BIOS_INFO, "Digitizer state forced as present\n");
	return 1;
#elif IS_ENABLED(CONFIG_DIGITIZER_ABSENT)
	printk (BIOS_INFO, "Digitizer state forced as absent\n");
	return 0;
#else
	const char *pn;
	int i;
	static int result = -1;
	device_t superio;

	if (result != -1)
		return result;
	superio = dev_find_slot_pnp (0x164e, 3);
	if (!superio) {
		printk (BIOS_INFO, "No Super I/O, skipping wacom\n");
		return (result = 0);
	}

	/* Probe ID. */
	if (pnp_read_config(superio, 0x20) != 0xf2) {
		printk (BIOS_INFO, "Super I/O probe failed, skipping wacom\n");
		return (result = 0);
	}

	pn = lenovo_mainboard_partnumber();
	if (!pn)
		return 0;
	printk (BIOS_DEBUG, "Lenovo P/N is %s\n", pn);
	for (i = 0; i < sizeof (tablet_numbers) / sizeof (tablet_numbers[0]);
	     i++)
		if (memcmp (tablet_numbers[i], pn, 4) == 0) {
			printk (BIOS_DEBUG, "Lenovo P/N %s is a tablet\n", pn);
			return (result = 1);
		}
	printk (BIOS_DEBUG, "Lenovo P/N %s is not a tablet\n", pn);
	return (result = 0);
#endif
}

void
drivers_lenovo_serial_ports_ssdt_generate(const char *scope,
					  int have_dock_serial)
{
	int scopelen, devicelen, reslen, methodlen;

	scopelen = acpigen_write_scope(scope);

	if (drivers_lenovo_is_wacom_present()) {
		/* Device op.  */
		scopelen += acpigen_emit_byte(0x5b);
		scopelen += acpigen_emit_byte(0x82);
		devicelen = acpigen_write_len_f();
		devicelen += acpigen_emit_namestring("DTR");

		devicelen += acpigen_write_name("_HID");
		devicelen += acpigen_emit_eisaid("WACF004");

		devicelen += acpigen_write_name("_CRS");

		reslen = acpigen_write_resourcetemplate_header();
		reslen += acpigen_write_io16(0x200, 0x200, 1, 8, 1);
		reslen += acpigen_write_irq(0x20);

		devicelen += reslen;
		devicelen += acpigen_write_resourcetemplate_footer(reslen);

		/* method op */
		devicelen += acpigen_emit_byte(0x14);
		methodlen = acpigen_write_len_f();
		methodlen += acpigen_emit_namestring("_STA");
		/* no fnarg */
		methodlen += acpigen_emit_byte(0x00);
		/* return */
		methodlen += acpigen_emit_byte(0xa4);
		methodlen += acpigen_write_byte(0xf);

		acpigen_patch_len(methodlen - 1);
		devicelen += methodlen;

		acpigen_patch_len(devicelen - 1);
		scopelen += devicelen;
	}

	if (have_infrared) {
		/* Device op. */
		scopelen += acpigen_emit_byte(0x5b);
		scopelen += acpigen_emit_byte(0x82);
		devicelen = acpigen_write_len_f();
		devicelen += acpigen_emit_namestring("FIR");

		devicelen += acpigen_write_name("_HID");
		devicelen += acpigen_emit_eisaid("IBM0071");
		devicelen += acpigen_write_name("_CID");
		devicelen += acpigen_emit_eisaid("PNP0511");
		devicelen += acpigen_write_name("_UID");

		/* One */
		methodlen += acpigen_write_byte(0x1);
		devicelen += acpigen_write_name("_CRS");

		reslen = acpigen_write_resourcetemplate_header();
		reslen += acpigen_write_io16(0x2f8, 0x2f8, 1, 8, 1);
		reslen += acpigen_write_irq(0x80);

		devicelen += reslen;
		devicelen += acpigen_write_resourcetemplate_footer(reslen);

		/* method op */
		devicelen += acpigen_emit_byte(0x14);
		methodlen = acpigen_write_len_f();
		methodlen += acpigen_emit_namestring("_STA");
		/* no fnarg */
		methodlen += acpigen_emit_byte(0x00);
		/* return */
		methodlen += acpigen_emit_byte(0xa4);
		methodlen += acpigen_write_byte(0xf);
		acpigen_patch_len(methodlen - 1);

		devicelen += methodlen;

		acpigen_patch_len(devicelen - 1);
		scopelen += devicelen;
	}

	if (have_dock_serial) {
		/* Device op.  */
		scopelen += acpigen_emit_byte(0x5b);
		scopelen += acpigen_emit_byte(0x82);
		devicelen = acpigen_write_len_f();
		devicelen += acpigen_emit_namestring("COMA");

		devicelen += acpigen_write_name("_HID");
		devicelen += acpigen_emit_eisaid("PNP0511");
		devicelen += acpigen_write_name("_UID");
		/* Byte */
		methodlen += acpigen_write_byte(0xa);
		methodlen += acpigen_write_byte(0x2);

		devicelen += acpigen_write_name("_CRS");

		reslen = acpigen_write_resourcetemplate_header();
		reslen += acpigen_write_io16(0x3f8, 0x3f8, 1, 8, 1);
		reslen += acpigen_write_irq(0x10);

		devicelen += reslen;
		devicelen += acpigen_write_resourcetemplate_footer(reslen);

		/* method op */
		devicelen += acpigen_emit_byte(0x14);
		methodlen = acpigen_write_len_f();
		methodlen += acpigen_emit_namestring("_STA");
		/* no fnarg */
		methodlen += acpigen_emit_byte(0x00);
		/* return */
		methodlen += acpigen_emit_byte(0xa4);
		methodlen += acpigen_write_byte(0xf);
		acpigen_patch_len(methodlen - 1);

		devicelen += methodlen;

		acpigen_patch_len(devicelen - 1);
		scopelen += devicelen;
	}

	acpigen_patch_len(scopelen - 1);
}
