/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/cpu.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <types.h>

static void write_cstate_entry(acpi_cstate_t *entry, const acpi_cstate_t *data,
			       uint32_t cstate_io_base_address)
{
	if (!data->ctype) {
		printk(BIOS_WARNING, "Invalid C-state data; skipping entry.\n");
		return;
	}

	entry->ctype = data->ctype;
	entry->latency = data->latency;
	entry->power = data->power;

	if (data->ctype == 1) {
		entry->resource = (acpi_addr_t){
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = 2,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.addrl = 0,
			.addrh = 0,
		};
	} else {
		entry->resource = (acpi_addr_t){
			.space_id = ACPI_ADDRESS_SPACE_IO,
			.bit_width = 8,
			.bit_offset = 0,
			/* ctype is 1-indexed while the offset into cstate_io_base_address is
			   0-indexed */
			.addrl = cstate_io_base_address + data->ctype - 1,
			.addrh = 0,
			.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS,
		};
	}
}

size_t get_cstate_info(acpi_cstate_t *cstate_values)
{
	size_t i;
	size_t cstate_count;
	uint32_t cstate_io_base_address =
		rdmsr(MSR_CSTATE_ADDRESS).lo & MSR_CSTATE_ADDRESS_MASK;
	const acpi_cstate_t *cstate_config = get_cstate_config_data(&cstate_count);

	if (cstate_count > MAX_CSTATE_COUNT) {
		printk(BIOS_WARNING, "cstate_info array has too many entries. "
			"Skipping last %zu entries.\n",
			cstate_count - MAX_CSTATE_COUNT);
		cstate_count = MAX_CSTATE_COUNT;
	}

	for (i = 0; i < cstate_count; i++) {
		write_cstate_entry(&cstate_values[i], &cstate_config[i], cstate_io_base_address);
	}

	return i;
}
