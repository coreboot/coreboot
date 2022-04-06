/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/name.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <acpi/acpi.h>
#include <arch/bert_storage.h>
#include <string.h>
#include <types.h>

/* BERT region management:  Allow the chipset to determine the specific
 * location of the BERT region.  We find that base and size, then manage
 * the allocation of error information within it.
 *
 * Use simple static variables for managing the BERT region.  This is a thin
 * implementation; it is only created and consumed by coreboot, and only in
 * a single stage, and we don't want its information to survive reboot or
 * resume cycles.  If the requirements change, consider using IMD to help
 * manage the space.
 */
static bool bert_region_broken;
static void *bert_region_base;
static size_t bert_region_size;
static size_t bert_region_used;

/* Calculate the remaining space in the BERT region.  This knowledge may help
 * the caller prioritize the information to store.
 */
size_t bert_storage_remaining(void)
{
	return bert_region_broken ? 0 : bert_region_size - bert_region_used;
}

bool bert_errors_present(void)
{
	return !bert_region_broken && bert_region_used;
}

void bert_errors_region(void **start, size_t *size)
{
	if (bert_region_broken) {
		*start = NULL;
		*size = 0;
		return;
	}

	/* No metadata, etc. with our region, so this is easy */
	*start = bert_region_base;
	*size = bert_region_used;
}

static void *bert_allocate_storage(size_t size)
{
	size_t alloc;

	if (bert_region_broken)
		return NULL;
	if (bert_region_used + size > bert_region_size)
		return NULL;

	alloc = bert_region_used;
	bert_region_used += size;

	return (void *)((u8 *)bert_region_base + alloc);
}

/* Generic Error Status:  Each Status represents a unique error event within
 * the BERT errors region.  Each event may have multiple errors associated
 * with it.
 */

/* Find the nth (1-based) Generic Data Structure attached to an Error Status */
static void *acpi_hest_generic_data_nth(
		acpi_generic_error_status_t *status, int num)
{
	acpi_hest_generic_data_v300_t *ptr;
	size_t struct_size;

	if (!num || num > bert_entry_count(status))
		return NULL;

	ptr = (acpi_hest_generic_data_v300_t *)(status + 1);
	while (--num) {
		if (ptr->revision == HEST_GENERIC_ENTRY_V300)
			struct_size = sizeof(acpi_hest_generic_data_v300_t);
		else
			struct_size = sizeof(acpi_hest_generic_data_t);
		ptr = (acpi_hest_generic_data_v300_t *)(
				(u8 *)ptr
				+ ptr->data_length
				+ struct_size);
	}
	return ptr;
}

/* Update data_length for this Error Status, and final Data Entry it contains */
static void revise_error_sizes(acpi_generic_error_status_t *status, size_t size)
{
	acpi_hest_generic_data_v300_t *entry;
	int entries;

	if (!status)
		return;

	entries = bert_entry_count(status);
	entry = acpi_hest_generic_data_nth(status, entries);
	status->data_length += size;
	if (entry)
		entry->data_length += size;
}

/* Create space for a new BERT Generic Error Status Block, by finding the next
 * available slot and moving the ending location.  There is nothing to designate
 * this as another Generic Error Status Block (e.g. no signature); only that it
 * is within the BERT region.
 *
 * It is up to the caller to correctly fill the information, including status
 * and error severity, and to update/maintain data offsets and lengths as
 * entries are added.
 */
static acpi_generic_error_status_t *new_bert_status(void)
{
	acpi_generic_error_status_t *status;

	status = bert_allocate_storage(sizeof(*status));

	if (!status) {
		printk(BIOS_ERR, "New BERT error entry would exceed available region\n");
		return NULL;
	}

	status->error_severity = ACPI_GENERROR_SEV_NONE;
	return status;
}

/* Generic Error Data:  Each Generic Error Status may contain zero or more
 * Generic Error Data structures.  The data structures describe particular
 * error(s) associated with an event.  The definition for the structure is
 * found in the ACPI spec, however the data types and any accompanying data
 * definitions are in the Common Platform Error Record appendix of the UEFI
 * spec.
 */

