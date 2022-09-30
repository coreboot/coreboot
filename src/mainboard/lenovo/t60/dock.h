/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THINKPAD_T60_DOCK_H
#define THINKPAD_T60_DOCK_H

int dock_connect(void);
void dock_disconnect(void);
int dock_present(void);
int dlpc_init(void);

int legacy_io_present(void);
void legacy_io_init(void);
#endif
