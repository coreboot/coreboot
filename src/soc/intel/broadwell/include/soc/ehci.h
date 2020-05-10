/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_EHCI_H_
#define _BROADWELL_EHCI_H_

/* EHCI Memory Registers */
#define EHCI_USB_CMD		0x20
#define  EHCI_USB_CMD_RUN	(1 << 0)
#define  EHCI_USB_CMD_PSE	(1 << 4)
#define  EHCI_USB_CMD_ASE	(1 << 5)
#define EHCI_PORTSC(port)	(0x64 + (port * 4))
#define  EHCI_PORTSC_ENABLED	(1 << 2)
#define  EHCI_PORTSC_SUSPEND	(1 << 7)

#endif
