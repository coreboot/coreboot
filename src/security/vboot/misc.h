/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __VBOOT_MISC_H__
#define __VBOOT_MISC_H__

#include <assert.h>
#include <security/vboot/vboot_common.h>
#include <vb2_api.h>

/*
 * Source: security/vboot/common.c
 */
struct vb2_context *vboot_get_context(void);

/*
 * Returns 1 if firmware slot A is used, 0 if slot B is used.
 */
static inline int vboot_is_firmware_slot_a(struct vb2_context *ctx)
{
	return !(ctx->flags & VB2_CONTEXT_FW_SLOT_B);
}

/*
 * Check if given flag is set in the flags field in GBB header.
 * Return value:
 * true: Flag is set.
 * false: Flag is not set.
 */
static inline bool vboot_is_gbb_flag_set(enum vb2_gbb_flag flag)
{
	return !!(vb2api_gbb_get_flags(vboot_get_context()) & flag);
}

/*
 * Locates firmware as a region device. Returns 0 on success, -1 on failure.
 */
int vboot_locate_firmware(struct vb2_context *ctx, struct region_device *fw);

/*
 * The stage loading code is compiled and entered from multiple stages. The
 * helper functions below attempt to provide more clarity on when certain
 * code should be called. They are implemented inline for better compile-time
 * code elimination.
 */

static inline int verification_should_run(void)
{
	if (CONFIG(VBOOT_SEPARATE_VERSTAGE))
		return ENV_SEPARATE_VERSTAGE;
	else if (CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		return ENV_ROMSTAGE;
	else if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK))
		return ENV_BOOTBLOCK;
	else
		dead_code();
}

static inline int verstage_should_load(void)
{
	if (CONFIG(VBOOT_SEPARATE_VERSTAGE) && !CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		return ENV_BOOTBLOCK;
	else
		return 0;
}

static inline int vboot_logic_executed(void)
{
	extern int vboot_executed;	/* should not be globally accessible */

	/* If we are in the stage that runs verification, or in the stage that
	   both loads the verstage and is returned to from it afterwards, we
	   need to check a global to see if verification has run. */
	if (verification_should_run() ||
	    (verstage_should_load() && CONFIG(VBOOT_RETURN_FROM_VERSTAGE)))
		return vboot_executed;

	if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)) {
		/* All other stages are "after the bootblock" */
		return !ENV_BOOTBLOCK;
	} else if (CONFIG(VBOOT_STARTS_IN_ROMSTAGE)) {
		/* Post-RAM stages are "after the romstage" */
		return !ENV_ROMSTAGE_OR_BEFORE;
	} else if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		return !ENV_SEPARATE_VERSTAGE;
	} else {
		dead_code();
	}
}

static inline bool vboot_hwcrypto_allowed(void)
{
	/* When not using vboot firmware verification, HW crypto is always allowed. */
	if (!CONFIG(VBOOT))
		return 1;

	/* Before vboot runs we can't check for HW crypto, so err on the side of caution. */
	if (!vboot_logic_executed())
		return 0;

	/* Otherwise, vboot can decide. */
	return vb2api_hwcrypto_allowed(vboot_get_context());
}

#endif /* __VBOOT_MISC_H__ */
