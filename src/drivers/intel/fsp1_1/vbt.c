/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootmode.h>
#include <console/console.h>
#include <drivers/intel/gma/opregion.h>
#include <fsp/ramstage.h>
#include <fsp/util.h>
#include <lib.h>

/* Locate VBT and pass it to FSP GOP */
void load_vbt(SILICON_INIT_UPD *params)
{
	const optionrom_vbt_t *vbt_data = NULL;
	size_t vbt_len;

	/* Check boot mode - for S3 resume path VBT loading is not needed */
	if (acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "S3 resume do not pass VBT to GOP\n");
	} else if (display_init_required()) {
		/* Get VBT data */
		vbt_data = locate_vbt(&vbt_len);
		if (vbt_data != NULL) {
			if (CONFIG(DISPLAY_VBT)) {
				/* Display the vbt file contents */
				printk(BIOS_DEBUG, "VBT Data:\n");
				hexdump(vbt_data, vbt_len);
				printk(BIOS_DEBUG, "\n");
			}
			printk(BIOS_DEBUG, "Passing VBT to GOP\n");
		} else {
			printk(BIOS_DEBUG, "VBT not found!\n");
		}
	} else {
		printk(BIOS_DEBUG, "Not passing VBT to GOP\n");
	}
	params->GraphicsConfigPtr = (u32)vbt_data;
}
