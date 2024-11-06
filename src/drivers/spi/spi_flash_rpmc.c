/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#include "spi_flash_internal.h"

void spi_flash_fill_rpmc_caps(struct spi_flash *flash)
{
	struct sfdp_rpmc_info rpmc_info;

	flash->rpmc_caps.rpmc_available = false;

	if (spi_flash_get_sfdp_rpmc(flash, &rpmc_info) != CB_SUCCESS)
		return;

	if (rpmc_info.monotonic_counter_size != SFDP_RPMC_COUNTER_BITS_32) {
		printk(BIOS_WARNING, "RPMC: unexpected counter size\n");
		return;
	}

	flash->rpmc_caps.poll_op2_ext_stat = rpmc_info.busy_polling_method ==
					     SFDP_RPMC_POLL_OP2_EXTENDED_STATUS;
	flash->rpmc_caps.number_of_counters = rpmc_info.number_of_counters;
	flash->rpmc_caps.op1_write_cmd = rpmc_info.op1_write_command;
	flash->rpmc_caps.op2_read_cmd = rpmc_info.op2_read_command;
	flash->rpmc_caps.rpmc_available = true;
}

static bool spi_flash_rpmc_is_available(const struct spi_flash *flash)
{
	return flash->rpmc_caps.rpmc_available;
}

#define RPMC_OP2_READ_BUF_EXT_STATUS_OFFSET	0
#define RPMC_OP2_READ_BUF_TAG_OFFSET		1
#define RPMC_OP2_READ_BUF_COUNTER_DATA_OFFSET	(1 + SPI_RPMC_TAG_LEN)
#define RPMC_OP2_READ_BUF_SIGNATURE_OFFSET	(1 + SPI_RPMC_TAG_LEN + SPI_RPMC_COUNTER_DATA_LEN)

/*
 * 'extended_status', 'tag', 'counter_data', and 'signature' may be NULL if the caller
 * doesn't need the value. 'tag' must be a SPI_RPMC_TAG_LEN bytes long buffer, 'signature'
 * must be a SPI_RPMC_SIG_LEN bytes long buffer. When only 'extended_status' is non-NULL, only
 * the extended status is read back.
 */
static enum cb_err spi_flash_rpmc_op2_read(const struct spi_flash *flash,
					   uint8_t *extended_status, uint8_t *tag,
					   uint8_t *counter_data, uint8_t *signature)
{
	int cmd_ret;
	const bool only_check_extended_status = extended_status != NULL && tag == NULL &&
			counter_data == NULL && signature == NULL;

	/* command and dummy byte */
	uint8_t write_buf[1 + 1];
	/* extended status, tag, counter data, signature */
	uint8_t read_buf[1 + SPI_RPMC_TAG_LEN + SPI_RPMC_COUNTER_DATA_LEN + SPI_RPMC_SIG_LEN];

	write_buf[0] = flash->rpmc_caps.op2_read_cmd;
	write_buf[1] = 0x00; /* dummy byte */

	cmd_ret = spi_flash_cmd_multi(&flash->spi, write_buf, sizeof(write_buf), read_buf,
				      only_check_extended_status ? 1 : sizeof(read_buf));

	if (cmd_ret)
		return CB_ERR;

	if (extended_status != NULL)
		*extended_status = read_buf[RPMC_OP2_READ_BUF_EXT_STATUS_OFFSET];

	if (tag != NULL)
		memcpy(tag, &read_buf[RPMC_OP2_READ_BUF_TAG_OFFSET], SPI_RPMC_TAG_LEN);

	if (counter_data != NULL)
		memcpy(counter_data, &read_buf[RPMC_OP2_READ_BUF_COUNTER_DATA_OFFSET],
		       SPI_RPMC_COUNTER_DATA_LEN);

	if (signature != NULL)
		memcpy(signature, &read_buf[RPMC_OP2_READ_BUF_SIGNATURE_OFFSET],
		       SPI_RPMC_SIG_LEN);

	return CB_SUCCESS;
}

