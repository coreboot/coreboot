/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __VBOOT_VBOOT_COMMON_H__
#define __VBOOT_VBOOT_COMMON_H__

#include <commonlib/region.h>
#include <cbfs.h>
#include <vb2_api.h>

/*
 * Function to check if there is a request to enter recovery mode. Returns
 * reason code if request to enter recovery mode is present, otherwise 0.
 */
int vboot_check_recovery_request(void);

/* ============================ VBOOT REBOOT ============================== */
/*
 * vboot_reboot handles the reboot requests made by vboot_reference library. It
 * allows the platform to run any preparation steps before the reboot and then
 * does a hard reset.
 */
void vboot_reboot(void);

/*
 * Save vboot data and reboot device. Subcode will only be printed. To store
 * failure reason and subcode vb2api_fail() should be called before this
 * function or vboot_fail_and_reboot() should be used instead.
 */
void vboot_save_and_reboot(struct vb2_context *ctx, uint8_t subcode);

/* Call vb2api_fail() with reason and subcode, save vboot data and reboot. */
void vboot_fail_and_reboot(struct vb2_context *ctx, uint8_t reason, uint8_t subcode);

/* Allow the platform to do any clean up work when vboot requests a reboot. */
void vboot_platform_prepare_reboot(void);

/* ============================ VBOOT RESUME ============================== */
/*
 * Save the provided hash digest to a secure location to check against in
 * the resume path. Returns 0 on success, < 0 on error.
 */
int vboot_save_hash(void *digest, size_t digest_size);

/*
 * Retrieve the previously saved hash digest.  Returns 0 on success,
 * < 0 on error.
 */
int vboot_retrieve_hash(void *digest, size_t digest_size);

/* ============================= VERSTAGE ================================== */
/*
 * Main logic for verified boot. verstage_main() is just the core vboot logic.
 * If the verstage is a separate stage, it should be entered via main().
 */
void verstage_main(void);
void verstage_mainboard_early_init(void);
void verstage_mainboard_init(void);

/* Check boot modes */
#if CONFIG(VBOOT) && !ENV_SMM
int vboot_developer_mode_enabled(void);
int vboot_recovery_mode_enabled(void);
int vboot_can_enable_udc(void);
void vboot_run_logic(void);
const struct cbfs_boot_device *vboot_get_cbfs_boot_device(void);
#else /* !CONFIG_VBOOT */
static inline int vboot_developer_mode_enabled(void) { return 0; }
static inline int vboot_recovery_mode_enabled(void) { return 0; }
/* If VBOOT is not enabled, we are okay enabling USB device controller (UDC). */
static inline int vboot_can_enable_udc(void) { return 1; }
static inline void vboot_run_logic(void) {}
static inline const struct cbfs_boot_device *vboot_get_cbfs_boot_device(void)
{
	return NULL;
}
#endif

void vboot_save_data(struct vb2_context *ctx);

/*
 * The API for performing EC software sync.  Does not support
 * "slow" updates or Auxiliary FW sync.
 */
void vboot_sync_ec(void);

#endif /* __VBOOT_VBOOT_COMMON_H__ */
