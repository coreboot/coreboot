/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/bert_storage.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include <intelblocks/crashlog.h>

static bool boot_error_src_present(void)
{
	if (!discover_crashlog()) {
		printk(BIOS_SPEW, "Crashlog discovery result: crashlog not found\n");
		return false;
	}

	collect_pmc_and_cpu_crashlog_from_srams();

	/* Discovery tables sizes can be larger than the actual valid collected data */
	u32 crashlog_size = cl_get_total_data_size();

	return (crashlog_size > 0);
}

static enum cb_err record_crashlog_into_bert(void **region, size_t *length)
{
	acpi_generic_error_status_t *status = NULL;
	size_t cpu_record_size, pmc_record_size;
	size_t gesb_header_size;
	void *cl_data = NULL;

	if (!boot_error_src_present()) {
		return CB_ERR;
	}

	if (!cl_get_total_data_size()) {
		printk(BIOS_ERR, "No crashlog record present\n");
		return CB_ERR;
	}

	status = bert_new_event(&CPER_SEC_FW_ERR_REC_REF_GUID);
	gesb_header_size = sizeof(*status);

	if (!status) {
		printk(BIOS_ERR, "unable to allocate GSB\n");
		return CB_ERR;
	}

	if (cl_get_total_data_size() > bert_storage_remaining()) {
		printk(BIOS_ERR, "Crashlog entry would exceed "
				"available region\n");
		return CB_ERR;
	}

	cpu_record_size = cl_get_cpu_record_size();
	if (cpu_record_size) {
		cl_data = new_cper_fw_error_crashlog(status, cpu_record_size);
		if (!cl_data) {
			printk(BIOS_ERR, "Crashlog CPU entry(size 0x%zx) "
				"would exceed available region\n",
				cpu_record_size);
			return CB_ERR;
		}
		printk(BIOS_DEBUG, "cl_data %p, cpu_record_size 0x%zx\n",
			cl_data, cpu_record_size);
		cl_fill_cpu_records(cl_data);
	}

	pmc_record_size = cl_get_pmc_record_size();
	if (pmc_record_size) {
		/* Allocate new FW ERR structure in case PMC crashlog is present */
		if (pmc_record_size && !bert_append_fw_err(status)) {
			printk(BIOS_ERR, "Crashlog PMC entry would "
				"exceed available region\n");
			return CB_ERR;
		}

		cl_data = new_cper_fw_error_crashlog(status, pmc_record_size);
		if (!cl_data) {
			printk(BIOS_ERR, "Crashlog PMC entry(size 0x%zx) "
				"would exceed available region\n",
				pmc_record_size);
			return CB_ERR;
		}
		printk(BIOS_DEBUG, "cl_data %p, pmc_record_size 0x%zx\n",
			cl_data, pmc_record_size);
		cl_fill_pmc_records(cl_data);
	}

	if (CONFIG(SOC_INTEL_IOE_DIE_SUPPORT)) {
		size_t ioe_record_size = cl_get_ioe_record_size();
		if (ioe_record_size) {
			/*  Allocate new FW ERR structure in case IOE crashlog is present */
			if (ioe_record_size && !bert_append_fw_err(status)) {
				printk(BIOS_ERR, "Crashlog IOE entry would "
						"exceed available region\n");
				return CB_ERR;
			}

			cl_data = new_cper_fw_error_crashlog(status, ioe_record_size);
			if (!cl_data) {
				printk(BIOS_ERR, "Crashlog IOE entry(size 0x%zx) "
						"would exceed available region\n",
						ioe_record_size);
				return CB_ERR;
			}
			printk(BIOS_DEBUG, "cl_data %p, ioe_record_size 0x%zx\n",
					cl_data, ioe_record_size);
			cl_fill_ioe_records(cl_data);
		}
	}

	*length = status->data_length + gesb_header_size;
	*region = (void *)status;

	return CB_SUCCESS;
}

enum cb_err acpi_soc_get_bert_region(void **region, size_t *length)
{
	if (CONFIG(SOC_INTEL_CRASHLOG)) {
		return record_crashlog_into_bert(region, length);
	} else {
		/* Check if MCA error has been added into BERT. */
		if (bert_should_generate_acpi_table()) {
			bert_errors_region(region, length);
			if (!*region) {
				printk(BIOS_ERR, "Can't find BERT storage area\n");
				return CB_ERR;
			}
		}
		return CB_SUCCESS;
	}
}
