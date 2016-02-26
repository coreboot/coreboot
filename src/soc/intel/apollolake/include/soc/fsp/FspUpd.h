/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP_API_H_
#define _FSP_API_H_

/** FSP UPD Header
**/
struct FSP_UPD_HEADER {

/** Offset 0x00 to 0x07 - UPD Region Signature
  The signature will be
  "FSPT_UPD" for FSP-T
  "FSPM_UPD" for FSP-M
  "FSPS_UPD" for FSP-S
**/
  uint64_t                      Signature;

/** Offset 0x08 - Revision
**/
  uint8_t                       Revision;

/** Offset 0x09 to 0x1F - ReservedUpd
**/
  uint8_t                       ReservedUpd[23];
} __attribute__((packed));

#endif /* _FSP_API_H_ */
