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

#include <console/streams.h>

int usbdebug_init(void);

void usb_tx_byte(int idx, unsigned char data);
void usb_tx_flush(int idx);
unsigned char usb_rx_byte(int idx);
int usb_can_rx_byte(int idx);

#define __CONSOLE_USB_ENABLE__	CONFIG_CONSOLE_USB && \
	(ENV_ROMSTAGE && CONFIG_USBDEBUG_IN_ROMSTAGE || ENV_RAMSTAGE)

#if __CONSOLE_USB_ENABLE__
#define __usbdebug_init()	usbdebug_init()
#define __usb_tx_byte(x)	usb_tx_byte(0, x)
#define __usb_tx_flush()	usb_tx_flush(0)
#else
#define __usbdebug_init()
#define __usb_tx_byte(x)
#define __usb_tx_flush()
#endif

#endif /* _CONSOLE_USB_H_ */
