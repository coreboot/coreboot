/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/bert_storage.h>
#include <console/console.h>
#include <types.h>

enum cb_err acpi_soc_get_bert_region(void **region, size_t *length)
{
	/*
	 * Skip the table if no errors are present.  ACPI driver reports
	 * a table with a 0-length region:
	 *   BERT: [Firmware Bug]: table invalid.
	 */
	if (!bert_should_generate_acpi_table())
		return CB_ERR;

	bert_errors_region(region, length);
	if (!*region) {
		printk(BIOS_ERR, "Can't find BERT storage area\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}
