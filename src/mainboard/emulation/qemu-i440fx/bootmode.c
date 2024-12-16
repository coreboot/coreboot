/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmode.h>
#include <console/console.h>
#include <device/fw_cfg.h>

/*
 * Enable recovery mode with fw_cfg option to qemu:
 *   -fw_cfg name=opt/cros/recovery,string=1
 */
int get_recovery_mode_switch(void)
{
	FWCfgFile f;

	if (!fw_cfg_check_file(&f, "opt/cros/recovery")) {
		uint8_t rec_mode;
		if (f.size != 1) {
			printk(BIOS_ERR, "opt/cros/recovery invalid size %d\n", f.size);
			return 0;
		}
		fw_cfg_get(f.select, &rec_mode, f.size);
		if (rec_mode == '1') {
			printk(BIOS_INFO, "Recovery is enabled.\n");
			return 1;
		}
	}

	return 0;
}
