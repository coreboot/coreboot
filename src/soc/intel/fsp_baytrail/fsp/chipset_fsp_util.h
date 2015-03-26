/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2014 Intel Corporation
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
 * Foundation, Inc.
 */

#ifndef CHIPSET_FSP_UTIL_H
#define CHIPSET_FSP_UTIL_H

#include <fsp.h>
#include <fsptypes.h>
#include <fspfv.h>
#include <fspffs.h>
#include <fspapi.h>
#include <fspplatform.h>
#include <fspinfoheader.h>
#include <fsphob.h>
#include <fspvpd.h>
#include <azalia.h>

#define FSP_INFO_HEADER_GUID \
  { \
  0x912740BE, 0x2284, 0x4734, {0xB9, 0x71, 0x84, 0xB0, 0x27, 0x35, 0x3F, 0x0C} \
  }

/*
 * The FSP Image ID is different for each platform's FSP and
 * can be used to verify that the right FSP binary is loaded.
 * For the Bay Trail FSP, the Image Id is "VLYVIEW0".
 */
#define FSP_IMAGE_ID_DWORD0 0x56594C56	/* 'VLYV' */
#define FSP_IMAGE_ID_DWORD1 0x30574549	/* 'IEW0' */

/* Revision of the FSP binary */
#define FSP_GOLD3_REV_ID    0x00000303

#define NO_DECREMENT_FOR_DEFAULT	0
#define DECREMENT_FOR_DEFAULT		1

#define UPD_MEMDOWN_CHECK(member, adjust) \
	if (config->member != UPD_DEFAULT) { \
		UpdData->PcdMemoryParameters.member = config->member - adjust; \
	}

#endif /* CHIPSET_FSP_UTIL_H */
