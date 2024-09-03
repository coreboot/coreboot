/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/cb_err.h>
#include <intelblocks/imc.h>
#include <soc/pci_devs.h>
#include <spd_bin.h>

#include "imclib.h"

int spd_read_byte(u8 slave_addr, u8 bus_addr)
{
	uint8_t value;

	if (imc_smbus_spd_xfer(IMC_SPD_DEV, slave_addr, bus_addr, IMC_DEVICE_EEPROM, IMC_DATA_BYTE,
			       IMC_CONTROLLER_ID0, IMC_READ, &value) == CB_SUCCESS) {
		return value;
	}

	return -1;
}

int spd_read_word(u8 slave_addr, u8 bus_addr)
{
	uint16_t value = 0;

	if (imc_smbus_spd_xfer(IMC_SPD_DEV, slave_addr, bus_addr, IMC_DEVICE_EEPROM, IMC_DATA_WORD,
			       IMC_CONTROLLER_ID0, IMC_READ, &value) == CB_SUCCESS) {
		return value;
	}

	return -1;
}

void spd_write_byte(u8 slave_addr, u8 bus_addr, u8 value)
{
	imc_smbus_spd_xfer(IMC_SPD_DEV, slave_addr, bus_addr, IMC_DEVICE_WP_EEPROM,
			   IMC_DATA_BYTE, IMC_CONTROLLER_ID0, IMC_WRITE, &value);
}
