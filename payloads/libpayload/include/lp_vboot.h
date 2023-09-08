/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _LP_VBOOT_H_
#define _LP_VBOOT_H_

#include <vb2_api.h>

struct vb2_context *vboot_get_context(void);

/*
 * Call vb2api_fail() with reason and subcode, save vboot data with vb2ex_commit_data()
 * and reboot with vboot_reboot().
 */
void vboot_fail_and_reboot(struct vb2_context *ctx, uint8_t reason, uint8_t subcode);

/* Returns non-zero if recovery mode is enabled. */
int vboot_recovery_mode_enabled(void);

#endif /* _LP_VBOOT_H_ */
