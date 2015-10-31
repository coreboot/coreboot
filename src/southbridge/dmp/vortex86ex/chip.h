/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

#ifndef _SOUTHBRIDGE_DMP_VORTEX86EX
#define _SOUTHBRIDGE_DMP_VORTEX86EX

struct southbridge_dmp_vortex86ex_config {
	/* PCI function enables */
	/* i.e. so that pci scan bus will find them. */
	/* I am putting in IDE as an example but obviously this needs
	 * to be more complete!
	 */
	int enable_ide;
	/* enables of functions of devices */
	int enable_usb;
	int enable_native_ide;
	int enable_com_ports;
	int enable_keyboard;
	int enable_nvram;
};

#endif				/* _SOUTHBRIDGE_DMP_VORTEX86EX */
