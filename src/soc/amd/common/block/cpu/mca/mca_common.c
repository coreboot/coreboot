/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <types.h>
#include "mca_common_defs.h"

static void mca_check_all_banks(void)
{
	struct mca_bank_status mci;
	const unsigned int num_banks = mca_get_bank_count();

	if (!mca_has_expected_bank_count())
		printk(BIOS_WARNING, "CPU has an unexpected number of MCA banks!\n");

	if (mca_skip_check())
		return;

	for (unsigned int i = 0 ; i < num_banks ; i++) {
		if (!mca_is_valid_bank(i))
			continue;

		mci.bank = i;
		/* The MCA status register can be used in both the MCA and MCAX case */
		mci.sts = rdmsr(IA32_MC_STATUS(i));
		if (mci.sts.hi || mci.sts.lo) {
			mca_print_error(i);

			if (CONFIG(ACPI_BERT) && mca_valid(mci.sts))
				build_bert_mca_error(&mci);
		}
	}
}

void check_mca(void)
{
	mca_check_all_banks();
	/* mca_clear_status uses the MCA registers which can be used in both the MCA and MCAX
	  case */
	mca_clear_status();
}
