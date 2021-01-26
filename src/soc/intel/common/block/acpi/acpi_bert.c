/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/bert_storage.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include <intelblocks/crashlog.h>


void acpi_soc_fill_bert(acpi_bert_t *bert,
			void **region,
			size_t *length)
{
	acpi_generic_error_status_t *status = NULL;
	size_t cpu_record_size, pmc_record_size;
	void *cl_data = NULL;

	if (!cl_get_total_data_size()) {
		printk(BIOS_ERR, "Error: No crashlog record present\n");
		return;
	}

	status = bert_new_event(&CPER_SEC_FW_ERR_REC_REF_GUID);
	if (!status) {
		printk(BIOS_ERR, "Error: unable to allocate GSB\n");
		return;
	}

	if (cl_get_total_data_size() > bert_storage_remaining()) {
		printk(BIOS_ERR, "Error: Crashlog entry would exceed "
				"available region\n");
		return;
	}

	cpu_record_size = cl_get_cpu_record_size();
	if (cpu_record_size) {
		cl_data = new_cper_fw_error_crashlog(status, cpu_record_size);
		if (!cl_data) {
			printk(BIOS_ERR, "Error: Crashlog CPU entry(size %lu) "
				"would exceed available region\n",
				cpu_record_size);
			return;
		}
		printk(BIOS_DEBUG, "cl_data %p, cpu_record_size %lu\n",
			cl_data, cpu_record_size);
		cl_fill_cpu_records(cl_data);
	}

	pmc_record_size = cl_get_pmc_record_size();
	if (pmc_record_size) {
		/*  Allocate new FW ERR structure in case CPU crashlog is present */
		if (cpu_record_size && !bert_append_fw_err(status)) {
			printk(BIOS_ERR, "Error: Crashlog PMC entry would "
				"exceed available region\n");
			return;
		}

		cl_data = new_cper_fw_error_crashlog(status, pmc_record_size);
		if (!cl_data) {
			printk(BIOS_ERR, "Error: Crashlog PMC entry(size %lu) "
				"would exceed available region\n",
				pmc_record_size);
			return;
		}
		printk(BIOS_DEBUG, "cl_data %p, pmc_record_size %lu\n",
			cl_data, pmc_record_size);
		cl_fill_pmc_records(cl_data);
	}

	*length = status->raw_data_length;
	*region = (void *)status;

}

bool acpi_is_boot_error_src_present(void)
{

	if (!CONFIG(SOC_INTEL_CRASHLOG)) {
		printk(BIOS_DEBUG, "Crashlog disabled.\n");
		return false;
	}

	if (!discover_crashlog()) {
		printk(BIOS_SPEW, "Crashlog discovery result: crashlog not found\n");
		return false;
	}

	collect_pmc_and_cpu_crashlog_from_srams();

	/* Discovery tables sizes can be larger than the actual valid collected data */
	u32 crashlog_size = cl_get_total_data_size();

	return (crashlog_size > 0);
}