static enum cb_err spi_flash_rpmc_op2_get_exteded_status(const struct spi_flash *flash,
							 uint8_t *extended_status)
{
	return spi_flash_rpmc_op2_read(flash, extended_status, NULL, NULL, NULL);
}

#define RPMC_OP2_EXT_STATUS_SUCCESS		BIT(7)
#define RPMC_OP2_EXT_STATUS_ERR_FATAL		BIT(5)
#define RPMC_OP2_EXT_STATUS_ERR_BAD_COUNTER	BIT(4)
#define RPMC_OP2_EXT_STATUS_ERR_UNINITIALIZED	BIT(3)
#define RPMC_OP2_EXT_STATUS_ERR_INVALID		BIT(2)
#define RPMC_OP2_EXT_STATUS_ERR_OTHER		BIT(1)
#define RPMC_OP2_EXT_STATUS_POLL_BUSY		BIT(0)

static bool spi_flash_rpmc_is_extended_status_successful(uint8_t extended_status)
{
	return extended_status & RPMC_OP2_EXT_STATUS_SUCCESS;
}

static void spi_flash_rpmc_print_extended_status_error(uint8_t extended_status)
{
	if (extended_status & RPMC_OP2_EXT_STATUS_POLL_BUSY)
		printk(BIOS_WARNING, "SPI flash RPMC is busy\n");

	if (extended_status & RPMC_OP2_EXT_STATUS_ERR_OTHER)
		printk(BIOS_ERR, "SPI flash RPMC other error\n");

	if (extended_status & RPMC_OP2_EXT_STATUS_ERR_INVALID)
		printk(BIOS_ERR, "SPI flash RPMC invalid input\n");

	if (extended_status & RPMC_OP2_EXT_STATUS_ERR_UNINITIALIZED)
		printk(BIOS_ERR, "SPI flash RPMC uninitialized\n");

	if (extended_status & RPMC_OP2_EXT_STATUS_ERR_BAD_COUNTER)
		printk(BIOS_ERR, "SPI flash RPMC counter mismatch\n");

	if (extended_status & RPMC_OP2_EXT_STATUS_ERR_FATAL)
		printk(BIOS_ERR, "SPI flash RPMC fatal error\n");
}

static enum cb_err spi_flash_rpmc_check_extended_status(const struct spi_flash *flash)
{
	uint8_t extended_status;

	if (spi_flash_rpmc_op2_get_exteded_status(flash, &extended_status) != CB_SUCCESS)
		return CB_ERR;

	if (spi_flash_rpmc_is_extended_status_successful(extended_status))
		return CB_SUCCESS;

	spi_flash_rpmc_print_extended_status_error(extended_status);

	return CB_ERR;
}

static enum cb_err spi_flash_rpmc_poll_op2(const struct spi_flash *flash, uint64_t timeout_ms)
{
	unsigned int attempt = 0;
	uint8_t extended_status;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	do {
		attempt++;

		if (spi_flash_rpmc_op2_get_exteded_status(flash, &extended_status)) {
			printk(BIOS_WARNING,
			       "SPI command failed on attempt %d\n", attempt);
			return CB_ERR;
		}

		if ((extended_status & RPMC_OP2_EXT_STATUS_POLL_BUSY) == 0)
			return CB_SUCCESS;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_WARNING, "SPI RPMC poll timeout at %lld msec after %d attempts\n",
	       stopwatch_duration_msecs(&sw), attempt);

	return CB_ERR;
}

static enum cb_err spi_flash_rpmc_wait_ready(const struct spi_flash *flash,
					     uint64_t timeout_ms)
{
	if (!flash->rpmc_caps.poll_op2_ext_stat) {
		/* poll read status */
		if (spi_flash_cmd_wait_ready(flash, timeout_ms))
			return CB_ERR;
	} else {
		/* poll RPMC OP2 extended status */
		if (spi_flash_rpmc_poll_op2(flash, timeout_ms) != CB_SUCCESS)
			return CB_ERR;
	}

	return CB_SUCCESS;
}

