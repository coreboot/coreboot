/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>

void check_mca(void)
{
	mca_check_all_banks();
	/* mca_clear_status uses the MCA registers which can be used in both the MCA and MCAX
	  case */
	mca_clear_status();
}
