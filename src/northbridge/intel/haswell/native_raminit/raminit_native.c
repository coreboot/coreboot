/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

static bool early_init_native(int s3resume)
{
	printk(BIOS_DEBUG, "Starting native platform initialisation\n");

	intel_early_me_init();
	/** TODO: CPU replacement check must be skipped in warm boots and S3 resumes **/
	const bool cpu_replaced = !s3resume && intel_early_me_cpu_replacement_check();

	early_pch_init_native(s3resume);

	if (!CONFIG(INTEL_LYNXPOINT_LP))
		dmi_early_init();

	return cpu_replaced;
}

void perform_raminit(const int s3resume)
{
	/*
	 * See, this function's name is a lie. There are more things to
	 * do that memory initialisation, but they are relatively easy.
	 */
	const bool cpu_replaced = early_init_native(s3resume);

	(void)cpu_replaced;

	/** TODO: Move after raminit */
	if (intel_early_me_uma_size() > 0) {
		/** TODO: Update status once raminit is implemented **/
		uint8_t me_status = ME_INIT_STATUS_ERROR;
		intel_early_me_init_done(me_status);
	}

	intel_early_me_status();

	/** TODO: Implement the required magic **/
	die("NATIVE RAMINIT: More Magic (tm) required.\n");
}
