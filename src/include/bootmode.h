/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BOOTMODE_H__
#define __BOOTMODE_H__

#include <stdbool.h>

/* functions implemented per mainboard: */
void init_bootmode_straps(void);
int get_write_protect_state(void);
int get_recovery_mode_switch(void);
int get_recovery_mode_retrain_switch(void);
int clear_recovery_mode_switch(void);
int get_wipeout_mode_switch(void);
int get_lid_switch(void);
int get_ec_is_trusted(void);
bool mainboard_ec_running_ro(void);

/* Return 1 if display initialization is required. 0 if not. */
int display_init_required(void);
int gfx_get_init_done(void);
void gfx_set_init_done(int done);

/*
 * Determine if the platform is resuming from suspend. Returns 0 when
 * not resuming, > 0 if resuming, and < 0 on error.
 */
int platform_is_resuming(void);

#endif /* __BOOTMODE_H__ */