static bool spi_flash_rpmc_is_valid_counter_addr(const struct spi_flash *flash,
						 uint8_t counter_addr)
{
	return counter_addr < flash->rpmc_caps.number_of_counters;
}

/* TODO: replace with proper value read from SFDP?! */
#define SPI_FLASH_RPMC_TIMEOUT_MS			200

#define RPMC_OP1_CMD_WRITE_ROOT_KEY_REGISTER		0x00
#define RPMC_OP1_CMD_UPDATE_HMAC_KEY_REGISTER		0x01
#define RPMC_OP1_CMD_INCREMENT_MONOTONIC_COUNTER	0x02
#define RPMC_OP1_CMD_REQUEST_MONOTONIC_COUNTER		0x03

enum cb_err spi_flash_rpmc_write_root_key(const struct spi_flash *flash, uint8_t counter_addr,
					  const uint8_t *root_key,
					  const uint8_t *truncated_signature)
{
	/* command, command type, counter address, reserved, root key, truncated signature */
	uint8_t buf[1 + 1 + 1 + 1 + SPI_RPMC_ROOT_KEY_LEN + SPI_RPMC_TRUNCTAED_SIG_LEN];

	if (!spi_flash_rpmc_is_available(flash))
		return CB_ERR;

	if (!spi_flash_rpmc_is_valid_counter_addr(flash, counter_addr))
		return CB_ERR;

	buf[0] = flash->rpmc_caps.op1_write_cmd;
	buf[1] = RPMC_OP1_CMD_WRITE_ROOT_KEY_REGISTER;
	buf[2] = counter_addr;
	buf[3] = 0x00; /* reserved */
	memcpy(&buf[4], root_key, SPI_RPMC_ROOT_KEY_LEN);
	memcpy(&buf[4 + SPI_RPMC_ROOT_KEY_LEN], truncated_signature,
	       SPI_RPMC_TRUNCTAED_SIG_LEN);

	if (spi_flash_cmd_multi(&flash->spi, buf, sizeof(buf), NULL, 0))
		return CB_ERR;

	if (spi_flash_rpmc_wait_ready(flash, SPI_FLASH_RPMC_TIMEOUT_MS) != CB_SUCCESS)
		return CB_ERR;

	if (spi_flash_rpmc_check_extended_status(flash) != CB_SUCCESS)
		return CB_ERR;

	return CB_SUCCESS;
}

enum cb_err spi_flash_rpmc_update_hmac_key(const struct spi_flash *flash, uint8_t counter_addr,
					   uint8_t *key_data, const uint8_t *signature)
{
	/* command, command type, counter address, reserved, key data, signature */
	uint8_t buf[1 + 1 + 1 + 1 + SPI_RPMC_KEY_DATA_LEN + SPI_RPMC_SIG_LEN];

	if (!spi_flash_rpmc_is_available(flash))
		return CB_ERR;

	if (!spi_flash_rpmc_is_valid_counter_addr(flash, counter_addr))
		return CB_ERR;

	buf[0] = flash->rpmc_caps.op1_write_cmd;
	buf[1] = RPMC_OP1_CMD_UPDATE_HMAC_KEY_REGISTER;
	buf[2] = counter_addr;
	buf[3] = 0x00; /* reserved */
	memcpy(&buf[4], key_data, SPI_RPMC_KEY_DATA_LEN);
	memcpy(&buf[4 + SPI_RPMC_KEY_DATA_LEN], signature, SPI_RPMC_SIG_LEN);

	if (spi_flash_cmd_multi(&flash->spi, buf, sizeof(buf), NULL, 0))
		return CB_ERR;

	if (spi_flash_rpmc_wait_ready(flash, SPI_FLASH_RPMC_TIMEOUT_MS) != CB_SUCCESS)
		return CB_ERR;

	if (spi_flash_rpmc_check_extended_status(flash) != CB_SUCCESS)
		return CB_ERR;

	return CB_SUCCESS;
}

