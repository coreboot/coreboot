/* SPDX-License-Identifier: GPL-2.0-only */

#include "atf.h"
#include "nvparam.h"
#include <console/console.h>

/*
 * The NVPARAM service of the Standalone MM secure partition, see "Altra ATF
 * Interface Specification", "NVPARAM MM Interface", and edk2-platforms
 * AmpereAltraPkg Library/NVParamLib. The board settings (NV_SI_RO_BOARD_*)
 * are read from the BOARD_SETTINGS region of the flash.
 */

#define NVPARAM_MM_GUID \
	GUID_INIT(0xE4AC5024, 0x29BE, 0x4ADC, \
	0x93, 0x36, 0x87, 0xB5, 0xA0, 0x76, 0x23, 0x2D)

#define MM_NVPARAM_FUNC_READ		0x01

#define MM_NVPARAM_RES_SUCCESS		0xAABBCC00
#define MM_NVPARAM_RES_NOT_SET		0xAABBCC01
#define MM_NVPARAM_RES_NO_PERM		0xAABBCC02

#define NV_PERM_ALL			0xFFFF

int nvparam_get(uint32_t param, uint32_t *val)
{
	const guid_t guid = NVPARAM_MM_GUID;
	uint64_t dw[5] = { MM_NVPARAM_FUNC_READ, param, NV_PERM_ALL, 0, 0 };

	if (atf_mm_request(&guid, dw))
		return -1;

	switch (dw[0]) {
	case MM_NVPARAM_RES_SUCCESS:
		*val = dw[1];
		return 0;
	case MM_NVPARAM_RES_NOT_SET:
		return -1;
	default:
		printk(BIOS_ERR, "NVPARAM 0x%x: read failed (status 0x%llx)\n", param, dw[0]);
		return -1;
	}
}
