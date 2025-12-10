/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_RPMH_H__
#define __SOC_QUALCOMM_RPMH_H__

#include <device/device.h>
#include <soc/tcs.h>

int rpmh_write(enum rpmh_state state, const struct tcs_cmd *cmd, u32 n);

int rpmh_write_async(enum rpmh_state state, const struct tcs_cmd *cmd, u32 n);

int rpmh_write_batch(enum rpmh_state state, const struct tcs_cmd *cmd, u32 *n);

void rpmh_invalidate(void);

int rpmh_write_sleep_and_wake(void);

int rpmh_mode_solver_set(bool enable);

int rpmh_init_fast_path(struct tcs_cmd *cmd, int n);

int rpmh_update_fast_path(struct tcs_cmd *cmd, int n, u32 update_mask);

#endif /* __SOC_QUALCOMM_RPMH_H__ */
