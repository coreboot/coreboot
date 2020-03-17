/*
 * This file is part of the coreboot project.
 *
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

#ifndef RAMINIT_H
#define RAMINIT_H

#include "ironlake.h"

void chipset_init(const int s3resume);
/* spd_addrmap is array of 4 elements:
   Channel 0 Slot 0
   Channel 0 Slot 1
   Channel 1 Slot 0
   Channel 1 Slot 1
   0 means "not present"
*/
void raminit(const int s3resume, const u8 *spd_addrmap);

#endif				/* RAMINIT_H */
