/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook, Inc.
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

/*
 * As per "Intel Xeon Processor D-1500 Product Family" volume 2,
 * "The UBOX [Processor Utility Box] is the piece of processor logic that deals with
 * the non mainstream flows in the system. This includes transactions like the register
 * accesses, interrupt flows, lock flows and events. In addition, the UBOX houses
 * coordination for the performance architecture, and also houses scratchpad and
 * semaphore registers."
 *
 * In other words, this is a one-die block that has all the useful magic registers.
*/

#ifndef _BROADWELL_UBOX_H_
#define _BROADWELL_UBOX_H_

#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/vtd.h>

#define UBOX_UART_ENABLE		0xf8
#define   UBOX_UART_ENABLE_PORT0	(1u << 0)
#define   UBOX_UART_ENABLE_PORT1	(1u << 1)

#define UBOX_SC_RESET_STATUS		0xc8
#define   UBOX_SC_BYPASS		(1u << 3)

#define UBOX_DEVHIDE0			0xb0

void iio_hide(DEVTREE_CONST struct device *dev);
#endif
