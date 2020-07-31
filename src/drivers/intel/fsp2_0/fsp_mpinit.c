/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <intelblocks/mp_init.h>

/*
 * As per FSP integration guide:
 * If bootloader needs to take control of APs back, a full AP re-initialization is
 * required after FSP-S is completed and control has been transferred back to bootloader
 */
void do_mpinit_after_fsp(void)
{
	init_cpus();
}