/* Create space for a new BERT Generic Data Entry.  Update the count and
 * data length in the parent Generic Error Status Block.  Version 0x300 of
 * the structure is used, and the timestamp is filled and marked precise
 * (i.e. assumed close enough for reporting).
 *
 * It is up to the caller to fill the Section Type field and add the Common
 * Platform Error Record type data as appropriate.  In addition, the caller
 * should update the error severity, and may optionally add FRU information
 * or override any existing information.
 */
static acpi_hest_generic_data_v300_t *new_generic_error_entry(
		acpi_generic_error_status_t *status)
{
	acpi_hest_generic_data_v300_t *entry;

	if (bert_entry_count(status) == GENERIC_ERR_STS_ENTRY_COUNT_MAX) {
		printk(BIOS_ERR, "New BERT error would exceed maximum entries\n");
		return NULL;
	}

	entry = bert_allocate_storage(sizeof(*entry));
	if (!entry) {
		printk(BIOS_ERR, "New BERT error entry would exceed available region\n");
		return NULL;
	}

	entry->revision = HEST_GENERIC_ENTRY_V300;

	entry->timestamp = cper_timestamp(CPER_TIMESTAMP_PRECISE);
	entry->validation_bits |= ACPI_GENERROR_VALID_TIMESTAMP;

	status->data_length += sizeof(*entry);
	bert_bump_entry_count(status);

	return entry;
}

/* Find the size of a CPER error section w/o any add-ons */
static size_t sizeof_error_section(guid_t *guid)
{
	if (!guidcmp(guid, &CPER_SEC_PROC_GENERIC_GUID))
		return sizeof(cper_proc_generic_error_section_t);
	else if (!guidcmp(guid, &CPER_SEC_PROC_IA32X64_GUID))
		return sizeof(cper_ia32x64_proc_error_section_t);
	else if (!guidcmp(guid, &CPER_SEC_FW_ERR_REC_REF_GUID))
		return sizeof(cper_fw_err_rec_section_t);
	/* else if ... sizeof(structures not yet defined) */

	printk(BIOS_ERR, "Requested size of unrecognized CPER GUID\n");
	return 0;
}

void *new_cper_fw_error_crashlog(acpi_generic_error_status_t *status, size_t cl_size)
{
	void *cl_data = bert_allocate_storage(cl_size);
	if (!cl_data) {
		printk(BIOS_ERR, "Crashlog entry (size %zu) would exceed available region\n",
			cl_size);
		return NULL;
	}

	revise_error_sizes(status, cl_size);

	return cl_data;
}

/* Helper to append an ACPI Generic Error Data Entry per crashlog data */
acpi_hest_generic_data_v300_t *bert_append_fw_err(acpi_generic_error_status_t *status)
{
	acpi_hest_generic_data_v300_t *entry;
	cper_fw_err_rec_section_t *fw_err;

	entry = bert_append_error_datasection(status, &CPER_SEC_FW_ERR_REC_REF_GUID);
	if (!entry)
		return NULL;

	status->block_status |= GENERIC_ERR_STS_UNCORRECTABLE_VALID;
	status->error_severity = ACPI_GENERROR_SEV_FATAL;
	entry->error_severity = ACPI_GENERROR_SEV_FATAL;

	fw_err = section_of_acpientry(fw_err, entry);

	fw_err->record_type = CRASHLOG_RECORD_TYPE;
	fw_err->revision = CRASHLOG_FW_ERR_REV;
	fw_err->record_id = 0;
	guidcpy(&fw_err->record_guid, &FW_ERR_RECORD_ID_CRASHLOG_GUID);

	return entry;
}

/* Append a new ACPI Generic Error Data Entry plus CPER Error Section to an
 * existing ACPI Generic Error Status Block.  The caller is responsible for
 * the setting the status and entry severity, as well as populating all fields
 * of the error section.
 */
