/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <device/pci_ops.h>

#define D0F0_PCIEXBAR_LO 0x60

static void mainboard_machine_check(void)
{
	/* Check that MCFG is active. If it's not qemu was started for machine PC */
	if (!CONFIG(BOOTBLOCK_CONSOLE) &&
	    (pci_read_config32(PCI_DEV(0, 0, 0), D0F0_PCIEXBAR_LO) !=
	     (CONFIG_MMCONF_BASE_ADDRESS | 1)))
		die("You must run qemu for machine Q35 (-M q35)");
}

void mainboard_romstage_entry(void)
{
	i82801ix_early_init();

	mainboard_machine_check();

	cbmem_recovery(0);
}
