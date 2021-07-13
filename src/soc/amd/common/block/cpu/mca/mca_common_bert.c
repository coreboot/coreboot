/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cper.h>
#include <types.h>
#include "mca_common_defs.h"

enum cper_x86_check_type error_to_chktype(struct mca_bank_status *mci)
{
	int error = mca_err_type(mci->sts);

	if (error == MCA_ERRTYPE_BUS)
		return X86_PROCESSOR_BUS_CHK;
	if (error == MCA_ERRTYPE_INT)
		return X86_PROCESSOR_MS_CHK;
	if (error == MCA_ERRTYPE_MEM)
		return X86_PROCESSOR_CACHE_CHK;
	if (error == MCA_ERRTYPE_TLB)
		return X86_PROCESSOR_TLB_CHK;

	return X86_PROCESSOR_MS_CHK; /* unrecognized */
}

/* Fill additional information in the Generic Processor Error Section. */
void fill_generic_section(cper_proc_generic_error_section_t *sec,
		struct mca_bank_status *mci)
{
	int type = mca_err_type(mci->sts);

	if (type == MCA_ERRTYPE_BUS) /* try to map MCA errors to CPER types */
		sec->error_type = GENPROC_ERRTYPE_BUS;
	else if (type == MCA_ERRTYPE_INT)
		sec->error_type = GENPROC_ERRTYPE_UARCH;
	else if (type == MCA_ERRTYPE_MEM)
		sec->error_type = GENPROC_ERRTYPE_CACHE;
	else if (type == MCA_ERRTYPE_TLB)
		sec->error_type = GENPROC_ERRTYPE_TLB;
	else
		sec->error_type = GENPROC_ERRTYPE_UNKNOWN;
	sec->validation |= GENPROC_VALID_PROC_ERR_TYPE;
}
