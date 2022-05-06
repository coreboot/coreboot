/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <types.h>

static bool early_init_native(int s3resume)
{
	printk(BIOS_DEBUG, "Starting native platform initialisation\n");

	if (!CONFIG(INTEL_LYNXPOINT_LP))
		dmi_early_init();

	return false;
}

void perform_raminit(const int s3resume)
{
	/*
	 * See, this function's name is a lie. There are more things to
	 * do that memory initialisation, but they are relatively easy.
	 */
	const bool cpu_replaced = early_init_native(s3resume);

	(void)cpu_replaced;

	/** TODO: Implement the required magic **/
	die("NATIVE RAMINIT: More Magic (tm) required.\n");
}
