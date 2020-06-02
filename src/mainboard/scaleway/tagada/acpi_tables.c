/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <device/device.h>
#include <cpu/x86/msr.h>

#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/nvs.h>

extern const unsigned char AmlCode[];

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 0;
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
