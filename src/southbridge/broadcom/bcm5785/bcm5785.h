/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#ifndef BCM5785_H
#define BCM5785_H

#include "chip.h"

#ifndef __PRE_RAM__
void bcm5785_enable(struct device *dev);
#else
void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn);
#endif

void ldtstop_sb(void);

#endif /* BCM5785_H */
