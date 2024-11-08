/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <types.h>

#include "spi_flash_internal.h"

/*
 * Assumptions (not) made in the code:
 * - code is supposed to be endian-safe
 *
 * Limitations:
 * - only legacy access protocol supported:
 *   - only NOR flash
 *   - read SFDP instruction is sufficient and no fetch SFDP instruction needed
 *   - always 3 byte addresses for SFDP
 *   - only 1 bit SPI mode supported
 *   - always 8 dummy cycles after the address is sent
*/

#define CMD_READ_SFDP			0x5a

static void read_sfdp_data_update_buffer_offset(uint8_t *buf_cmd, uint32_t offset)
{
	buf_cmd[1] = offset >> 16 & 0xff;
	buf_cmd[2] = offset >> 8 & 0xff;
	buf_cmd[3] = offset >> 0 & 0xff;
}

static enum cb_err read_sfdp_data(const struct spi_flash *flash, uint32_t offset, size_t len,
				  uint8_t *buf)
{
	uint8_t buf_cmd[5];

	buf_cmd[0] = CMD_READ_SFDP;
	/* the read offset in buf_cmd[1..3] gets written in
	   read_sfdp_data_update_buffer_offset */
	buf_cmd[4] = 0; /* dummy byte */

	uint8_t *data = buf;

	/* split data transfers into chunks that each fit into the SPI controller's buffer */
	while (len) {
		uint32_t xfer_len = spi_crop_chunk(&flash->spi, sizeof(buf_cmd), len);
		read_sfdp_data_update_buffer_offset(buf_cmd, offset);
		if (spi_flash_cmd_multi(&flash->spi, buf_cmd, sizeof(buf_cmd), data, xfer_len))
			return CB_ERR;
		offset += xfer_len;
		data += xfer_len;
		len -= xfer_len;
	}

	return CB_SUCCESS;
}

#define SFDP_HEADER_OFFSET			0
#define SFDP_HEADER_LEN				(2 * sizeof(uint32_t))
#define SFDP_HEADER_SIGNATURE			0x50444653 /* LE: "SFDP" */
#define SFDP_HEADER_ACCESS_PROTOCOL_LEGACY	0xff
/* header byte offsets */
#define SFDP_HEADER_SIGNATURE_0			0
#define SFDP_HEADER_SIGNATURE_1			1
#define SFDP_HEADER_SIGNATURE_2			2
#define SFDP_HEADER_SIGNATURE_3			3
#define SFDP_HEADER_MINOR_REV			4
#define SFDP_HEADER_MAJOR_REV			5
#define SFDP_HEADER_PARAMETER_HEADER_NUMBER	6
#define SFDP_HEADER_ACCESS_PROTOCOL		7

static enum cb_err get_sfdp_header(const struct spi_flash *flash, uint16_t *sfdp_revision,
				   uint8_t *number_of_parameter_headers,
				   uint8_t *access_prototcol)
{
	uint8_t buf[SFDP_HEADER_LEN];
	uint32_t signature;

	if (read_sfdp_data(flash, SFDP_HEADER_OFFSET, SFDP_HEADER_LEN, buf) != CB_SUCCESS)
		return CB_ERR;

	signature = buf[SFDP_HEADER_SIGNATURE_0] | buf[SFDP_HEADER_SIGNATURE_1] << 8 |
		buf[SFDP_HEADER_SIGNATURE_2] << 16 | buf[SFDP_HEADER_SIGNATURE_3] << 24;
	*sfdp_revision = buf[SFDP_HEADER_MINOR_REV] | buf[SFDP_HEADER_MAJOR_REV] << 8;
	*number_of_parameter_headers = buf[SFDP_HEADER_PARAMETER_HEADER_NUMBER] + 1;
	*access_prototcol = buf[SFDP_HEADER_ACCESS_PROTOCOL];

	if (signature != SFDP_HEADER_SIGNATURE)
		return CB_ERR;

	/* We make the assumption in the code that the legacy access protocol is used */
	if (*access_prototcol != SFDP_HEADER_ACCESS_PROTOCOL_LEGACY)
		return CB_ERR;

	return CB_SUCCESS;
}

#define SFDP_PARAMETER_HEADER_LEN		(2 * sizeof(uint32_t))
#define SFDP_PARAMETER_HEADER_OFFSET(n)		(SFDP_HEADER_OFFSET + SFDP_HEADER_LEN + \
						 n * SFDP_PARAMETER_HEADER_LEN)
