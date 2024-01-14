/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_SIFIVE_FU740_SPI_H__
#define __SOC_SIFIVE_FU740_SPI_H__

#include <spi-generic.h>

// Frame Format Register (fmt)

#define FU740_SPI_PROTO_S	0 // Data Pins: MOSI MISO
#define FU740_SPI_PROTO_D	1 // Data Pins: DQ0 DQ1
#define FU740_SPI_PROTO_Q	2 // Data Pins: DQ0 DQ1 DQ2 DQ3

#define FU740_SPI_ENDIAN_BIG	0 // send MSB first
#define FU740_SPI_ENDIAN_LITTLE	1 // send LSB first

// Serial Clock Mode Register (sckmode)

#define FU740_SPI_PHA_LOW	0 // inactive state of SCK is logical 0
#define FU740_SPI_PHA_HIGH	1 // inactive state of SCK is logical 1

#define FU740_SPI_POL_LEADING	0 // data is sampled on leading edge of sck
#define FU740_SPI_POL_TRAILING	1 // data is sampled on trailing edge of sck

// ffmt register (SPI Flash Instruction Format Register)
struct fu740_spi_ffmt_config {
	unsigned int cmd_en;     // enable sending of command
	unsigned int addr_len;   // number of address bytes (0-4)
	unsigned int pad_cnt;    // number of dummy cycles
	unsigned int cmd_proto;  // protocol for transmitting command
	unsigned int addr_proto; // protocol for transmitting address and padding
	unsigned int data_proto; // protocol for receiving data bytes
	unsigned int cmd_code;   // value of command byte
	unsigned int pad_code;   // first 8 bits to transmit during dummy cycles
};

struct fu740_spi_fmt_config {
	unsigned int protocol;   // FU740_SPI_PROTO_S, FU740_SPI_PROTO_D, FU740_SPI_PROTO_Q
	unsigned int endianness; // 0 = MSB, 1 = LSB
	unsigned int io_dir;     // Rx, Tx
	unsigned int bits_per_frame; // up to 8bits
};

struct fu740_spi_config {
	unsigned int freq; // speed of spi interface
	unsigned int pha;  // serial clock phase
	unsigned int pol;  // serial clock polarity
	struct fu740_spi_fmt_config fmt_config;   // frame format config
	struct fu740_spi_ffmt_config ffmt_config; // flash instruction format
};

extern struct fu740_spi_config fu740_spi_configs[];

int fu740_spi_setup(const struct spi_slave *slave);

#endif /* __SOC_SIFIVE_FU740_SPI_H__ */
