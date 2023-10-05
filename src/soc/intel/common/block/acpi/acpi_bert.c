/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/bert_storage.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include <intelblocks/crashlog.h>

static bool boot_error_src_present(cl_node_t *head)
{
	if (!discover_crashlog()) {
		printk(BIOS_SPEW, "Crashlog discovery result: crashlog not found\n");
		return false;
	}

	collect_pmc_and_cpu_crashlog_from_srams(head);

	/* Discovery tables sizes can be larger than the actual valid collected data */
	u32 crashlog_size = cl_get_total_data_size();

	return (crashlog_size > 0);
}

static enum cb_err record_crashlog_into_bert(void **region, size_t *length)
{
	acpi_generic_error_status_t *status = NULL;
	size_t gesb_header_size;
	void *cl_acpi_data = NULL;
	cl_node_t cl_list_head = {.size = 0, .data = NULL, .next = NULL};

	if (!boot_error_src_present(&cl_list_head)) {
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
		printk(BIOS_ERR, "Crashlog entry would exceed available region\n");
		return CB_ERR;
	}

	bool multi_entry = false;
	cl_node_t *cl_node = cl_list_head.next;
	while (cl_node) {

		if ((cl_node->size <= 0) || (!(cl_node->data))) {
			cl_node = cl_node->next;
			continue;
		}

		if (multi_entry) {
			if (!bert_append_fw_err(status)) {
				printk(BIOS_ERR, "Crashlog entry would exceed available region\n");
				return CB_ERR;
			}
		}

		cl_acpi_data = new_cper_fw_error_crashlog(status, cl_node->size);
		if (!cl_acpi_data) {
			printk(BIOS_ERR, "Crashlog entry(size 0x%x) would exceed available region\n",
					cl_node->size);
			return CB_ERR;
		}
		memcpy(cl_acpi_data, (void *) cl_node->data, cl_node->size);

		cl_node_t *temp = cl_node;
		cl_node = cl_node->next;
		free_cl_node(temp);
		multi_entry = true;
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
