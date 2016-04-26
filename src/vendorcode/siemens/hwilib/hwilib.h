/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
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

#ifndef SIEMENS_HWI_LIB_H_
#define SIEMENS_HWI_LIB_H_

#include <types.h>


/* Declare all supported fields which can be get with hwilib. */
typedef enum {
	HIB_VerID = 0,
	SIB_VerID,
	EIB_VerID,
	HIB_HwRev,
	SIB_HwRev,
	UniqueNum,
	Mac1,
	Mac1Aux,
	Mac2,
	Mac2Aux,
	Mac3,
	Mac3Aux,
	Mac4,
	Mac4Aux,
	SPD,
	FF_FreezeDis,
	FF_FanReq,
	BiosFlags,
	MacMapping1,
	MacMapping2,
	MacMapping3,
	MacMapping4,
	PF_Color_Depth,
	PF_DisplType,
	PF_DisplCon,
	Edid,
	VddRef
} hwinfo_field_t;

/* Define used values in supported fields */
#define PF_COLOR_DEPTH_6BIT	0x00
#define PF_COLOR_DEPTH_8BIT	0x01
#define PF_COLOR_DEPTH_10BIT	0x02
#define PF_DISPLCON_LVDS_SINGLE	0x00
#define PF_DISPLCON_LVDS_DUAL	0x05
#define FF_FREEZE_DISABLE	0x01
#define FF_FAN_NEEDED		0x01

/* Use this function to find all supported blocks in cbfs. It must be called
 * once with a valid cbfs file name before hwilib_get_field() can be used.
 */
enum cb_err hwilib_find_blocks (const char *hwi_filename);

/* Use this function to get fields out of supported info blocks
 * This function returns the number of copied bytes or 0 on error.
 */
uint32_t hwilib_get_field (hwinfo_field_t field, uint8_t *data, uint32_t maxlen);

/* This functions needs to be implemented for every mainboard that uses i210. */
enum cb_err mainboard_get_mac_address(uint16_t bus, uint8_t devfn,
					uint8_t mac[6]);
#endif /* SIEMENS_HWI_LIB_H_ */
