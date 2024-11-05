/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <types.h>

#include "spi_flash_internal.h"

/*
 * Assumptions (not) made in the code:
 * - code is supposed to be endian-safe
 * - requested SFDP data blocks are small enough to fit into the SPI controller buffer, so no
 *   spi_crop_chunk calls are needed
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

static enum cb_err read_sfdp_data(const struct spi_flash *flash, uint32_t offset, size_t len,
				  uint8_t *buf)
{
	uint8_t buf_cmd[5];

	buf_cmd[0] = CMD_READ_SFDP;
	buf_cmd[1] = offset >> 16 & 0xff;
	buf_cmd[2] = offset >> 8 & 0xff;
	buf_cmd[3] = offset >> 0 & 0xff;
	buf_cmd[4] = 0; /* dummy byte */

	/* TODO: use spi_crop_chunk to break up larger transfers */
	if (spi_flash_cmd_multi(&flash->spi, buf_cmd, sizeof(buf_cmd), buf, len))
		return CB_ERR;

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

static inline enum cb_err find_sfdp_parameter_header(const struct spi_flash *flash,
						     uint16_t table_id, uint16_t *revision,
						     uint8_t *length_dwords,
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
