/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <types.h>

/* Validate the FSP-M header in romstage */
void soc_validate_fspm_header(const struct fsp_header *hdr)
{
	/* Check if the image fits into the reserved memory region */
	if (hdr->image_size > CONFIG_FSP_M_SIZE)
		die("The FSP-M binary is %u bytes larger than the memory region allocated for "
			"it. Increase FSP_M_SIZE to make it fit.\n",
			hdr->image_size - CONFIG_FSP_M_SIZE);
}
