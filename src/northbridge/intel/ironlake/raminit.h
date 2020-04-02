/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
