/*
 * This file is part of the coreboot project.
 *
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

/*
 * Include the FSP binary interface files
 *
 * These files include the necessary UEFI constants and data structures
 * that are used to interface to the FSP binary.
 */

#include <uefi_types.h>				/* UEFI data types */
#include <IntelFspPkg/Include/FspApi.h>		/* FSP API definitions */
#include <IntelFspPkg/Include/FspInfoHeader.h>	/* FSP binary layout */
#include <MdePkg/Include/Pi/PiBootMode.h>	/* UEFI boot mode definitions */
#include <MdePkg/Include/Pi/PiFirmwareFile.h>	/* UEFI file definitions */
#include <MdePkg/Include/Pi/PiFirmwareVolume.h>	/* UEFI file system defs */
#include <MdePkg/Include/Uefi/UefiMultiPhase.h>	/* UEFI memory types */
#include <MdePkg/Include/Pi/PiHob.h>		/* Hand off block definitions */
#include <MdePkg/Include/Library/HobLib.h>	/* HOB routine declarations */
#include <FspUpdVpd.h>		/* Vital/updatable product data definitions */

#endif	/* CHIPSET_FSP_UTIL_H */
