/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#ifndef _AGESAWRAPPER_H_
#define _AGESAWRAPPER_H_

#include <stdint.h>
#include <vendorcode/amd/agesa/f14/AGESA.h>


/**
 * Define AMD Ontario APU SSID/SVID
 */
#define AMD_APU_SVID		0x1022
#define AMD_APU_SSID		0x1234
#define PCIE_BASE_ADDRESS	 CONFIG_MMCONF_BASE_ADDRESS

enum {
	PICK_DMI,		/**< DMI Interface */
	PICK_PSTATE,	/**< Acpi Pstate SSDT Table */
	PICK_SRAT,		/**< SRAT Table */
	PICK_SLIT,		/**< SLIT Table */
	PICK_WHEA_MCE,	/**< WHEA MCE table */
	PICK_WHEA_CMC,	/**< WHEA CMV table */
	PICK_ALIB,		/**< SACPI SSDT table with ALIB implementation */
};

typedef struct {
	UINT32 CalloutName;
	AGESA_STATUS (*CalloutPtr) (UINT32 Func, UINT32 Data, VOID* ConfigPtr);
} BIOS_CALLOUT_STRUCT;


AGESA_STATUS agesawrapper_amdinitreset(void);
AGESA_STATUS agesawrapper_amdinitearly(void);
AGESA_STATUS agesawrapper_amdinitenv(void);
AGESA_STATUS agesawrapper_amdinitlate(void);
AGESA_STATUS agesawrapper_amdinitpost(void);
AGESA_STATUS agesawrapper_amdinitmid(void);

AGESA_STATUS agesawrapper_amdreadeventlog(void);

AGESA_STATUS agesawrapper_amdinitcpuio(void);
AGESA_STATUS agesawrapper_amdinitmmio(void);
AGESA_STATUS agesawrapper_amdinitresume(void);
AGESA_STATUS agesawrapper_amdS3Save(void);
AGESA_STATUS agesawrapper_amds3laterestore(void);
AGESA_STATUS agesawrapper_amdlaterunaptask(uint32_t, uint32_t, void *);
void * agesawrapper_getlateinitptr(int);

#endif /* _AGESAWRAPPER_H_ */
