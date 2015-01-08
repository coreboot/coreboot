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
#include <console/console.h>
#include <cpu/x86/smm.h>
#include "southbridge/intel/i82801gx/nvs.h"
#include "southbridge/intel/i82801gx/i82801gx.h"
#include <pc80/mc146818rtc.h>
#include <delay.h>

/* The southbridge SMI handler checks whether gnvs has a
 * valid pointer before calling the trap handler
 */
extern global_nvs_t *gnvs;

static void mainboard_smm_init(void)
{
	printk(BIOS_DEBUG, "initializing SMI\n");
}

int mainboard_io_trap_handler(int smif)
{
	static int smm_initialized;

	if (!smm_initialized) {
		mainboard_smm_init();
		smm_initialized = 1;
	}

	switch (smif) {
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}

int mainboard_smi_apmc(u8 data)
{
	u16 pmbase = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0x40) & 0xfffc;
	u8 tmp;

	printk(BIOS_DEBUG, "%s: pmbase %04X, data %02X\n", __func__, pmbase, data);

	if (!pmbase)
		return 0;

	switch(data) {
		case APM_CNT_ACPI_ENABLE:
			/* route H8SCI to SCI */
			outw(inw(pmbase + ALT_GP_SMI_EN) & ~0x1000, pmbase + ALT_GP_SMI_EN);
			tmp = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xbb);
			tmp &= ~0x03;
			tmp |= 0x02;
			pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xbb, tmp);
			break;
		case APM_CNT_ACPI_DISABLE:
			/* route H8SCI# to SMI */
			outw(inw(pmbase + ALT_GP_SMI_EN) | 0x1000, pmbase + ALT_GP_SMI_EN);
			tmp = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xbb);
			tmp &= ~0x03;
			tmp |= 0x01;
			pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xbb, tmp);
			break;
		default:
			break;
	}
	return 0;
}
