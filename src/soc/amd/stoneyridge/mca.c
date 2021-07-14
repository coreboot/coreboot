/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>
#include <types.h>

static const char *const mca_bank_name[] = {
	[0] = "Load-store unit",
	[1] = "Instruction fetch unit",
	[2] = "Combined unit",
	/* Bank 3 is reserved and not all corresponding MSRs are implemented in Family 15h.
	   Accessing non-existing MSRs causes a general protection fault. */
	[3] = NULL,
	[4] = "Northbridge",
	[5] = "Execution unit",
	[6] = "Floating point unit"
};

bool mca_has_expected_bank_count(void)
{
	return ARRAY_SIZE(mca_bank_name) == mca_get_bank_count();
}

bool mca_is_valid_bank(unsigned int bank)
{
	return (bank < ARRAY_SIZE(mca_bank_name) && mca_bank_name[bank] != NULL);
}

const char *mca_get_bank_name(unsigned int bank)
{
	if (mca_is_valid_bank(bank))
		return mca_bank_name[bank];
	else
		return "";
}