acpi_hest_generic_data_v300_t *bert_append_error_datasection(
		acpi_generic_error_status_t *status, guid_t *guid)
{
	acpi_hest_generic_data_v300_t *entry;
	void *sect;
	size_t sect_size;

	sect_size = sizeof_error_section(guid);
	if (!sect_size)
		return NULL; /* Don't allocate structure if bad GUID passed */

	if (sizeof(*entry) + sect_size > bert_storage_remaining())
		return NULL;

	entry = new_generic_error_entry(status);
	if (!entry)
		return NULL;

	/* error section immediately follows the Generic Error Data Entry */
	sect = bert_allocate_storage(sect_size);
	if (!sect)
		return NULL;

	revise_error_sizes(status, sect_size);

	guidcpy(&entry->section_type, guid);
	return entry;
}

/* Helper to append an ACPI Generic Error Data Entry plus a CPER Processor
 * Generic Error Section.  As many fields are populated as possible for the
 * caller.
 */
acpi_hest_generic_data_v300_t *bert_append_genproc(
		acpi_generic_error_status_t *status)
{
	acpi_hest_generic_data_v300_t *entry;
	cper_proc_generic_error_section_t *ges;

	entry = bert_append_error_datasection(status,
					&CPER_SEC_PROC_GENERIC_GUID);
	if (!entry)
		return NULL;

	status->block_status |= GENERIC_ERR_STS_UNCORRECTABLE_VALID;
	status->error_severity = ACPI_GENERROR_SEV_FATAL;

	entry->error_severity = ACPI_GENERROR_SEV_FATAL;

	ges = section_of_acpientry(ges, entry);

	ges->proc_type = GENPROC_PROCTYPE_IA32X64;
	ges->validation |= GENPROC_VALID_PROC_TYPE;

	ges->cpu_version = cpuid_eax(1);
	ges->validation |= GENPROC_VALID_CPU_VERSION;

	fill_processor_name(ges->cpu_brand_string);
	ges->validation |= GENPROC_VALID_CPU_BRAND;

	ges->proc_id = lapicid();
	ges->validation |= GENPROC_VALID_CPU_ID;

	return entry;
}

/* Add a new IA32/X64 Processor Context Structure (Table 261), following any
 * other contexts, to an existing Processor Error Section (Table 255).  Contexts
 * may only be added after the entire Processor Error Info array has been
 * created.
 *
 * This function fills only the minimal amount of information required to parse
 * or step through the contexts.  The type is filled and PROC_CONTEXT_INFO_NUM
 * is updated.
 *
 * type is one of:
 *   CPER_IA32X64_CTX_UNCL
 *   CPER_IA32X64_CTX_MSR
 *   CPER_IA32X64_CTX_32BIT_EX
 *   CPER_IA32X64_CTX_64BIT_EX
 *   CPER_IA32X64_CTX_FXSAVE
 *   CPER_IA32X64_CTX_32BIT_DBG
 *   CPER_IA32X64_CTX_64BIT_DBG
 *   CPER_IA32X64_CTX_MEMMAPPED
 * num is the number of bytes eventually used to fill the context's register
 *   array, e.g. 4 MSRs * sizeof(msr_t)
 *
 * status and entry data_length values are updated.
 */
cper_ia32x64_context_t *new_cper_ia32x64_ctx(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err, int type, int num)
{
	size_t size;
	cper_ia32x64_context_t *ctx;
	static const char * const ctx_names[] = {
			"Unclassified Data",
			"MSR Registers",
			"32-bit Mode Execution",
			"64-bit Mode Execution",
			"FXSAVE",
			"32-bit Mode Debug",
			"64-bit Mode Debug",
			"Memory Mapped"
	};

	if (type > CPER_IA32X64_CTX_MEMMAPPED)
		return NULL;

	if (cper_ia32x64_proc_num_ctxs(x86err) == I32X64SEC_VALID_CTXNUM_MAX) {
		printk(BIOS_ERR, "New IA32X64 %s context entry would exceed max allowable contexts\n",
				ctx_names[type]);
		return NULL;
	}

	size = cper_ia32x64_ctx_sz_bytype(type, num);
	ctx = bert_allocate_storage(size);
	if (!ctx) {
		printk(BIOS_ERR, "New IA32X64 %s context entry would exceed available region\n",
				ctx_names[type]);
		return NULL;
	}

	revise_error_sizes(status, size);

	ctx->type = type;
	ctx->array_size = num;
	cper_bump_ia32x64_ctx_count(x86err);

	return ctx;
}

