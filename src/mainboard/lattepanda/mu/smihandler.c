/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <arch/io.h>
#include <cpu/x86/smm.h>
#include <intelblocks/smihandler.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8613e/it8613e.h>

#define it8613e_index	(0x2e)
#define EC_DEV		PNP_DEV(it8613e_index, IT8613E_EC)

void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "SMI: sleep S%d\n", slp_typ);

	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
	case ACPI_S5:
		/* 5VSB_CTRL# Enabled */
		ite_reg_write(EC_DEV, 0xfa, 2);
		break;
	default:
		break;
	}

}
