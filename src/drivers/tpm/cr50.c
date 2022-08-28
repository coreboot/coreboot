/* SPDX-License-Identifier: BSD-3-Clause */

#include <drivers/spi/tpm/tpm.h>
#include <security/tpm/tis.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#define CR50_DID_VID	0x00281ae0L
#define TI50_DID_VID	0x504a6666L

#define CR50_BOARD_CFG_LOCKBIT_MASK 0x80000000U
#define CR50_BOARD_CFG_FEATUREBITS_MASK 0x3FFFFFFFU

#define CR50_BOARD_CFG_100US_READY_PULSE 0x00000001U
#define CR50_BOARD_CFG_VALUE \
		(CONFIG(CR50_USE_LONG_INTERRUPT_PULSES) \
		 ? CR50_BOARD_CFG_100US_READY_PULSE : 0)

enum cr50_register {
	CR50_FW_VER_REG,
	CR50_BOARD_CFG_REG,
};

#define CR50_FW_VER_REG_SPI		(TPM_LOCALITY_0_SPI_BASE + 0xf90)
#define CR50_BOARD_CFG_REG_SPI		(TPM_LOCALITY_0_SPI_BASE + 0xfe0)

#define CR50_FW_VER_REG_I2C		0x0f
#define CR50_BOARD_CFG_REG_I2C		0x1c

/* Return register address, which depends on the bus type, or -1 for error. */
static int get_reg_addr(enum cr50_register reg)
{
	if (CONFIG(SPI_TPM)) {
		switch (reg) {
		case CR50_FW_VER_REG:
			return CR50_FW_VER_REG_SPI;
		case CR50_BOARD_CFG_REG:
			return CR50_BOARD_CFG_REG_SPI;
		default:
			return -1;
		}
	}

	if (CONFIG(I2C_TPM)) {
		switch (reg) {
		case CR50_FW_VER_REG:
			return CR50_FW_VER_REG_I2C;
		case CR50_BOARD_CFG_REG:
			return CR50_BOARD_CFG_REG_I2C;
		default:
			return -1;
		}
	}

	return -1;
}

static bool cr50_fw_supports_board_cfg(struct cr50_firmware_version *version)
{
	/* Cr50 supports the CR50_BOARD_CFG register from version 0.5.5 / 0.6.5
	 * and onwards. */
	if (version->epoch > 0 || version->major >= 7
	    || (version->major >= 5 && version->minor >= 5))
		return true;

	printk(BIOS_INFO, "Cr50 firmware does not support CR50_BOARD_CFG, version: %d.%d.%d\n",
	       version->epoch, version->major, version->minor);

	return false;
}

/*
 * Expose method to read the CR50_BOARD_CFG register, will return zero if
 * register not supported by Cr50 firmware.
 */
static uint32_t cr50_get_board_cfg(void)
{
	struct cr50_firmware_version ver;
	uint32_t value;

	if (cr50_get_firmware_version(&ver) != CB_SUCCESS)
		return 0;

	if (!cr50_fw_supports_board_cfg(&ver))
		return 0;

	const enum cb_err ret = tis_vendor_read(get_reg_addr(CR50_BOARD_CFG_REG), &value,
					     sizeof(value));
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Error reading from Cr50\n");
		return 0;
	}

	return value & CR50_BOARD_CFG_FEATUREBITS_MASK;
}

/**
 * Set the BOARD_CFG register on the TPM chip to a particular compile-time constant value.
 */
enum cb_err cr50_set_board_cfg(void)
{
	/* If we get here and we aren't cr50, then we must be ti50 which does
	 * not currently need to support a board_cfg register. */
	if (!CONFIG(TPM_GOOGLE_CR50))
		return CB_SUCCESS;

	struct cr50_firmware_version ver;
	enum cb_err ret;
	uint32_t value;

	if (cr50_get_firmware_version(&ver) != CB_SUCCESS)
		return CB_ERR;

	if (!cr50_fw_supports_board_cfg(&ver))
		return CB_ERR;

