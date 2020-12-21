/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <crc_byte.h>
#include <device/smbus_host.h>
#include <soc/intel/common/block/smbus/smbuslib.h>
#include <types.h>

#include "variants/baseboard/include/eeprom.h"

#define I2C_ADDR_EEPROM 0x57

#define EEPROM_OFFSET_BOARD_SETTINGS 0x1f00

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

/*
 * Read board settings from the EEPROM and verify their checksum.
 * If checksum is valid, we assume the settings are sane as well.
 */
static bool get_board_settings_from_eeprom(struct eeprom_board_settings *board_cfg)
{
	if (read_write_config(board_cfg, EEPROM_OFFSET_BOARD_SETTINGS, 0, sizeof(*board_cfg))) {
		printk(BIOS_ERR, "CFG EEPROM: Failed to read board settings\n");
		return false;
	}

	const uint32_t crc =
		CRC(&board_cfg->raw_settings, sizeof(board_cfg->raw_settings), crc32_byte);

	if (crc != board_cfg->signature) {
		printk(BIOS_ERR, "CFG EEPROM: Board settings have invalid checksum\n");
		return false;
	}
	return true;
}

struct eeprom_board_settings *get_board_settings(void)
{
	static struct eeprom_board_settings board_cfg = {0};

	/* Tri-state: -1: settings are invalid, 0: uninitialized, 1: settings are valid */
	static int checked_valid = 0;

	if (checked_valid == 0) {
		const bool success = get_board_settings_from_eeprom(&board_cfg);
		checked_valid = success ? 1 : -1;
	}
	return checked_valid > 0 ? &board_cfg : NULL;
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
