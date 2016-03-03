/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#ifndef _QUARK_PCI_DEVS_H_
#define _QUARK_PCI_DEVS_H_

#include <arch/io.h>
#include <device/pci.h>
#include <soc/QuarkNcSocId.h>

/* DEVICE 0 (Memroy Controller Hub) */
#define MC_BDF		PCI_DEV(PCI_BUS_NUMBER_QNC, MC_DEV, MC_FUN)

/* IO Fabric 1 */
#define HSUART_DEVID	0x0936

#define SIO1_DEV 0x14
# define HSUART1_DEV SIO1_DEV
# define HSUART1_FUNC 5

/* Platform Controller Unit */
# define LPC_DEV_FUNC	PCI_DEVFN(PCI_DEVICE_NUMBER_QNC_LPC, \
				PCI_FUNCTION_NUMBER_QNC_LPC)

#endif /* _QUARK_PCI_DEVS_H_ */