	/* Set the CR50_BOARD_CFG register, for e.g. asking cr50 to use longer ready pulses. */
	ret = tis_vendor_read(get_reg_addr(CR50_BOARD_CFG_REG), &value, sizeof(value));
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Error reading from Cr50\n");
		return CB_ERR;
	}

	if ((value & CR50_BOARD_CFG_FEATUREBITS_MASK) == CR50_BOARD_CFG_VALUE) {
		printk(BIOS_INFO, "Current CR50_BOARD_CFG = 0x%08x, matches desired = 0x%08x\n",
		       value, CR50_BOARD_CFG_VALUE);
		return CB_SUCCESS;
	}

	if (value & CR50_BOARD_CFG_LOCKBIT_MASK) {
		/* The high bit is set, meaning that the Cr50 is already locked on a particular
		 * value for the register, but not the one we wanted. */
		printk(BIOS_ERR, "Current CR50_BOARD_CFG = 0x%08x, does not match"
		       "desired = 0x%08x\n", value, CR50_BOARD_CFG_VALUE);
		return CB_ERR;
	}

	printk(BIOS_INFO, "Current CR50_BOARD_CFG = 0x%08x, setting to 0x%08x\n",
	       value, CR50_BOARD_CFG_VALUE);
	value = CR50_BOARD_CFG_VALUE;

	ret = tis_vendor_write(get_reg_addr(CR50_BOARD_CFG_REG), &value, sizeof(value));
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Error writing to cr50\n");
		return ret;
	}

	return CB_SUCCESS;
}

bool cr50_is_long_interrupt_pulse_enabled(void)
{
	if (CONFIG(TPM_GOOGLE_CR50))
		return !!(cr50_get_board_cfg() & CR50_BOARD_CFG_100US_READY_PULSE);

        /* Ti50 and future GSCs will support only long interrupt pulses. */
        return true;
}

static enum cb_err cr50_parse_fw_version(const char *version_str,
					 struct cr50_firmware_version *ver)
{
	int epoch, major, minor;

	char *number = strstr(version_str, " RW_A:");
	if (!number)
		number = strstr(version_str, " RW_B:");
	if (!number)
		return CB_ERR_ARG;
	number += 6; /* Skip past the colon. */

	epoch = skip_atoi(&number);
	if (*number++ != '.')
		return CB_ERR_ARG;
	major = skip_atoi(&number);
	if (*number++ != '.')
		return CB_ERR_ARG;
	minor = skip_atoi(&number);

	ver->epoch = epoch;
	ver->major = major;
	ver->minor = minor;
	return CB_SUCCESS;
}

enum cb_err cr50_get_firmware_version(struct cr50_firmware_version *version)
{
	static struct cr50_firmware_version cr50_firmware_version;

	if (cr50_firmware_version.epoch || cr50_firmware_version.major ||
	    cr50_firmware_version.minor)
		goto success;

	int chunk_count = 0;
	size_t chunk_size = 50;
	char version_str[301];
	int addr = get_reg_addr(CR50_FW_VER_REG);

	/*
	 * Does not really matter what's written, this just makes sure
	 * the version is reported from the beginning.
	 */
	tis_vendor_write(addr, &chunk_size, 1);

	/*
	 * Read chunk_size bytes at a time, last chunk will be zero padded.
	 */
	do {
		uint8_t *buf = (uint8_t *)version_str + chunk_count * chunk_size;
		tis_vendor_read(addr, buf, chunk_size);
		if (!version_str[++chunk_count * chunk_size - 1])
			/* Zero padding detected: end of string. */
			break;
		/* Check if there is enough room for reading one more chunk. */
	} while (chunk_count * chunk_size < sizeof(version_str) - chunk_size);

	version_str[chunk_count * chunk_size] = '\0';
	printk(BIOS_INFO, "Firmware version: %s\n", version_str);

	if (cr50_parse_fw_version(version_str, &cr50_firmware_version) != CB_SUCCESS) {
		printk(BIOS_ERR, "Did not recognize Cr50 version format\n");
		return CB_ERR;
	}

success:
	if (version)
		*version = cr50_firmware_version;
	return CB_SUCCESS;
}

enum cb_err cr50_wait_tpm_ready(void)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, CONFIG_GOOGLE_TPM_IRQ_TIMEOUT_MS);

	while (!tis_plat_irq_status())
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "Cr50 TPM IRQ timeout!\n");
			return CB_ERR;
		}

	return CB_SUCCESS;
}
