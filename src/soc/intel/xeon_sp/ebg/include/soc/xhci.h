/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XHCI_H_
#define _XHCI_H_

#include <types.h>

#define USB2_OC_PIN_0 0x90A4
#define USB2_OC_PIN_1 0x90A8
#define USB2_OC_PIN_2 0x90AC
#define USB2_OC_PIN_3 0x90B0
#define USB2_OC_PIN_4 0x90B4
#define USB2_OC_PIN_5 0x90B8
#define USB2_OC_PIN_6 0x90BC
#define USB2_OC_PIN_7 0x90C0

#define USB3_OC_PIN_0 0x9124
#define USB3_OC_PIN_1 0x9128
#define USB3_OC_PIN_2 0x912C
#define USB3_OC_PIN_3 0x9130
#define USB3_OC_PIN_4 0x9134
#define USB3_OC_PIN_5 0x9138
#define USB3_OC_PIN_6 0x913C
#define USB3_OC_PIN_7 0x9140

#define USB_PORT_0 BIT(0)
#define USB_PORT_1 BIT(1)
#define USB_PORT_2 BIT(2)
#define USB_PORT_3 BIT(3)
#define USB_PORT_4 BIT(4)
#define USB_PORT_5 BIT(5)
#define USB_PORT_6 BIT(6)
#define USB_PORT_7 BIT(7)
#define USB_PORT_8 BIT(8)
#define USB_PORT_9 BIT(9)
#define USB_PORT_10 BIT(10)
#define USB_PORT_11 BIT(11)
#define USB_PORT_12 BIT(12)
#define USB_PORT_13 BIT(13)
#define USB_PORT_14 BIT(14)
#define USB_PORT_15 BIT(15)
#define USB_PORT_NONE 0
#define OCCFGDONE BIT(31)

struct usb_oc_mapping {
	uint32_t pin;
	uint32_t port;
};

void write_usb_oc_mapping(const struct usb_oc_mapping *config, uint8_t pins);
void lock_oc_cfg(bool lock);

#endif /* _XHCI_H_ */
