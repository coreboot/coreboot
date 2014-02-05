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

#ifndef _CONSOLE_USB_H_
#define _CONSOLE_USB_H_

struct dbgp_pipe;

int usbdebug_init(void);

struct dbgp_pipe *dbgp_console_output(void);
struct dbgp_pipe *dbgp_console_input(void);
int dbgp_ep_is_active(struct dbgp_pipe *pipe);
int dbgp_bulk_write_x(struct dbgp_pipe *pipe, const char *bytes, int size);
int dbgp_bulk_read_x(struct dbgp_pipe *pipe, void *data, int size);
void usbdebug_tx_byte(struct dbgp_pipe *pipe, unsigned char data);
void usbdebug_tx_flush(struct dbgp_pipe *pipe);

#endif /* _CONSOLE_USB_H_ */
