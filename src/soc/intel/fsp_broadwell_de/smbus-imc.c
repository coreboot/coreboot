/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook, Inc.
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

#include <stddef.h>
#include <device/pci_def.h>
#include <device/early_smbus.h>
#include <intelblocks/imc.h>
#include <soc/pci_devs.h>
#include <spd.h>

/* read word, return value on success */
uint16_t smbus_read_word(u32 smbus_dev, u8 addr, u8 offset)
{
	uint16_t res = 0;

	if (imc_smbus_spd_xfer(IMC_DEV, addr, offset, IMC_DEVICE_EEPROM, IMC_DATA_WORD,
			       IMC_CONTROLLER_ID0, IMC_READ, &res)
	    == 0) {
		return res;
	}
	return 0;
}

/* read byte, return value on success */
uint8_t smbus_read_byte(u32 smbus_dev, u8 addr, u8 offset)
{
	uint16_t res = 0;

	if (imc_smbus_spd_xfer(IMC_DEV, addr, offset, IMC_DEVICE_EEPROM, IMC_DATA_BYTE,
			       IMC_CONTROLLER_ID0, IMC_READ, &res)
	    == 0) {
		return res;
	}
	return 0;
}

/* write byte, return 0 on success, -1 otherwise */
uint8_t smbus_write_byte(u32 smbus_dev, u8 addr, u8 offset, u8 value)
{
	if (imc_smbus_spd_xfer(IMC_DEV, addr, offset, IMC_DEVICE_WP_EEPROM, IMC_DATA_BYTE,
			       IMC_CONTROLLER_ID0, IMC_WRITE, &value)
	    == 0) {
		return 0;
	}
	return -1;
}
