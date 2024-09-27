/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <soc/intel/common/reset.h>

void do_global_reset(void)
{
	do_full_reset();
}
