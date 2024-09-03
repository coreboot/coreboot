/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_IMC_IMCLIB_H
#define SOC_INTEL_COMMON_BLOCK_IMC_IMCLIB_H

#include <device/pci_type.h>

enum smbus_command { IMC_READ, IMC_WRITE };

enum access_width { IMC_DATA_BYTE, IMC_DATA_WORD };

enum memory_controller_id { IMC_CONTROLLER_ID0, IMC_CONTROLLER_ID1 };

enum device_type_id {
	IMC_DEVICE_TSOD = 0x3,
	IMC_DEVICE_WP_EEPROM = 0x6,
	IMC_DEVICE_EEPROM = 0xa
};

/* Initiate SMBus/I2C transaction to DIMM EEPROM */
int imc_smbus_spd_xfer(pci_devfn_t dev, uint8_t slave_addr, uint8_t bus_addr,
		       enum device_type_id dti, enum access_width width,
		       enum memory_controller_id mcid, enum smbus_command cmd, void *data);

#endif