/* parameter header byte offsets */
#define SFDP_PARAMETER_HEADER_ID_LSB		0
#define SFDP_PARAMETER_HEADER_MINOR_REV		1
#define SFDP_PARAMETER_HEADER_MAJOR_REV		2
#define SFDP_PARAMETER_HEADER_LENGTH_DWORDS	3
#define SFDP_PARAMETER_HEADER_POINTER_0		4
#define SFDP_PARAMETER_HEADER_POINTER_1		5
#define SFDP_PARAMETER_HEADER_POINTER_2		6
#define SFDP_PARAMETER_HEADER_ID_MSB		7

/* get_sfdp_parameter_header_by_index must be called with a valid index */
static enum cb_err get_sfdp_parameter_header_by_index(const struct spi_flash *flash,
						      uint8_t index, uint16_t *id,
						      uint16_t *revision,
						      uint8_t *length_dwords,
						      uint32_t *table_pointer)
{
	uint8_t buf[SFDP_PARAMETER_HEADER_LEN];

	if (read_sfdp_data(flash, SFDP_PARAMETER_HEADER_OFFSET(index),
		       SFDP_PARAMETER_HEADER_LEN, buf) != CB_SUCCESS)
		return CB_ERR;

	*id = buf[SFDP_PARAMETER_HEADER_ID_LSB] | buf[SFDP_PARAMETER_HEADER_ID_MSB] << 8;
	*revision = buf[SFDP_PARAMETER_HEADER_MINOR_REV] |
		buf[SFDP_PARAMETER_HEADER_MAJOR_REV] << 8;
	*length_dwords = buf[SFDP_PARAMETER_HEADER_LENGTH_DWORDS];
	*table_pointer = buf[SFDP_PARAMETER_HEADER_POINTER_0] |
		buf[SFDP_PARAMETER_HEADER_POINTER_1] << 8 |
		buf[SFDP_PARAMETER_HEADER_POINTER_2] << 16;

	/* the parameter table pointer byte address must be DWORD-aligned */
	if (!IS_ALIGNED(*table_pointer, sizeof(uint32_t)))
		return CB_ERR;

	/* TODO: check id validity? */

	return CB_SUCCESS;
}

void spi_flash_print_sfdp_headers(const struct spi_flash *flash)
{
	enum cb_err stat;
	uint16_t sfdp_rev;
	uint8_t param_header_count;
	uint8_t access_protocol;

	if (get_sfdp_header(flash, &sfdp_rev, &param_header_count, &access_protocol) !=
			CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to read SFDP header from SPI flash\n");
		return;
	}

	printk(BIOS_DEBUG, "SFDP header found in SPI flash.\n");
	printk(BIOS_DEBUG, "major rev %#x, minor rev %#x, access protocol %#x, "
	       "number of headers %d\n", sfdp_rev >> 8, sfdp_rev & 0xff, access_protocol,
	       param_header_count);

	uint16_t tbl_id;
	uint16_t tbl_rev;
	uint8_t tbl_len_dwords;
	uint32_t tbl_pointer;

	for (unsigned int i = 0; i < param_header_count; i++) {
		stat = get_sfdp_parameter_header_by_index(flash, i, &tbl_id, &tbl_rev,
							  &tbl_len_dwords, &tbl_pointer);

		if (stat == CB_SUCCESS) {
			printk(BIOS_DEBUG, "SFPD header with index %d:\n", i);
			printk(BIOS_DEBUG, "  table ID %#x, major rev %#x, minor rev %#x\n",
			       tbl_id, tbl_rev >> 8, tbl_rev & 0xff);
			printk(BIOS_DEBUG, "  table pointer %#x, table length DWORDS %#x\n",
			       tbl_pointer, tbl_len_dwords);
		} else {
			printk(BIOS_ERR, "Cound't read SFPD header with index %d.\n", i);
		}
	}
}

static enum cb_err find_sfdp_parameter_header(const struct spi_flash *flash, uint16_t table_id,
					      uint16_t *revision, uint8_t *length_dwords,
					      uint32_t *table_pointer)
{
	enum cb_err stat;
	uint16_t sfdp_rev;
	uint8_t param_header_count;
	uint8_t access_protocol;

	if (get_sfdp_header(flash, &sfdp_rev, &param_header_count, &access_protocol) !=
			CB_SUCCESS)
		return CB_ERR;

	/* TODO: add version check? */

	uint16_t tbl_id;
	uint16_t tbl_rev;
	uint8_t tbl_len_dwords;
	uint32_t tbl_pointer;

	for (unsigned int i = 0; i < param_header_count; i++) {
		stat = get_sfdp_parameter_header_by_index(flash, i, &tbl_id, &tbl_rev,
							  &tbl_len_dwords, &tbl_pointer);

		if (stat == CB_SUCCESS && tbl_id == table_id) {
			*revision = tbl_rev;
			*length_dwords = tbl_len_dwords;
			*table_pointer = tbl_pointer;
			return CB_SUCCESS;
		}
	}

	printk(BIOS_WARNING, "Cound't find SFPD header with table ID %#x.\n", table_id);

	return CB_ERR;
}

