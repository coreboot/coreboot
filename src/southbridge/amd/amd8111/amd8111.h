/*
 * This file is part of the coreboot project.
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

#ifndef AMD8111_H
#define AMD8111_H

#include <device/device.h>

void amd8111_enable(struct device *dev);
void enable_fid_change_on_sb(unsigned int sbbusn, unsigned int sbdn);

#endif /* AMD8111_H */
