/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>
#include <types.h>

/* TODO: Check if non-core MCA banks are same for all cores */
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
	[15] = "Microprocessor5 Management Controller",
	[16] = "Parameter Block",
	[17] = "GMI Controller",
	[18] = "GMI Controller",
	[19] = "High Speed Interface Unit (GMI)",
	[20] = "High Speed Interface Unit (GMI)",
	[21] = "Unified Memory Controller",
	[22] = "Unified Memory Controller",
	[23] = "Coherent Station",
	[24] = "Coherent Station",
	[25] = "Northbridge IO Unit",
	[26] = "PCIe Root Port",
	[27] = "PCIe Root Port",
	[28] = "Power Management, Interrupts, Etc.",
	[29] = "SMU",
	[30] = "XGMI Controller",
	[31] = "High Speed Interface Unit (XGMI)",
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