#define SFDP_PARAMETER_ID_RPMC			0xff03

#define SFDP_RPMC_TABLE_LENGTH_DWORDS		2
#define SFDP_RPMC_TABLE_SUPPORTED_MAJOR_REV	1

/* RPMC parameter table byte offsets and fields */
#define SFDP_RPMC_TABLE_CONFIG					0
#define   SFDP_RPMC_TABLE_CONFIG_FLASH_HARDENING_BIT		BIT(0)
#define   SFDP_RPMC_TABLE_CONFIG_MONOTONIC_COUNTER_SIZE_BIT	BIT(1)
#define   SFDP_RPMC_TABLE_CONFIG_BUSY_POLLING_METHOD		BIT(2)
#define   SFDP_RPMC_TABLE_CONFIG_RESERVED			BIT(3)
#define   SFDP_RPMC_TABLE_CONFIG_RESERVED_VALUE			0x08
#define   SFDP_RPMC_TABLE_CONFIG_NUM_COUNTERS_MASK		0xf0
#define   SFDP_RPMC_TABLE_CONFIG_NUM_COUNTERS_SHIFT		4
#define SFDP_RPMC_TABLE_RPMC_OP1				1
#define SFDP_RPMC_TABLE_RPMC_OP2				2
#define SFDP_RPMC_TABLE_UPDATE_RATE				3
#define   SFDP_RPMC_TABLE_UPDATE_RATE_MASK			0x0f
#define   SFDP_RPMC_TABLE_UPDATE_RATE_RESERVED_MASK		0xf0
#define   SFDP_RPMC_TABLE_UPDATE_RATE_RESERVED_VALUE		0xf0
#define SFDP_RPMC_TABLE_READ_COUNTER_POLLING_DELAY		4
#define SFDP_RPMC_TABLE_WRITE_COUNTER_POLLING_SHORT_DELAY	5
#define SFDP_RPMC_TABLE_WRITE_COUNTER_POLLING_LONG_DELAY	6
#define SFDP_RPMC_TABLE_RESERVED_BYTE				7
#define   SFDP_RPMC_TABLE_RESERVED_BYTE_VALUE			0xff

static uint64_t calc_rpmc_update_rate_s(uint8_t val)
{
	/* val is at most 15, so this won't overflow */
	return 5 * 1 << (val & SFDP_RPMC_TABLE_UPDATE_RATE_MASK);
}

#define SPDF_RPMC_DELAY_VALUE_MASK		0x1f
#define SPDF_RPMC_DELAY_UNIT_MASK		0x60
#define SPDF_RPMC_DELAY_UNIT_SHIFT		5
#define SPDF_RPMC_DELAY_SHORT_UNIT_0_US		1	/* 1us */
#define SPDF_RPMC_DELAY_SHORT_UNIT_1_US		16	/* 16us */
#define SPDF_RPMC_DELAY_SHORT_UNIT_2_US		128	/* 128us */
#define SPDF_RPMC_DELAY_SHORT_UNIT_3_US		1000	/* 1ms */
#define SPDF_RPMC_DELAY_LONG_UNIT_0_US		1000	/* 1ms */
#define SPDF_RPMC_DELAY_LONG_UNIT_1_US		16000	/* 16ms */
#define SPDF_RPMC_DELAY_LONG_UNIT_2_US		128000	/* 128ms */
#define SPDF_RPMC_DELAY_LONG_UNIT_3_US		1000000	/* 1s */

static uint64_t calc_rpmc_short_delay_us(uint8_t val)
{
	const uint8_t value = val & SPDF_RPMC_DELAY_VALUE_MASK;
	const uint8_t shift = (val & SPDF_RPMC_DELAY_UNIT_MASK) >> SPDF_RPMC_DELAY_UNIT_SHIFT;
	uint64_t multiplier;

	switch (shift) {
	case 0:
		multiplier = SPDF_RPMC_DELAY_SHORT_UNIT_0_US;
		break;
	case 1:
		multiplier = SPDF_RPMC_DELAY_SHORT_UNIT_1_US;
		break;
	case 2:
		multiplier = SPDF_RPMC_DELAY_SHORT_UNIT_2_US;
		break;
	default:
		multiplier = SPDF_RPMC_DELAY_SHORT_UNIT_3_US;
		break;
	}

	return value * multiplier;
}

