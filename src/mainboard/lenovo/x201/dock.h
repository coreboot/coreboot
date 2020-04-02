/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THINKPAD_X201_DOCK_H
#define THINKPAD_X201_DOCK_H
void init_dock(void);
void dock_connect(void);
void dock_disconnect(void);
int dock_present(void);
#endif
