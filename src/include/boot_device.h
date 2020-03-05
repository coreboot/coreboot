/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BOOT_DEVICE_H_
#define _BOOT_DEVICE_H_

#include <commonlib/region.h>

/*
 * Boot device region can be protected by 2 sources, media and controller.
 * The following modes are identified. It depends on the flash chip and the
 * controller if mode is actually supported.
 *
 * MEDIA_WP : Flash/Boot device enforces write protect
 * CTRLR_WP : Controller device enforces write protect
 * CTRLR_RP : Controller device enforces read protect
 * CTRLR_RWP : Controller device enforces read-write protect
 */
enum bootdev_prot_type {
	CTRLR_WP = 1,
	CTRLR_RP = 2,
	CTRLR_RWP = 3,
	MEDIA_WP = 4,
};
/*
 * Please note that the read-only boot device may not be coherent with
 * the read-write boot device. Thus, mixing mmap() and writeat() is
 * most likely not to work so don't rely on such semantics.
 */

/* Return the region_device for the read-only boot device. This is the root
   device for all CBFS boot devices. */
const struct region_device *boot_device_ro(void);

/* Return the region_device for the read-write boot device. */
const struct region_device *boot_device_rw(void);

/*
 * Create a sub-region of the read-only boot device.
 * Returns 0 on success, < 0 on error.
 */
int boot_device_ro_subregion(const struct region *sub,
				struct region_device *subrd);

/*
 * Create a sub-region of the read-write boot device.
 * Returns 0 on success, < 0 on error.
 */
int boot_device_rw_subregion(const struct region *sub,
				struct region_device *subrd);

/*
 * Write protect a sub-region of the boot device represented
 * by the region device.
 * Returns 0 on success, < 0 on error.
 */
int boot_device_wp_region(const struct region_device *rd,
				const enum bootdev_prot_type type);

/*
 * Initialize the boot device. This may be called multiple times within
 * a stage so boot device implementations should account for this behavior.
 **/
void boot_device_init(void);

/*
 * Restrict read/write access to the bootmedia using platform defined rules.
 */
#if CONFIG(BOOTMEDIA_LOCK_NONE) || (CONFIG(BOOTMEDIA_LOCK_IN_VERSTAGE) && ENV_RAMSTAGE)
static inline void boot_device_security_lockdown(void) {}
#else
void boot_device_security_lockdown(void);
#endif
#endif /* _BOOT_DEVICE_H_ */
