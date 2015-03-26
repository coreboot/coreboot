/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Eric Biederman (ebiederm@xmission.com)
 * Copyright (C) 2007 AMD
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
 * Foundation, Inc.
 */

#include <console/usb.h>
#include "ehci_debug.h"

static void usbdebug_tx_byte(struct dbgp_pipe *pipe, unsigned char data)
{
	if (!dbgp_try_get(pipe))
		return;
	pipe->buf[pipe->bufidx++] = data;
	if (pipe->bufidx >= 8) {
		dbgp_bulk_write_x(pipe, pipe->buf, pipe->bufidx);
		pipe->bufidx = 0;
	}
	dbgp_put(pipe);
}

static void usbdebug_tx_flush(struct dbgp_pipe *pipe)
{
	if (!dbgp_try_get(pipe))
		return;
	if (pipe->bufidx > 0) {
		dbgp_bulk_write_x(pipe, pipe->buf, pipe->bufidx);
		pipe->bufidx = 0;
	}
	dbgp_put(pipe);
}

static unsigned char usbdebug_rx_byte(struct dbgp_pipe *pipe)
{
	unsigned char data = 0xff;
	if (!dbgp_try_get(pipe))
		return 0xff;
	while (pipe->bufidx >= pipe->buflen) {
		pipe->buflen = 0;
		pipe->bufidx = 0;
		int count = dbgp_bulk_read_x(pipe, pipe->buf, 8);
		if (count>0)
			pipe->buflen = count;
	}
	data = pipe->buf[pipe->bufidx++];
	dbgp_put(pipe);
	return data;
}

void usb_tx_byte(int idx, unsigned char data)
{
	usbdebug_tx_byte(dbgp_console_output(), data);
}

void usb_tx_flush(int idx)
{
	usbdebug_tx_flush(dbgp_console_output());
}

unsigned char usb_rx_byte(int idx)
{
	return usbdebug_rx_byte(dbgp_console_input());
}

int usb_can_rx_byte(int idx)
{
	return dbgp_ep_is_active(dbgp_console_input());
}
