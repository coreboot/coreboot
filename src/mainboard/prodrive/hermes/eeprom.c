/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <device/smbus_host.h>
#include <soc/intel/common/block/smbus/smbuslib.h>
#include "variants/baseboard/include/eeprom.h"

#define I2C_ADDR_EEPROM 0x57

/*
 * Check Signature in EEPROM (M24C32-FMN6TP)
 * If signature is there we assume that that the content is valid
 */
int check_signature(const size_t offset, const uint64_t signature)
{
	u8 blob[8] = {0};

	if (!read_write_config(blob, offset, 0, ARRAY_SIZE(blob))) {
		/* Check signature */
		if (*(uint64_t *)blob == signature) {
			printk(BIOS_DEBUG, "CFG EEPROM: Signature valid.\n");
			return 1;
		}
		printk(BIOS_DEBUG, "CFG EEPROM: Signature invalid - skipping option write.\n");
		return 0;
	}
	return 0;
}

/* Read data from offset and write it to offset in UPD */
bool read_write_config(void *blob, size_t read_offset, size_t write_offset, size_t size)
{
	int ret = 0;

	u32 smb_ctrl_reg = pci_read_config32(PCH_DEV_SMBUS, HOSTC);
	pci_write_config32(PCH_DEV_SMBUS, HOSTC, smb_ctrl_reg | I2C_EN);

	printk(BIOS_SPEW, "%s\tOffset: %04zx\tSize: %02zx\n", __func__,
		read_offset, size);

	/* We can always read two bytes at a time */
	for (size_t i = 0; i < size; i = i + 2) {
		u8 tmp[2] = {0};

		ret = do_smbus_process_call(SMBUS_IO_BASE, I2C_ADDR_EEPROM, 0,
			swab16(read_offset + i), (uint16_t *)&tmp[0]);
		if (ret < 0)
			break;

		/* Write to UPD */
		uint8_t *writePointer = (uint8_t *)blob + write_offset + i;
		if (size > 1 && (size % 2 == 0))
			memcpy(writePointer, tmp, 2);
		else
			*writePointer = tmp[0];
	}

	/* Restore I2C_EN bit */
	pci_write_config32(PCH_DEV_SMBUS, HOSTC, smb_ctrl_reg);

	return ret;
}
