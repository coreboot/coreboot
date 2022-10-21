/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>
#include <types.h>

static const char *const mca_bank_name[] = {
	[0]  = "Load-store unit",
	[1]  = "Instruction fetch unit",
	[2]  = "L2 cache unit",
	[3]  = "Decode unit",
	[4]  = "",
	[5]  = "Execution unit",
	[6]  = "Floating point unit",
	[7]  = "L3 cache unit",
	[8]  = "L3 cache unit",
	[9]  = "L3 cache unit",
	[10] = "L3 cache unit",
	[11] = "L3 cache unit",
	[12] = "L3 cache unit",
	[13] = "L3 cache unit",
	[14] = "L3 cache unit",
	[15] = "UMC",
	[16] = "UMC",
	[17] = "UMC",
	[18] = "UMC",
	[19] = "CS",
	[20] = "CS",
	[21] = "CS",
	[22] = "CS",
	[23] = "",
	[24] = "",
	[25] = "NBIO",
	[26] = "",
	[27] = "",
	[28] = "PIE",
	[29] = "",
	[30] = "",
	[31] = "",
	[32] = "",
	[33] = "",
	[34] = "",
	[35] = "",
	[36] = "",
	[37] = "",
	[38] = "",
	[39] = "",
	[40] = "",
	[41] = "",
	[42] = "",
	[43] = "",
	[44] = "",
	[45] = "",
	[46] = "",
	[47] = "",
	[48] = "",
	[49] = "",
	[50] = "",
	[51] = "",
	[52] = "",
	[53] = "",
	[54] = "",
	[55] = "",
	[56] = "",
	[57] = "",
	[58] = "",
	[59] = "",
	[60] = "",
	[61] = "",
	[62] = "",
	[63] = "",
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
