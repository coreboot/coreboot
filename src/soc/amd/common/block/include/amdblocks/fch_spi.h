/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Silverback Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _FCH_SPI_H_
#define _FCH_SPI_H_

#include <stdint.h>
#include <stddef.h>

#define GRANULARITY_TEST_4k		0x0000f000		/* bits 15-12 */
#define WORD_TO_DWORD_UPPER(x)		((x << 16) & 0xffff0000)
#define SPI_PAGE_WRITE			0x02
#define SPI_WRITE_ENABLE		0x06
/*
 * IDCODE_CONT_LEN may be redefined if a device needs to declare a
 * larger "shift" value.  IDCODE_PART_LEN generally shouldn't be
 * changed.  This is the max number of bytes probe functions may
 * examine when looking up part-specific identification info.
 */
#define IDCODE_CONT_CODE		0x7f
#define IDCODE_CONT_LEN			1 /* currently support only bank 0 */
#define IDCODE_PART_LEN			5
#define IDCODE_LEN (IDCODE_CONT_LEN + IDCODE_PART_LEN)

/* SPI MMIO registers */
#define SPI_RESTRICTED_CMD1		0x04
#define SPI_RESTRICTED_CMD2		0x08
#define SPI_CNTRL1			0x0c
#define SPI_CMD_CODE			0x45
#define SPI_CMD_TRIGGER			0x47
#define   SPI_CMD_TRIGGER_EXECUTE	BIT(7)
#define SPI_TX_BYTE_COUNT		0x48
#define SPI_RX_BYTE_COUNT		0x4b
#define SPI_STATUS			0x4c
#define   SPI_DONE_BYTE_COUNT_SHIFT	0
#define   SPI_DONE_BYTE_COUNT_MASK	0xff
#define   SPI_FIFO_WR_PTR_SHIFT		8
#define   SPI_FIFO_WR_PTR_MASK		0x7f
#define   SPI_FIFO_RD_PTR_SHIFT		16
#define   SPI_FIFO_RD_PTR_MASK		0x7f

/* Special SST write commands */
#define CMD_SST_BP			0x02	/* Byte Program */
#define CMD_SST_AAI_WP			0xad	/* Auto Address Increment Word Program */

#define SST_256				0x004b	/* Only SST that programs 256 bytes at once */

enum non_standard_spi {
	NON_STANDARD_SPI_NONE = 0,
	NON_STANDARD_SPI_SST,
};

struct spi_flash_table {
	const u8 shift;
	const u8 idcode;
	int (*probe)(const struct spi_slave *spi, u8 *idcode,
		      struct spi_flash *flash);
};

struct spi_data {
	const char *name;
	u32 size;
	u32 sector_size;
	u32 page_size;
	u8 status_cmd;
	u8 erase_cmd;
	u8 write_cmd;
	u8 write_enable_cmd;
	u8 read_cmd;
	u8 read_cmd_len;
	enum non_standard_spi non_standard;
};

void fch_spi_init(void);
void fch_spi_flash_ops_init(struct spi_flash *flash);
int fch_spi_flash_cmd(const void *dout, size_t bytes_out, void *din, size_t bytes_in);
int fch_spi_flash_cmd_write(const u8 *cmd, size_t cmd_len, const void *data, size_t data_len);
int fch_spi_wait_cmd_ready(unsigned long timeout);
int non_standard_sst_byte_write(u32 offset, const void *buf);
int non_standard_sst_write_aai(u32 offset, size_t len, const void *buf, size_t start);
const struct spi_flash_table *get_spi_flash_table(int *table_size);
const struct spi_data *get_ctrl_spi_data(void);

static inline int fch_spi_enable_write(void)
{
	u8 cmd_enable = SPI_WRITE_ENABLE;
	return fch_spi_flash_cmd(&cmd_enable, 1, NULL, 0);
}

#endif /* _FCH_SPI_H_ */
