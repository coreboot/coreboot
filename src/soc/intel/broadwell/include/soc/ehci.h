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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

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
