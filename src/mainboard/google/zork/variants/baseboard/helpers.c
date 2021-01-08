/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <inttypes.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

/* Global definitions for FW_CONFIG values */
enum {
	/* Daughterboard index for attributes. */
	FW_CONFIG_MASK_DB_INDEX = 0xf,
	FW_CONFIG_DB_INDEX_SHIFT = 0,
	/* Mainboard USB index for attributes. */
	FW_CONFIG_MASK_MB_USB_INDEX = 0xf,
	FW_CONFIG_MB_USB_INDEX_SHIFT = 4,
	/* Lid accelerometer properties. */
	FW_CONFIG_MASK_LID_ACCEL = 0x7,
	FW_CONFIG_LID_ACCEL_SHIFT = 8,
	/* Base gyro sensor properties. */
	FW_CONFIG_MASK_BASE_GYRO = 0x7,
	FW_CONFIG_BASE_GYRO_SHIFT = 11,
	/* Keyboard backlight presence */
	FW_CONFIG_MASK_KEYB_BL = 0x1,
	FW_CONFIG_KEYB_BL_SHIFT = 14,
	/* Tablet mode supported through lid angle */
	FW_CONFIG_MASK_LID_ANGLE_TABLET_MODE = 0x1,
	FW_CONFIG_LID_ANGLE_TABLET_MODE_SHIFT = 15,
	/* Stylus presence */
	FW_CONFIG_MASK_STYLUS = 0x1,
	FW_CONFIG_STYLUS_SHIFT = 16,
	/* Fingerprint sensor presence */
	FW_CONFIG_MASK_FP = 0x1,
	FW_CONFIG_SHIFT_FP = 17,
	/* NVME presence */
	FW_CONFIG_MASK_NVME = 0x1,
	FW_CONFIG_SHIFT_NVME = 18,
	/* EMMC presence */
	FW_CONFIG_MASK_EMMC = 0x1,
	FW_CONFIG_SHIFT_EMMC = 19,
	/* SD controller type */
	FW_CONFIG_MASK_SD_CTRLR = 0x7,
	FW_CONFIG_SHIFT_SD_CTRLR = 20,
	/* SPI speed value */
	FW_CONFIG_MASK_SPI_SPEED = 0xf,
	FW_CONFIG_SHIFT_SPI_SPEED = 23,
	/* Fan information */
	FW_CONFIG_MASK_FAN = 0x3,
	FW_CONFIG_SHIFT_FAN = 27,
	/* WWAN presence */
	FW_CONFIG_MASK_WWAN = 0x1,
	FW_CONFIG_SHIFT_WWAN = 29,
};

static int get_fw_config(uint64_t *val)
{
	static uint64_t known_value;

	if (known_value) {
		*val = known_value;
		return 0;
	}

	if (google_chromeec_cbi_get_fw_config(&known_value) != 0) {
		printk(BIOS_ERR, "FW_CONFIG not set in CBI\n");
		return -1;
	}

	*val = known_value;

	return 0;
}

static unsigned int extract_field(uint64_t mask, int shift)
{
	uint64_t fw_config;

	/* On errors nothing is assumed to be set. */
	if (get_fw_config(&fw_config))
		return 0;

	return (fw_config >> shift) & mask;
}

int variant_has_emmc(void)
{
	return !!extract_field(FW_CONFIG_MASK_EMMC, FW_CONFIG_SHIFT_EMMC);
}

int variant_has_nvme(void)
{
	return !!extract_field(FW_CONFIG_MASK_NVME, FW_CONFIG_SHIFT_NVME);
}

int variant_has_wwan(void)
{
	return !!extract_field(FW_CONFIG_MASK_WWAN, FW_CONFIG_SHIFT_WWAN);
}

bool variant_uses_v3_schematics(void)
{
	uint32_t board_version;

	if (!CONFIG(VARIANT_SUPPORTS_PRE_V3_SCHEMATICS))
		return true;

	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		return false;

	if ((int)board_version < CONFIG_VARIANT_MIN_BOARD_ID_V3_SCHEMATICS)
		return false;

	return true;
}

bool variant_uses_v3_6_schematics(void)
{
	uint32_t board_version;

	if (!CONFIG(VARIANT_SUPPORTS_PRE_V3_6_SCHEMATICS))
		return true;

	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		return false;

	if ((int)board_version < CONFIG_VARIANT_MIN_BOARD_ID_V3_6_SCHEMATICS)
		return false;

	return true;
}

/*
 * pre-v3.6, CODEC_GPI was used as headphone jack interrupt.
 * Starting v3.6 this was changed to a separate GPIO.
 */
bool variant_uses_codec_gpi(void)
{
	return !variant_uses_v3_6_schematics();
}

bool variant_has_active_low_wifi_power(void)
{
	uint32_t board_version;

	if (!CONFIG(VARIANT_SUPPORTS_WIFI_POWER_ACTIVE_HIGH))
		return true;

	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		return false;

	if ((int)board_version < CONFIG_VARIANT_MIN_BOARD_ID_WIFI_POWER_ACTIVE_LOW)
		return false;

	return true;
}

int variant_get_daughterboard_id(void)
{
	return extract_field(FW_CONFIG_MASK_DB_INDEX, FW_CONFIG_DB_INDEX_SHIFT);
}

bool variant_has_fingerprint(void)
{
	if (CONFIG(VARIANT_HAS_FPMCU))
		return true;

	return false;
}

bool fpmcu_needs_delay(void)
{
	/*
	 *  Older board versions need an extra delay here to finish resetting
	 *  the FPMCU.  The resistor value in the glitch prevention circuit was
	 *  sized so that the FPMCU doesn't turn of for ~1 second.  On newer
	 *  boards, that's been updated to ~30ms, which allows the FPMCU's
	 *  reset to be completed in the time between bootblock and finalize.
	 */
	uint32_t board_version;

	if (google_chromeec_cbi_get_board_version(&board_version))
		board_version = 1;

	if (board_version <= CONFIG_VARIANT_MAX_BOARD_ID_BROKEN_FMPCU_POWER)
		return true;

	return false;
}
