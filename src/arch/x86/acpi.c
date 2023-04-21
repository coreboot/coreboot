/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cf9_reset.h>
#include <cpu/x86/smm.h>

void arch_fill_fadt(acpi_fadt_t *fadt)
{
	if (CONFIG(HAVE_CF9_RESET)) {
		fadt->reset_reg.space_id = ACPI_ADDRESS_SPACE_IO;
		fadt->reset_reg.bit_width = 8;
		fadt->reset_reg.bit_offset = 0;
		fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
		fadt->reset_reg.addrl = RST_CNT;
		fadt->reset_reg.addrh = 0;

		fadt->reset_value = RST_CPU | SYS_RST;

		fadt->flags |= ACPI_FADT_RESET_REGISTER;
	}

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}
}
