/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LIPPERT_SEMA_H__
#define __LIPPERT_SEMA_H__

/* Signal SEMA watchdog a successful boot.
 * Returns < 0 is SMBus message failed after
 * several retries.
 */
int sema_send_alive(void);

#endif
