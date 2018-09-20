/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_XHCI_H_
#define _BROADWELL_XHCI_H_

/* XHCI PCI Registers */
#define XHCI_PWR_CTL_STS	0x74
#define  XHCI_PWR_CTL_SET_MASK	0x3
#define  XHCI_PWR_CTL_SET_D0	0x0
#define  XHCI_PWR_CTL_SET_D3	0x3
#define  XHCI_PWR_CTL_ENABLE_PME (1 << 8)
#define  XHCI_PWR_CTL_STATUS_PME (1 << 15)
#define XHCI_USB2PR		0xd0
#define XHCI_USB2PRM		0xd4
#define  XHCI_USB2PR_HCSEL	0x7fff
#define XHCI_USB3PR		0xd8
#define  XHCI_USB3PR_SSEN	0x3f
#define XHCI_USB3PRM		0xdc
#define XHCI_USB3FUS		0xe0
#define  XHCI_USB3FUS_SS_MASK	3
#define  XHCI_USB3FUS_SS_SHIFT	3
#define XHCI_USB3PDO		0xe8

/* XHCI Memory Registers */
#define XHCI_USB3_PORTSC(port)	(0x530 + (port * 0x10))
#define  XHCI_USB3_PORTSC_CHST	(0x7f << 17)
#define  XHCI_USB3_PORTSC_WCE	(1 << 25)	/* Wake on Connect */
#define  XHCI_USB3_PORTSC_WDE	(1 << 26)	/* Wake on Disconnect */
#define  XHCI_USB3_PORTSC_WOE	(1 << 27)	/* Wake on Overcurrent */
#define  XHCI_USB3_PORTSC_WRC	(1 << 19)	/* Warm Reset Complete */
#define  XHCI_USB3_PORTSC_LWS	(1 << 16)	/* Link Write Strobe */
#define  XHCI_USB3_PORTSC_PED	(1 << 1)	/* Port Enabled/Disabled */
#define  XHCI_USB3_PORTSC_WPR	(1 << 31)	/* Warm Port Reset */
#define  XHCI_USB3_PORTSC_PLS	(0xf << 5)	/* Port Link State */
#define   XHCI_PLSR_DISABLED	(4 << 5)	/* Port is disabled */
#define   XHCI_PLSR_RXDETECT	(5 << 5)	/* Port is disconnected */
#define   XHCI_PLSR_POLLING	(7 << 5)	/* Port is polling */
#define   XHCI_PLSW_ENABLE	(5 << 5)	/* Transition from disabled */

#ifdef __SMM__
void usb_xhci_sleep_prepare(pci_devfn_t dev, u8 slp_typ);
#endif

#endif
