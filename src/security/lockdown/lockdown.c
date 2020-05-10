/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot_device.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <bootstate.h>
#include <fmap.h>

/*
 * Enables read- /write protection of the bootmedia.
 */
void boot_device_security_lockdown(void)
{
	const struct region_device *rdev = NULL;
	struct region_device dev;
	enum bootdev_prot_type lock_type;

	printk(BIOS_DEBUG, "BM-LOCKDOWN: Enabling boot media protection scheme ");

	if (CONFIG(BOOTMEDIA_LOCK_CONTROLLER)) {
		if (CONFIG(BOOTMEDIA_LOCK_WHOLE_RO)) {
			printk(BIOS_DEBUG, "'readonly'");
			lock_type = CTRLR_WP;
		} else if (CONFIG(BOOTMEDIA_LOCK_WHOLE_NO_ACCESS)) {
			printk(BIOS_DEBUG, "'no access'");
			lock_type = CTRLR_RWP;
		} else if (CONFIG(BOOTMEDIA_LOCK_WPRO_VBOOT_RO)) {
			printk(BIOS_DEBUG, "'WP_RO only'");
			lock_type = CTRLR_WP;
		}
		printk(BIOS_DEBUG, "using CTRL...\n");
	} else {
		if (CONFIG(BOOTMEDIA_LOCK_WHOLE_RO)) {
			printk(BIOS_DEBUG, "'readonly'");
			lock_type = MEDIA_WP;
		} else if (CONFIG(BOOTMEDIA_LOCK_WPRO_VBOOT_RO)) {
			printk(BIOS_DEBUG, "'WP_RO only'");
			lock_type = MEDIA_WP;
		}
		printk(BIOS_DEBUG, "using flash chip...\n");
	}

	if (CONFIG(BOOTMEDIA_LOCK_WPRO_VBOOT_RO)) {
		if (fmap_locate_area_as_rdev("WP_RO", &dev) < 0)
			printk(BIOS_ERR, "BM-LOCKDOWN: Could not find region 'WP_RO'\n");
		else
			rdev = &dev;
	} else {
		rdev = boot_device_ro();
	}

	if (rdev && boot_device_wp_region(rdev, lock_type) >= 0)
		printk(BIOS_INFO, "BM-LOCKDOWN: Enabled bootmedia protection\n");
	else
		printk(BIOS_ERR, "BM-LOCKDOWN: Failed to enable bootmedia protection\n");
}

static void lock(void *unused)
{
	boot_device_security_lockdown();
}

/*
 * Keep in sync with mrc_cache.c
 */

#if CONFIG(MRC_WRITE_NV_LATE)
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_EXIT, lock, NULL);
#else
BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, lock, NULL);
#endif
