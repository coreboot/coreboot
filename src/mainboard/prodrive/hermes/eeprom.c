/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <crc_byte.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <soc/intel/common/block/smbus/smbuslib.h>
#include <string.h>
#include <types.h>

#include "eeprom.h"

#define I2C_ADDR_EEPROM 0x57

/*
 * Check Signature in EEPROM (M24C32-FMN6TP)
 * If signature is there we assume that that the content is valid
 */
int check_signature(const size_t offset, const uint64_t signature)
{
	u8 blob[8] = {0};

	if (!eeprom_read_buffer(blob, offset, ARRAY_SIZE(blob))) {
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
static size_t get_board_settings_from_eeprom(struct eeprom_board_settings *board_cfg)
{
	const size_t board_settings_offset = offsetof(struct eeprom_layout, board_settings);

	if (eeprom_read_buffer(board_cfg, board_settings_offset, sizeof(*board_cfg))) {
		printk(BIOS_ERR, "CFG EEPROM: Failed to read board settings\n");
		return 0;
	}

	/*
	 * Ideally, the definition for board settings would always be the same across
	 * coreboot and the BMC. However, this is not always the case. When there's a
	 * mismatch, coreboot often has the newer definition with more settings. When
	 * there's a mismatch, coreboot and the BMC calculate the CRC for a different
	 * set of bytes, which results in two different CRC values.
	 *
	 * As existing board settings do not get repurposed, it is relatively easy to
	 * make coreboot backwards compatible with older BMC firmware revisions which
	 * do not provide the latest board settings. Starting with all board settings
	 * coreboot knows about, if the CRC does not match, we drop the last byte and
	 * try again until we find a match or exhaust all bytes.
	 */
	for (size_t len = sizeof(board_cfg->raw_settings); len > 0; len--) {
		const uint32_t crc = CRC(&board_cfg->raw_settings, len, crc32_byte);
		if (crc != board_cfg->signature)
			continue;

		printk(BIOS_DEBUG, "CFG EEPROM: Board settings CRC OK for %zu / %lu bytes\n",
				len, sizeof(board_cfg->raw_settings));
		return len;
	}

	printk(BIOS_ERR, "CFG EEPROM: Board settings have invalid checksum\n");
	return 0;
}

struct eeprom_board_settings *get_board_settings(void)
{
	/*
	 * Default settings to be used if the EEPROM settings are unavailable.
	 * Unspecified settings default to 0. These default values do not get
	 * passed to edk2 in any way, so there is no need to provide defaults
	 * for edk2-only options like `secureboot`.
	 */
	const struct eeprom_board_settings default_cfg = {
		.deep_sx_enabled      = 1,
		.wake_on_usb          = 1,
		.power_state_after_g3 = !CONFIG_MAINBOARD_POWER_FAILURE_STATE,
		.blue_rear_vref       = 2,
		.pink_rear_vref       = 2,
	};

	static struct eeprom_board_settings board_cfg = {0};
	static bool cfg_cached = false;

	if (cfg_cached)
		return &board_cfg;

	const size_t valid_bytes = get_board_settings_from_eeprom(&board_cfg);

	/* If we could not read all settings from the EEPROM, get the rest from defaults */
	for (size_t i = valid_bytes; i < sizeof(board_cfg.raw_settings); i++)
		board_cfg.raw_settings[i] = default_cfg.raw_settings[i];

	cfg_cached = true;

	return &board_cfg;
}

struct eeprom_bmc_settings *get_bmc_settings(void)
{
	const size_t bmc_settings_offset = offsetof(struct eeprom_layout, bmc_settings);
	static struct eeprom_bmc_settings bmc_cfg = {0};

	/* 0: uninitialized, 1: settings are valid */
	static int valid = 0;

	if (valid == 0) {
		if (eeprom_read_buffer(&bmc_cfg, bmc_settings_offset, sizeof(bmc_cfg))) {
			printk(BIOS_ERR, "CFG EEPROM: Failed to read BMC settings\n");
			return NULL;
		}
		valid = 1;
	}
	return &bmc_cfg;
}

const char *eeprom_read_serial(const size_t offset, const char *const fallback)
{
	static char serial_no[HERMES_SN_PN_LENGTH + 1] = { 0 };
	memset(serial_no, 0, sizeof(serial_no));

	if (eeprom_read_buffer(serial_no, offset, HERMES_SN_PN_LENGTH) == 0)
		return serial_no;
	else
		return fallback;
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

/* Read data from an EEPROM on SMBus and write it to a buffer */
bool eeprom_read_buffer(void *blob, size_t read_offset, size_t size)
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
		uint8_t *write_ptr = (uint8_t *)blob + i;
		write_ptr[0] = tmp[0];
		if (size - i > 1)
			write_ptr[1] = tmp[1];
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
bool eeprom_write_byte(const uint8_t data, const uint16_t write_offset)
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
	const size_t off = offsetof(struct eeprom_layout, board_layout);
	struct eeprom_board_layout old_layout = {0};
	bool ret = false;
	bool changed = false;

	/* Read old settings */
	if (eeprom_read_buffer(&old_layout, off, sizeof(old_layout))) {
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
			if (eeprom_write_byte(new[i], off + i)) {
				printk(BIOS_ERR, "CFG EEPROM: Write operation failed\n");
				ret = true;
				break;
			}
		}
	}

	printk(BIOS_DEBUG, "CFG EEPROM: Board Layout up%s\n", changed ? "dated" : " to date");

	return ret;
}
