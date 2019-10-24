/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SECUNET_DMI_EEPROM_H
#define _SECUNET_DMI_EEPROM_H

#include <stdint.h>

enum bx26_strings {
	BOARD_MATNR,
	BOARD_SERIAL_NUMBER,
	BOARD_VERSION,
	BOARD_MCTRL_FW_VERSION,
	BOARD_CCR_FW_VERSION,
	BOARD_NIC_FW_VERSION,
	BOARD_LP_VERSION,
	BOARD_VERSION_ID,

	SYSTEM_PRODUCT_NAME,
	SYSTEM_VERSION,
	SYSTEM_SERIAL_NUMBER,
	SYSTEM_UUID,
	SYSTEM_MANUFACTURER,
	SYSTEM_PRODUCTION_DATE,
	SYSTEM_MLFB,
	SYSTEM_MATNR,
};

struct bx26_location {
	uint16_t offset;
	uint16_t length;
};

static const struct bx26_location bx26_locations[] = {
	[BOARD_MATNR]			= { 0x0000, 0x20 },
	[BOARD_SERIAL_NUMBER]		= { 0x0020, 0x20 },
	[BOARD_VERSION]			= { 0x0040, 0x20 },
	[BOARD_MCTRL_FW_VERSION]	= { 0x0060, 0x20 },
	[BOARD_CCR_FW_VERSION]		= { 0x0080, 0x20 },
	[BOARD_NIC_FW_VERSION]		= { 0x00a0, 0x20 },
	[BOARD_LP_VERSION]		= { 0x00c0, 0x20 },
	[BOARD_VERSION_ID]		= { 0x0100, 0x20 },

	[SYSTEM_PRODUCT_NAME]		= { 0x4000, 0x20 },
	[SYSTEM_VERSION]		= { 0x4040, 0x10 },
	[SYSTEM_SERIAL_NUMBER]		= { 0x4060, 0x10 },
	[SYSTEM_UUID]			= { 0x4080, 0x24 },
	[SYSTEM_MANUFACTURER]		= { 0x40c0, 0x20 },
	[SYSTEM_PRODUCTION_DATE]	= { 0x4100, 0x20 },
	[SYSTEM_MLFB]			= { 0x4140, 0x20 },
	[SYSTEM_MATNR]			= { 0x4180, 0x20 },
};

#endif
