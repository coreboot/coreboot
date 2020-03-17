/*
 * This file is part of the coreboot project.
 *
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

#ifndef __SOC_SIFIVE_HIFIVE_U_SPI_H__
#define __SOC_SIFIVE_HIFIVE_U_SPI_H__
#include <spi-generic.h>

/* Data Pins: MOSI MISO */
#define FU540_SPI_PROTO_S	0
/* Data Pins: DQ0 DQ1 */
#define FU540_SPI_PROTO_D	1
/* Data Pins: DQ0 DQ1 DQ2 DQ3 */
#define FU540_SPI_PROTO_Q	2

/* send MSB first */
#define FU540_SPI_ENDIAN_BIG	0
/* send LSB first */
#define FU540_SPI_ENDIAN_LITTLE	1

/* inactive state of SCK is logical 0 */
#define FU540_SPI_PHA_LOW	0
/* inactive state of SCK is logical 1 */
#define FU540_SPI_PHA_HIGH	1

/* data is sampled on leading edge */
#define FU540_SPI_POL_LEADING	0
/* data is sampled on trailing edge */
#define FU540_SPI_POL_TRAILING	1

struct fu540_spi_config {
	/* speed of spi interface */
	unsigned int freq;
	/* serial clock phase */
	unsigned int pha;
	/* serial clock polarity */
	unsigned int pol;
	unsigned int protocol;
	unsigned int endianness;
	/* up to 8bits */
	unsigned int bits_per_frame;
};

/* more detailed spi configuration */
int fu540_spi_setup(unsigned int bus, unsigned int cs, struct spi_slave *slave,
		struct fu540_spi_config *config);

/* This structure is used to describe the read command of SPI FLASH. */
struct fu540_spi_mmap_config {
	/* enable sending of command */
	unsigned int cmd_en;
	/* number of address bytes (0-4) */
	unsigned int addr_len;
	/* number of dummy cycles */
	unsigned int pad_cnt;
	/* protocol for transmitting command */
	unsigned int cmd_proto;
	/* protocol for transmitting address and padding */
	unsigned int addr_proto;
	/* protocol for receiving data bytes */
	unsigned int data_proto;
	/* value of command byte */
	unsigned int cmd_code;
	/* first 8 bits to transmit during dummy cycles */
	unsigned int pad_code;
};

int fu540_spi_mmap(
		const struct spi_slave *slave,
		const struct fu540_spi_mmap_config *config);

#endif /* __SOC_SIFIVE_HIFIVE_U_SPI_H__ */
