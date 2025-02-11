/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _INTEL_COMMON_RESET_H_
#define _INTEL_COMMON_RESET_H_

#include <efi/efi_datatype.h>

/*
 * Implement SoC specific global reset (i.e. a reset of both host and
 * ME partitions). Usually the ME is asked to perform the reset first.
 * If that doesn't work out, fall back to a manual global reset.
 */
void do_global_reset(void);

/* Prepare for reset, run do_global_reset(), halt. */
__noreturn void global_reset(void);

/*
 * Return PCH Reset Status
 * The return status can be between EfiResetCold, EfiResetWarm, EfiResetShutdown
 * or EfiResetPlatformSpecific.
 *
 * If reset type if `EfiResetPlatformSpecific` then relying on pch_reset_data structure
 * to know if the reset type is a global reset.
 */
efi_return_status_t fsp_get_pch_reset_status(void);

/*
 * Issue power off due to low battery
 *
 * Call this function to power off the platform if the battery level is critically low.
 */
void do_low_battery_poweroff(void);

#endif	/* _INTEL_COMMON_RESET_H_ */
