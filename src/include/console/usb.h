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
 */

#ifndef _CONSOLE_USB_H_
#define _CONSOLE_USB_H_

#include <rules.h>
#include <stdint.h>

int usbdebug_init(void);

void usb_tx_byte(int idx, unsigned char data);
void usb_tx_flush(int idx);
unsigned char usb_rx_byte(int idx);
int usb_can_rx_byte(int idx);

#define __CONSOLE_USB_ENABLE__	CONFIG_CONSOLE_USB && \
	((ENV_ROMSTAGE && CONFIG_USBDEBUG_IN_ROMSTAGE) || ENV_RAMSTAGE)

#define USB_PIPE_FOR_CONSOLE 0
#define USB_PIPE_FOR_GDB 0

#if __CONSOLE_USB_ENABLE__
static inline void __usbdebug_init(void)	{ usbdebug_init(); }
static inline void __usb_tx_byte(u8 data)	{ usb_tx_byte(USB_PIPE_FOR_CONSOLE, data); }
static inline void __usb_tx_flush(void)	{ usb_tx_flush(USB_PIPE_FOR_CONSOLE); }
#else
static inline void __usbdebug_init(void)	{}
static inline void __usb_tx_byte(u8 data)	{}
static inline void __usb_tx_flush(void)	{}
#endif

/*  */
#if 0 && CONFIG_GDB_STUB && \
	((ENV_ROMSTAGE && CONFIG_USBDEBUG_IN_ROMSTAGE) || ENV_RAMSTAGE)
static inline void __gdb_hw_init(void)	{ usbdebug_init(); }
static inline void __gdb_tx_byte(u8 data)	{ usb_tx_byte(USB_PIPE_FOR_GDB, data); }
static inline void __gdb_tx_flush(void)	{ usb_tx_flush(USB_PIPE_FOR_GDB); }
static inline u8 __gdb_rx_byte(void)		{ return usb_rx_byte(USB_PIPE_FOR_GDB); }
#endif

#endif /* _CONSOLE_USB_H_ */
