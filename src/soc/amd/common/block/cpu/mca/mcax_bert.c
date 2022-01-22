/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <amdblocks/msr_zen.h>
#include <cpu/x86/msr.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <arch/bert_storage.h>
#include <cper.h>
#include <types.h>
#include "mca_common_defs.h"

/* MISC4 is the last used register in the MCAX banks of Picasso */
#define MCAX_USED_REGISTERS_PER_BANK	(MCAX_MISC4_OFFSET + 1)

static inline size_t mca_report_size_reqd(void)
{
	size_t size;

	size = sizeof(acpi_generic_error_status_t);

	size += sizeof(acpi_hest_generic_data_v300_t);
	size += sizeof(cper_proc_generic_error_section_t);

	size += sizeof(acpi_hest_generic_data_v300_t);
	size += sizeof(cper_ia32x64_proc_error_section_t);

	/* Check Error */
	size += cper_ia32x64_check_sz();

	/* Context of MCG_CAP, MCG_STAT, MCG_CTL */
	size += cper_ia32x64_ctx_sz_bytype(CPER_IA32X64_CTX_MSR, 3);

	/* Context of CTL, STATUS, ADDR, MISC0, CONFIG, IPID, SYND, RESERVED, DESTAT, DEADDR,
	   MISC1, MISC2, MISC3, MISC4 */
	size += cper_ia32x64_ctx_sz_bytype(CPER_IA32X64_CTX_MSR, MCAX_USED_REGISTERS_PER_BANK);

	/* Context of CTL_MASK */
	size += cper_ia32x64_ctx_sz_bytype(CPER_IA32X64_CTX_MSR, 1);

	return size;
}

/* Convert an error reported by an MCA bank into BERT information to be reported
 * by the OS.  The ACPI driver doesn't recognize/parse the IA32/X64 structure,
 * which is the best method to report MSR context.  As a result, add two
 * structures:  A "processor generic error" that is parsed, and an IA32/X64 one
 * to capture complete information.
 */
void build_bert_mca_error(struct mca_bank_status *mci)
{
	acpi_generic_error_status_t *status;
	acpi_hest_generic_data_v300_t *gen_entry;
	acpi_hest_generic_data_v300_t *x86_entry;
	cper_proc_generic_error_section_t *gen_sec;
	cper_ia32x64_proc_error_section_t *x86_sec;
	cper_ia32x64_proc_error_info_t *chk;
	cper_ia32x64_context_t *ctx;

	if (mca_report_size_reqd() > bert_storage_remaining())
		goto failed;

	status = bert_new_event(&CPER_SEC_PROC_GENERIC_GUID);
	if (!status)
		goto failed;

	gen_entry = acpi_hest_generic_data3(status);
	gen_sec = section_of_acpientry(gen_sec, gen_entry);

	fill_generic_section(gen_sec, mci);

	x86_entry = bert_append_ia32x64(status);
	x86_sec = section_of_acpientry(x86_sec, x86_entry);

	chk = new_cper_ia32x64_check(status, x86_sec, error_to_chktype(mci));
	if (!chk)
		goto failed;

	ctx = cper_new_ia32x64_context_msr(status, x86_sec, IA32_MCG_CAP, 3);
	if (!ctx)
		goto failed;
	ctx = cper_new_ia32x64_context_msr(status, x86_sec, MCAX_CTL_MSR(mci->bank),
					   MCAX_USED_REGISTERS_PER_BANK);
	if (!ctx)
		goto failed;
	ctx = cper_new_ia32x64_context_msr(status, x86_sec, MCA_CTL_MASK_MSR(mci->bank), 1);
	if (!ctx)
		goto failed;

	return;

failed:
	/* We're here because of a hardware error, don't break something else */
	printk(BIOS_ERR, "Not enough room in BERT region for Machine Check error\n");
}
