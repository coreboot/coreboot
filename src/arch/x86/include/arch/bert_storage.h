/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BERT_STORAGE_H_
#define _BERT_STORAGE_H_

#include <acpi/acpi.h>
#include <types.h>

/* Items in the BERT region
 *
 *  * Each item begins with a Generic Error Status Block
 *  * Zero or more Generic Error Data Entries follow, and
 *    are associated with the Status Block
 *  * Each Generic Error Data Entry must be a certain type,
 *    as defined in the UEFI CPER appendix
 *  * Each type may allow zero or more additional sets of
 *    data, e.g. error descriptions, or processor contexts.
 *
 * In the example layout below, there are three BERT region
 * entries.  The first two are a single error.  The third
 * has two errors, with one providing a variable amount
 * of additional information.
 *
 * +====================================================================+
 * | Generic Error  | Generic Error   | Platform Memory Error           |
 * | Status         | Data Entry      |                                 |
 * |====================================================================|
 * | Generic Error  | Generic Error   | Generic Processor Error         |
 * | Status         | Data Entry      |                                 |
 * |====================================================================|
 * | Generic Error  | Generic Error   | IA32/X64 Processor Error        |
 * | Status         | Data Entry      |    +----------------------------+
 * |                |                 |    | Error Check Data           |
 * |                |                 |    +----------------------------+
 * |                |                 |    | MSR Context                |
 * |                |                 |    +----------------------------+
 * |                |                 |    | X64 Registers Context      |
 * |                +-----------------+----+----------------------------+
 * |                | Generic Error   | PCI Express Error               |
 * |                | Data Entry      |                                 |
 * +--------------------------------------------------------------------+
 */

#define CRASHLOG_RECORD_TYPE	0x2
#define CRASHLOG_FW_ERR_REV	0x2

/* Get the region where BERT error structures have been constructed for
 * generating the ACPI table
 */
void bert_errors_region(void **start, size_t *size);

/* Get amount of available storage left for error info */
size_t bert_storage_remaining(void);
/* Find if errors were added, a BERT region is present, and ACPI table needed */
bool bert_errors_present(void);
/* The BERT table should only be generated when BERT support is enabled and there's an error */
static inline bool bert_should_generate_acpi_table(void)
{
	return CONFIG(ACPI_BERT) && bert_errors_present();
}

/* Get the number of entries associated with status */
static inline size_t bert_entry_count(acpi_generic_error_status_t *status)
{
	return (status->block_status & GENERIC_ERR_STS_ENTRY_COUNT_MASK)
				>> GENERIC_ERR_STS_ENTRY_COUNT_SHIFT;
}

/* Increment the number of entries this status describes */
static inline void bert_bump_entry_count(acpi_generic_error_status_t *status)
{
	int count;

	count = bert_entry_count(status) + 1;
	status->block_status &= ~GENERIC_ERR_STS_ENTRY_COUNT_MASK;
	status->block_status |= count << GENERIC_ERR_STS_ENTRY_COUNT_SHIFT;
}

/* Find the address of the first Generic Data structure from its status entry */
static inline acpi_hest_generic_data_v300_t *acpi_hest_generic_data3(
		acpi_generic_error_status_t *status)
{
	return (acpi_hest_generic_data_v300_t *)
			((u8 *)status + sizeof(*status));
}

/* Find the address of a Generic Data structure's CPER error record section */
#define section_of_acpientry(A, B) ((typeof(A))((u8 *)(B) + sizeof(*(B))))

/* Add a context to an existing IA32/X64-type error entry */
cper_ia32x64_context_t *new_cper_ia32x64_ctx(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err, int type, int num);

/* Helper to add an MSR context to an existing IA32/X64-type error entry */
cper_ia32x64_context_t *cper_new_ia32x64_context_msr(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err, u32 addr, int num);

/* Add check info to an existing IA32/X64-type error entry */
cper_ia32x64_proc_error_info_t *new_cper_ia32x64_check(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err,
		enum cper_x86_check_type type);

/* Append a new ACPI Generic Error Data Entry plus CPER Error Section to an
 * existing ACPI Generic Error Status Block.  The caller is responsible for
 * the setting the status and entry severity, as well as populating all fields
 * of the error section.
 */
acpi_hest_generic_data_v300_t *bert_append_error_datasection(
		acpi_generic_error_status_t *status, guid_t *guid);

/* Helper to append an ACPI Generic Error Data Entry plus a CPER Processor
 * Generic Error Section.  As many fields are populated as possible for the
 * caller.
 */
acpi_hest_generic_data_v300_t *bert_append_genproc(
					acpi_generic_error_status_t *status);

/* Helper to append an ACPI Generic Error Data Entry plus a CPER IA32/X64
 * Processor Error Section.  As many fields are populated as possible for the
 * caller.
 */
acpi_hest_generic_data_v300_t *bert_append_ia32x64(
					acpi_generic_error_status_t *status);

void *new_cper_fw_error_crashlog(acpi_generic_error_status_t *status, size_t cl_size);
acpi_hest_generic_data_v300_t *bert_append_fw_err(acpi_generic_error_status_t *status);

/* Add a new event to the BERT region.  An event consists of an ACPI Error
 * Status Block, a Generic Error Data Entry, and an associated CPER Error
 * Section.
 */
acpi_generic_error_status_t *bert_new_event(guid_t *guid);

#endif /* _BERT_STORAGE_H_ */
