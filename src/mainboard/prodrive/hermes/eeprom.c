/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/pci_ops.h>
#include <delay.h>
#include <console/console.h>
#include <crc_byte.h>
#include <device/smbus_host.h>
#include <soc/intel/common/block/smbus/smbuslib.h>
#include <types.h>

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

/*
 * Read board settings from the EEPROM and verify their checksum.
 * If checksum is valid, we assume the settings are sane as well.
 */
static bool get_board_settings_from_eeprom(struct eeprom_board_settings *board_cfg)
{
	const size_t board_settings_offset = offsetof(struct eeprom_layout, BoardSettings);

	if (read_write_config(board_cfg, board_settings_offset, 0, sizeof(*board_cfg))) {
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

struct eeprom_bmc_settings *get_bmc_settings(void)
{
	const size_t bmc_settings_offset = offsetof(struct eeprom_layout, BMCSettings);
	static struct eeprom_bmc_settings bmc_cfg = {0};

	/* 0: uninitialized, 1: settings are valid */
	static int valid = 0;

	if (valid == 0) {
		if (read_write_config(&bmc_cfg, bmc_settings_offset, 0, sizeof(bmc_cfg))) {
			printk(BIOS_ERR, "CFG EEPROM: Failed to read BMC settings\n");
			return NULL;
		}
		valid = 1;
	}
	return &bmc_cfg;
}

uint8_t get_bmc_hsi(void)
{
	uint8_t hsi = 0;
	struct eeprom_bmc_settings *s = get_bmc_settings();
	if (s)
		hsi = s->hsi;
	printk(BIOS_DEBUG, "CFG EEPROM: HSI 0x%x\n", hsi);

	return hsi;
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

void report_eeprom_error(const size_t off)
{
	printk(BIOS_ERR, "MB: Failed to read from EEPROM at addr. 0x%zx\n", off);
}

/*
 * Write a single byte into the EEPROM at specified offset.
 * Returns true on error, false on success.
 */
static bool write_byte_eeprom(const uint8_t data, const uint16_t write_offset)
{
	int ret = 0;

	printk(BIOS_SPEW, "CFG EEPROM: Writing %x at %x\n", data, write_offset);

	const uint32_t smb_ctrl_reg = pci_read_config32(PCH_DEV_SMBUS, HOSTC);
	pci_write_config32(PCH_DEV_SMBUS, HOSTC, smb_ctrl_reg | I2C_EN);

	/*
	 * The EEPROM expects two address bytes.
	 * Use the first byte of the block data as second address byte.
	 */
	uint8_t buffer[2] = {
		write_offset & 0xff,
		data,
	};

	for (size_t retry = 3; retry > 0; retry--) {
		/* The EEPROM NACKs request when busy writing */
		ret = do_smbus_block_write(SMBUS_IO_BASE, I2C_ADDR_EEPROM,
					   (write_offset >> 8) & 0xff,
					   sizeof(buffer), buffer);
		if (ret == sizeof(buffer))
			break;
		/* Maximum of 5 milliseconds write duration */
		mdelay(5);
	}

	/* Restore I2C_EN bit */
	pci_write_config32(PCH_DEV_SMBUS, HOSTC, smb_ctrl_reg);

	return ret != sizeof(buffer);
}

/*
 * Write board layout if it has changed into EEPROM.
 * Return true on error, false on success.
 */
bool write_board_settings(const struct eeprom_board_layout *new_layout)
{
	const size_t off = offsetof(struct eeprom_layout, BoardLayout);
	struct eeprom_board_layout old_layout = {0};
	bool ret = false;
	bool changed = false;

	/* Read old settings */
	if (read_write_config(&old_layout, off, 0, sizeof(old_layout))) {
		printk(BIOS_ERR, "CFG EEPROM: Read operation failed\n");
		return true;
	}

	assert(sizeof(old_layout) == sizeof(*new_layout));
	const uint8_t *const old = (const uint8_t *)&old_layout;
	const uint8_t *const new = (const uint8_t *)new_layout;

	/* Compare with new settings and only write changed bytes */
	for (size_t i = 0; i < sizeof(old_layout); i++) {
		if (old[i] != new[i]) {
			changed = true;
			if (write_byte_eeprom(new[i], off + i)) {
				printk(BIOS_ERR, "CFG EEPROM: Write operation failed\n");
				ret = true;
				break;
			}
		}
	}

	printk(BIOS_DEBUG, "CFG EEPROM: Board Layout up%s\n", changed ? "dated" : " to date");

	return ret;
}
