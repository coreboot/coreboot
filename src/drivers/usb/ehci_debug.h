/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef _EHCI_DEBUG_H_
#define _EHCI_DEBUG_H_

void usbdebug_re_enable(unsigned ehci_base);
void usbdebug_disable(void);

/* Returns 0 on success and sets MMIO base and dbg_offset if EHCI debug
 * capability was found and enabled. Returns non-zero on error.
 */
int ehci_debug_hw_enable(unsigned *base, unsigned *dbg_offset);
void ehci_debug_select_port(unsigned int port);

#define DBGP_EP_VALID		(1<<0)
#define DBGP_EP_ENABLED		(1<<1)
#define DBGP_EP_BUSY		(1<<2)
#define DBGP_EP_STATMASK	(DBGP_EP_VALID | DBGP_EP_ENABLED)

struct dbgp_pipe
{
	u8 devnum;
	u8 endpoint;
	u8 pid;
	u8 status;
	int timeout;

	u8 bufidx;
	u8 buflen;
	char buf[8];
};

void dbgp_put(struct dbgp_pipe *pipe);
int dbgp_try_get(struct dbgp_pipe *pipe);

#endif /* _EHCI_DEBUG_H_ */
