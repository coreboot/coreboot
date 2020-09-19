/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "../qemu-i440fx/fw_cfg.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, 1, "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return 0;
}

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

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, "QEMU"),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
