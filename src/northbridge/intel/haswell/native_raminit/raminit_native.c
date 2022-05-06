/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <northbridge/intel/haswell/raminit.h>

void perform_raminit(const int s3resume)
{
	/*
	 * See, this function's name is a lie. There are more things to
	 * do that memory initialisation, but they are relatively easy.
	 */

	/** TODO: Implement the required magic **/
	die("NATIVE RAMINIT: More Magic (tm) required.\n");
}
