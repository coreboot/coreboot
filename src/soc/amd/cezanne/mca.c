/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>

static void mca_check_all_banks(void)
{
	/* TODO: Implement MCAX register checking and BERT table generation. */
}

/* Check the Machine Check Architecture Extension registers */
void check_mca(void)
{
	mca_check_all_banks();
	/* mca_clear_status uses the MCA registers and not the MCAX ones. Since they are
	   aliases, we can use either set of registers. */
	mca_clear_status();
}
