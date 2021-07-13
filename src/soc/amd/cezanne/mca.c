/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>

/* Check the Machine Check Architecture Extension registers */
void check_mca(void)
{
	/* TODO: Implement MCAX register checking and BERT table generation. */

	/* mca_clear_status uses the MCA registers and not the MCAX ones. Since they are
	   aliases, we can use either set of registers. */
	mca_clear_status();
}