/* Add a new IA32/X64 Processor Error Information Structure (Table 256),
 * following any other errors, to an existing Processor Error Section
 * (Table 255).  All error structures must be added before any contexts are
 * added.
 *
 * This function fills only the minimal amount of information required to parse
 * or step through the errors.  The type is filled and PROC_ERR_INFO_NUM is
 * updated.
 */
cper_ia32x64_proc_error_info_t *new_cper_ia32x64_check(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err,
		enum cper_x86_check_type type)
{
	cper_ia32x64_proc_error_info_t *check;
	static const char * const check_names[] = {
			"cache",
			"TLB",
			"bus",
			"MS"
	};
	const guid_t check_guids[] = {
			X86_PROCESSOR_CACHE_CHK_ERROR_GUID,
			X86_PROCESSOR_TLB_CHK_ERROR_GUID,
			X86_PROCESSOR_BUS_CHK_ERROR_GUID,
			X86_PROCESSOR_MS_CHK_ERROR_GUID
	};

	if (type > X86_PROCESSOR_CHK_MAX)
		return NULL;

	if (cper_ia32x64_proc_num_chks(x86err) == I32X64SEC_VALID_ERRNUM_MAX) {
		printk(BIOS_ERR, "New IA32X64 %s check entry would exceed max allowable errors\n",
				check_names[type]);
		return NULL;
	}

	check = bert_allocate_storage(sizeof(*check));
	if (!check) {
		printk(BIOS_ERR, "New IA32X64 %s check entry would exceed available region\n",
				check_names[type]);
		return NULL;
	}

	revise_error_sizes(status, sizeof(*check));

	guidcpy(&check->type, &check_guids[type]);
	cper_bump_ia32x64_chk_count(x86err);

	return check;
}

/* Helper to append an ACPI Generic Error Data Entry plus a CPER IA32/X64
 * Processor Error Section.  As many fields are populated as possible for the
 * caller.
 */
acpi_hest_generic_data_v300_t *bert_append_ia32x64(
					acpi_generic_error_status_t *status)
{
	acpi_hest_generic_data_v300_t *entry;
	cper_ia32x64_proc_error_section_t *ipe;
	struct cpuid_result id;

	entry = bert_append_error_datasection(status,
					&CPER_SEC_PROC_IA32X64_GUID);
	if (!entry)
		return NULL;

	status->block_status |= GENERIC_ERR_STS_UNCORRECTABLE_VALID;
	status->error_severity = ACPI_GENERROR_SEV_FATAL;

	entry->error_severity = ACPI_GENERROR_SEV_FATAL;

	ipe = section_of_acpientry(ipe, entry);

	ipe->apicid = lapicid();
	ipe->validation |= I32X64SEC_VALID_LAPIC;

	id = cpuid(1);
	ipe->cpuid[0] = id.eax;
	ipe->cpuid[1] = id.ebx;
	ipe->cpuid[2] = id.ecx;
	ipe->cpuid[3] = id.edx;
	ipe->validation |= I32X64SEC_VALID_CPUID;

	return entry;
}

static const char * const generic_error_types[] = {
	"PROCESSOR_GENERIC",
	"PROCESSOR_SPECIFIC_X86",
	"PROCESSOR_SPECIFIC_ARM",
	"PLATFORM_MEMORY",
	"PLATFORM_MEMORY2",
	"PCIE",
	"FW_ERROR_RECORD",
	"PCI_PCIX_BUS",
	"PCI_DEVICE",
	"DMAR_GENERIC",
	"DIRECTED_IO_DMAR",
	"IOMMU_DMAR",
	"UNRECOGNIZED"
};

