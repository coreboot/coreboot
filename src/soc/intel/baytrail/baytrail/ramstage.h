/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_RAMSTAGE_H_
#define _BAYTRAIL_RAMSTAGE_H_

#include <device/device.h>

/* The baytrail_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized. */
void baytrail_init_pre_device(void);
void set_max_freq(void);

extern struct pci_operations soc_pci_ops;

#endif /* _BAYTRAIL_RAMSTAGE_H_ */
