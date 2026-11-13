/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CRESTLINE_THINKPAD_DOCK_H
#define CRESTLINE_THINKPAD_DOCK_H

#include <types.h>

/* Dock and Ultrabay code, implemented by the variants */
void dock_early_init(void);
int dock_connect(void);
void dock_disconnect(void);
int dock_present(void);
int dlpc_init(void);
int ultrabay_device_present(void);
void ultrabay_power_enable(bool enable);
#endif
