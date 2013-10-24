/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <cbmem.h>
#include <console/console.h>
#include <rmodule.h>

#include <baytrail/ramstage.h>
#include <baytrail/efi_wrapper.h>

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

void baytrail_run_reference_code(void)
{
	int ret;
	efi_wrapper_entry_t entry;
	struct efi_wrapper_params wrp = {
		.version = EFI_WRAPPER_VER,
		.console_out = send_to_console,
	};
	struct rmod_stage_load refcode = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.name = "fallback/refcode",
	};

	if (rmodule_stage_load_from_cbfs(&refcode) || refcode.entry == NULL) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return;
	}

	entry = refcode.entry;

	/* Call into reference code. */
	ret = entry(&wrp);

	if (ret != 0) {
		printk(BIOS_DEBUG, "Reference code returned %d\n", ret);
		return;
	}
}

