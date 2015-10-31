/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/* Number of all PCH devices */
#define PCH_MAX_DEV_INT_CONFIG 64

/* Number of PxRC register in ITSS */
#define PCH_PARC 0
#define PCH_PBRC 1
#define PCH_PCRC 2
#define PCH_PDRC 3
#define PCH_PERC 4
#define PCH_PFRC 5
#define PCH_PGRC 6
#define PCH_PHRC 7
#define PCH_MAX_IRQ_CONFIG 8

#define DEVICE_INT_CONFIG(dev, func, line, irqno) {\
	.Device = dev, \
	.Function = func, \
	.IntX = line, \
	.Irq = irqno }

#define no_int 0
#define int_A 1
#define int_B 2
#define int_C 3
#define int_D 4

#endif /* _INTERRUPT_H_ */