enum cb_err spi_flash_rpmc_increment(const struct spi_flash *flash, uint8_t counter_addr,
				     const uint8_t *counter_data, const uint8_t *signature)
{
	/* command, command type, counter address, reserved, counter data, signature */
	uint8_t buf[1 + 1 + 1 + 1 + SPI_RPMC_COUNTER_DATA_LEN + SPI_RPMC_SIG_LEN];

	if (!spi_flash_rpmc_is_available(flash))
		return CB_ERR;

	if (!spi_flash_rpmc_is_valid_counter_addr(flash, counter_addr))
		return CB_ERR;

	buf[0] = flash->rpmc_caps.op1_write_cmd;
	buf[1] = RPMC_OP1_CMD_INCREMENT_MONOTONIC_COUNTER;
	buf[2] = counter_addr;
	buf[3] = 0x00; /* reserved */
	memcpy(&buf[4], counter_data, SPI_RPMC_COUNTER_DATA_LEN);
	memcpy(&buf[4 + SPI_RPMC_COUNTER_DATA_LEN], signature, SPI_RPMC_SIG_LEN);

	if (spi_flash_cmd_multi(&flash->spi, buf, sizeof(buf), NULL, 0))
		return CB_ERR;

	if (spi_flash_rpmc_wait_ready(flash, SPI_FLASH_RPMC_TIMEOUT_MS) != CB_SUCCESS)
		return CB_ERR;

	if (spi_flash_rpmc_check_extended_status(flash) != CB_SUCCESS)
		return CB_ERR;

	return CB_SUCCESS;
}

enum cb_err spi_flash_rpmc_request(const struct spi_flash *flash, uint8_t counter_addr,
				   const uint8_t *tag, const uint8_t *signature,
				   uint8_t *counter_data, uint8_t *signature_out)
{
	/* command, command type, counter address, reserved, tag, signature */
	uint8_t buf[1 + 1 + 1 + 1 + SPI_RPMC_TAG_LEN + SPI_RPMC_SIG_LEN];
	uint8_t extended_status;

	if (!spi_flash_rpmc_is_available(flash))
		return CB_ERR;

	if (!spi_flash_rpmc_is_valid_counter_addr(flash, counter_addr))
		return CB_ERR;

	buf[0] = flash->rpmc_caps.op1_write_cmd;
	buf[1] = RPMC_OP1_CMD_REQUEST_MONOTONIC_COUNTER;
	buf[2] = counter_addr;
	buf[3] = 0x00; /* reserved */
	memcpy(&buf[4], tag, SPI_RPMC_TAG_LEN);
	memcpy(&buf[4 + SPI_RPMC_TAG_LEN], signature, SPI_RPMC_SIG_LEN);

	if (spi_flash_cmd_multi(&flash->spi, buf, sizeof(buf), NULL, 0))
		return CB_ERR;

	if (spi_flash_rpmc_wait_ready(flash, SPI_FLASH_RPMC_TIMEOUT_MS) != CB_SUCCESS)
		return CB_ERR;

	/*
	 * Since we need to read back more than just the extended status for this command and
	 * want to avoid unnecessary SPI transactions, spi_flash_rpmc_check_extended_status
	 * isn't used here.
	 */
	if (spi_flash_rpmc_op2_read(flash, &extended_status, NULL, counter_data, signature_out)
			!= CB_SUCCESS)
		return CB_ERR;

	if (!spi_flash_rpmc_is_extended_status_successful(extended_status)) {
		spi_flash_rpmc_print_extended_status_error(extended_status);
		return CB_ERR;
	}

	return CB_SUCCESS;
}
