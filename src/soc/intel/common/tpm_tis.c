/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <security/tpm/tis.h>

int tis_plat_irq_status(void)
{
	return acpi_get_gpe(CONFIG_TPM_TIS_ACPI_INTERRUPT);
}
