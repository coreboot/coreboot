/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CONSOLE_USB_H_
#define _CONSOLE_USB_H_

#include <types.h>

void usbdebug_init(void);
int usbdebug_hw_init(bool force);

void usb_tx_byte(int idx, unsigned char data);
void usb_tx_flush(int idx);
unsigned char usb_rx_byte(int idx);
int usb_can_rx_byte(int idx);

#define __CONSOLE_USB_ENABLE__	(CONFIG(CONSOLE_USB) && \
	((ENV_BOOTBLOCK && CONFIG(USBDEBUG_IN_PRE_RAM)) || \
	 (ENV_ROMSTAGE && CONFIG(USBDEBUG_IN_PRE_RAM)) || \
	 (ENV_POSTCAR && CONFIG(USBDEBUG_IN_PRE_RAM)) || \
	 (ENV_SEPARATE_VERSTAGE && CONFIG(USBDEBUG_IN_PRE_RAM)) || \
	 ENV_RAMSTAGE))

#define USB_PIPE_FOR_CONSOLE 0
#define USB_PIPE_FOR_GDB 0

#if __CONSOLE_USB_ENABLE__
static inline void __usbdebug_init(void)	{ usbdebug_init(); }
static inline void __usb_tx_byte(u8 data)
{
	usb_tx_byte(USB_PIPE_FOR_CONSOLE, data);
}
static inline void __usb_tx_flush(void)	{ usb_tx_flush(USB_PIPE_FOR_CONSOLE); }
#else
static inline void __usbdebug_init(void)	{}
static inline void __usb_tx_byte(u8 data)	{}
static inline void __usb_tx_flush(void)	{}
#endif

#endif /* _CONSOLE_USB_H_ */
