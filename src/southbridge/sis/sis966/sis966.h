/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SIS966_H
#define SIS966_H

#if CONFIG_HT_CHAIN_END_UNITID_BASE < CONFIG_HT_CHAIN_UNITID_BASE
#define SIS966_DEVN_BASE CONFIG_HT_CHAIN_END_UNITID_BASE
#else
#define SIS966_DEVN_BASE CONFIG_HT_CHAIN_UNITID_BASE
#endif

#define DEBUG_AZA 0
#define DEBUG_NIC 0
#define DEBUG_IDE 0
#define DEBUG_SATA 0
#define DEBUG_USB 0
#define DEBUG_USB2 0

#ifndef __SIMPLE_DEVICE__
void sis966_enable(struct device *dev);
#endif

#endif /* SIS966_H */