static uint64_t calc_rpmc_long_delay_us(uint8_t val)
{
	const uint8_t value = val & SPDF_RPMC_DELAY_VALUE_MASK;
	const uint8_t shift = (val & SPDF_RPMC_DELAY_UNIT_MASK) >> SPDF_RPMC_DELAY_UNIT_SHIFT;
	uint64_t multiplier;

	switch (shift) {
	case 0:
		multiplier = SPDF_RPMC_DELAY_LONG_UNIT_0_US;
		break;
	case 1:
		multiplier = SPDF_RPMC_DELAY_LONG_UNIT_1_US;
		break;
	case 2:
		multiplier = SPDF_RPMC_DELAY_LONG_UNIT_2_US;
		break;
	default:
		multiplier = SPDF_RPMC_DELAY_LONG_UNIT_3_US;
		break;
	}

	return value * multiplier;
}

enum cb_err spi_flash_get_sfdp_rpmc(const struct spi_flash *flash,
				    struct sfdp_rpmc_info *rpmc_info)
{
	uint16_t rev;
	uint8_t length_dwords;
	uint32_t table_pointer;
	uint8_t buf[SFDP_RPMC_TABLE_LENGTH_DWORDS * sizeof(uint32_t)];

	if (find_sfdp_parameter_header(flash, SFDP_PARAMETER_ID_RPMC, &rev, &length_dwords,
				       &table_pointer) != CB_SUCCESS)
		return CB_ERR;

	if (length_dwords != SFDP_RPMC_TABLE_LENGTH_DWORDS)
		return CB_ERR;

	if (rev >> 8 != SFDP_RPMC_TABLE_SUPPORTED_MAJOR_REV) {
		printk(BIOS_ERR, "Unsupprted major RPMC table revision %#x\n", rev >> 8);
		return CB_ERR;
	}

	if (read_sfdp_data(flash, table_pointer, sizeof(buf), buf) != CB_SUCCESS)
		return CB_ERR;

	if ((buf[SFDP_RPMC_TABLE_CONFIG] & SFDP_RPMC_TABLE_CONFIG_RESERVED) !=
	    SFDP_RPMC_TABLE_CONFIG_RESERVED_VALUE ||
	    (buf[SFDP_RPMC_TABLE_UPDATE_RATE] & SFDP_RPMC_TABLE_UPDATE_RATE_RESERVED_MASK) !=
	    SFDP_RPMC_TABLE_UPDATE_RATE_RESERVED_VALUE ||
	    buf[SFDP_RPMC_TABLE_RESERVED_BYTE] != SFDP_RPMC_TABLE_RESERVED_BYTE_VALUE) {
		printk(BIOS_ERR, "Unexpected reserved values in RPMC table\n");
		return CB_ERR;
	}

	rpmc_info->flash_hardening = !!(buf[SFDP_RPMC_TABLE_CONFIG] &
					SFDP_RPMC_TABLE_CONFIG_FLASH_HARDENING_BIT);
	rpmc_info->monotonic_counter_size = (buf[SFDP_RPMC_TABLE_CONFIG] &
					    SFDP_RPMC_TABLE_CONFIG_FLASH_HARDENING_BIT) ?
					    SFDP_RPMC_COUNTER_BITS_RESERVED :
					    SFDP_RPMC_COUNTER_BITS_32;
	rpmc_info->busy_polling_method = (buf[SFDP_RPMC_TABLE_CONFIG] &
					 SFDP_RPMC_TABLE_CONFIG_BUSY_POLLING_METHOD) ?
					 SFDP_RPMC_POLL_READ_STATUS :
					 SFDP_RPMC_POLL_OP2_EXTENDED_STATUS;
	rpmc_info->number_of_counters = ((buf[SFDP_RPMC_TABLE_CONFIG] &
					SFDP_RPMC_TABLE_CONFIG_NUM_COUNTERS_MASK) >>
					SFDP_RPMC_TABLE_CONFIG_NUM_COUNTERS_SHIFT) + 1;
	rpmc_info->op1_write_command = buf[SFDP_RPMC_TABLE_RPMC_OP1];
	rpmc_info->op2_read_command = buf[SFDP_RPMC_TABLE_RPMC_OP2];
	rpmc_info->update_rate_s = calc_rpmc_update_rate_s(buf[SFDP_RPMC_TABLE_UPDATE_RATE] &
							   SFDP_RPMC_TABLE_UPDATE_RATE_MASK);
	rpmc_info->read_counter_polling_delay_us = calc_rpmc_short_delay_us(
				buf[SFDP_RPMC_TABLE_READ_COUNTER_POLLING_DELAY]);
	rpmc_info->write_counter_polling_short_delay_us = calc_rpmc_short_delay_us(
				buf[SFDP_RPMC_TABLE_WRITE_COUNTER_POLLING_SHORT_DELAY]);
	rpmc_info->write_counter_polling_long_delay_us = calc_rpmc_long_delay_us(
				buf[SFDP_RPMC_TABLE_WRITE_COUNTER_POLLING_LONG_DELAY]);
	return CB_SUCCESS;
}
