/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <types.h>

bool mca_has_expected_bank_count(void)
{
	return true;
}

bool mca_is_valid_bank(unsigned int bank)
{
	return false;
}

const char *mca_get_bank_name(unsigned int bank)
{
	return "";
}
