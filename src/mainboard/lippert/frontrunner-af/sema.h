/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __LIPPERT_SEMA_H__
#define __LIPPERT_SEMA_H__

/* Signal SEMA watchdog a successful boot.
 * Returns < 0 is SMBus message failed after
 * several retries.
 */
int sema_send_alive(void);

#endif