static const char *generic_error_name(guid_t *guid)
{
	if (!guidcmp(guid, &CPER_SEC_PROC_GENERIC_GUID))
		return generic_error_types[0];
	if (!guidcmp(guid, &CPER_SEC_PROC_IA32X64_GUID))
		return generic_error_types[1];
	if (!guidcmp(guid, &CPER_SEC_PROC_ARM_GUID))
		return generic_error_types[2];
	if (!guidcmp(guid, &CPER_SEC_PLATFORM_MEM_GUID))
		return generic_error_types[3];
	if (!guidcmp(guid, &CPER_SEC_PLATFORM_MEM2_GUID))
		return generic_error_types[4];
	if (!guidcmp(guid, &CPER_SEC_PCIE_GUID))
		return generic_error_types[5];
	if (!guidcmp(guid, &CPER_SEC_FW_ERR_REC_REF_GUID))
		return generic_error_types[6];
	if (!guidcmp(guid, &CPER_SEC_PCI_X_BUS_GUID))
		return generic_error_types[7];
	if (!guidcmp(guid, &CPER_SEC_PCI_DEV_GUID))
		return generic_error_types[8];
	if (!guidcmp(guid, &CPER_SEC_DMAR_GENERIC_GUID))
		return generic_error_types[9];
	if (!guidcmp(guid, &CPER_SEC_DMAR_VT_GUID))
		return generic_error_types[10];
	if (!guidcmp(guid, &CPER_SEC_DMAR_IOMMU_GUID))
		return generic_error_types[11];
	return generic_error_types[12];
}

/* Add a new event to the BERT region.  An event consists of an ACPI Error
 * Status Block, a Generic Error Data Entry, and an associated CPER Error
 * Section.
 */
acpi_generic_error_status_t *bert_new_event(guid_t *guid)
{
	size_t size;
	acpi_generic_error_status_t *status;
	acpi_hest_generic_data_v300_t *entry, *r;

	size = sizeof(*status);
	size += sizeof(*entry);
	size += sizeof_error_section(guid);

	if (size > bert_storage_remaining()) {
		printk(BIOS_ERR, "Not enough BERT region space to add event for type %s\n",
				generic_error_name(guid));
		return NULL;
	}

	status = new_bert_status();
	if (!status)
		return NULL;

	if (!guidcmp(guid, &CPER_SEC_PROC_GENERIC_GUID))
		r = bert_append_genproc(status);
	else if (!guidcmp(guid, &CPER_SEC_PROC_GENERIC_GUID))
		r = bert_append_ia32x64(status);
	else if (!guidcmp(guid, &CPER_SEC_FW_ERR_REC_REF_GUID))
		r = bert_append_fw_err(status);
	/* else if other types not implemented */
	else
		r = NULL;

	if (r)
		return status;
	return NULL;
}

/* Helper to add an MSR context to an existing IA32/X64-type error entry */
cper_ia32x64_context_t *cper_new_ia32x64_context_msr(
		acpi_generic_error_status_t *status,
		cper_ia32x64_proc_error_section_t *x86err, u32 addr, int num)
{
	cper_ia32x64_context_t *ctx;
	int i;
	msr_t *dest;

	ctx = new_cper_ia32x64_ctx(status, x86err, CPER_IA32X64_CTX_MSR, num);
	if (!ctx)
		return NULL;

	/* already filled ctx->type = CPER_IA32X64_CTX_MSR; */
	ctx->msr_addr = addr;
	ctx->array_size = num * sizeof(msr_t);

	dest = (msr_t *)((u8 *)(ctx + 1)); /* point to the Register Array */

	for (i = 0 ; i < num ; i++)
		*(dest + i) = rdmsr(addr + i);
	return ctx;
}

/* The region must be in memory marked as reserved.  If not implemented,
 * skip generating the information in the region.
 */
__weak void bert_reserved_region(void **start, size_t *size)
{
	printk(BIOS_ERR, "%s not implemented.  BERT region generation disabled\n",
			__func__);
	*start = NULL;
	*size = 0;
}

static void bert_storage_setup(void *unused)
{
	/* Always start with a blank bert region.  Make sure nothing is
	 * maintained across reboots or resumes.
	 */
	bert_region_broken = false;
	bert_region_used = 0;

	bert_reserved_region(&bert_region_base, &bert_region_size);

	if (!bert_region_base || !bert_region_size) {
		printk(BIOS_ERR, "Bug: Can't find/add BERT storage area\n");
		bert_region_broken = true;
		return;
	}

	memset(bert_region_base, 0, bert_region_size);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, bert_storage_setup, NULL);
