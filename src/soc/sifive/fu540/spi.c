/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 SiFive, Inc
 * Copyright (C) 2019 HardenedLinux
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

#include <device/mmio.h>
#include <soc/spi.h>
#include <soc/clock.h>
#include <soc/addressmap.h>

#include "spi_internal.h"

static struct spi_ctrl *spictrls[] = {
	(struct spi_ctrl *)FU540_QSPI0,
	(struct spi_ctrl *)FU540_QSPI1,
	(struct spi_ctrl *)FU540_QSPI2
};


/**
 * Wait until SPI is ready for transmission and transmit byte.
 */
static void spi_tx(volatile struct spi_ctrl *spictrl, uint8_t in)
{
#if __riscv_atomic
	int32_t r;
	do {
		asm volatile (
			"amoor.w %0, %2, %1\n"
			: "=r" (r), "+A" (spictrl->txdata.raw_bits)
			: "r" (in)
		);
	} while (r < 0);
#else
	while ((int32_t) spictrl->txdata.raw_bits < 0)
		;
	spictrl->txdata.data = in;
#endif
}


/**
 * Wait until SPI receive queue has data and read byte.
 */
static uint8_t spi_rx(volatile struct spi_ctrl *spictrl)
{
	int32_t out;
	while ((out = (int32_t) spictrl->rxdata.raw_bits) < 0)
		;
	return (uint8_t) out;
}

static int spi_claim_bus_(const struct spi_slave *slave)
{
	struct spi_ctrl *spictrl = spictrls[slave->bus];
	spi_reg_csmode csmode;
	csmode.raw_bits = 0;
	csmode.mode = FU540_SPI_CSMODE_HOLD;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);
	return 0;
}

static void spi_release_bus_(const struct spi_slave *slave)
{
	struct spi_ctrl *spictrl = spictrls[slave->bus];
	spi_reg_csmode csmode;
	csmode.raw_bits = 0;
	csmode.mode = FU540_SPI_CSMODE_OFF;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);
}

static int spi_xfer_(const struct spi_slave *slave,
		const void *dout, size_t bytesout,
		void *din, size_t bytesin)
{
	struct spi_ctrl *spictrl = spictrls[slave->bus];
	spi_reg_fmt fmt;
	fmt.raw_bits = read32(&spictrl->fmt.raw_bits);
	if (fmt.proto == FU540_SPI_PROTO_S) {
		/* working in full-duplex mode
		 * receiving data needs to be triggered by sending data */
		while (bytesout || bytesin) {
			uint8_t in, out = 0;
			if (bytesout) {
				out = *(uint8_t *)dout++;
				bytesout--;
			}
			spi_tx(spictrl, out);
			in = spi_rx(spictrl);
			if (bytesin) {
				*(uint8_t *)din++ = in;
				bytesin--;
			}
		}
	} else {
		/* Working in half duplex
		 * send and receive can be done separately */
		if (dout && din)
			return -1;

		if (dout) {
			while (bytesout) {
				spi_tx(spictrl, *(uint8_t *)dout++);
				bytesout--;
			}
		}

		if (din) {
			while (bytesin) {
				*(uint8_t *)din++ = spi_rx(spictrl);
				bytesin--;
			}
		}
	}
	return 0;
}

static int spi_setup_(const struct spi_slave *slave)
{
	spi_reg_sckmode sckmode;
	spi_reg_csmode csmode;
	spi_reg_fmt fmt;

	if ((slave->bus > 2) || (slave->cs != 0))
		return -1;

	struct spi_ctrl *spictrl = spictrls[slave->bus];

	write32(&spictrl->sckdiv, spi_min_clk_divisor(clock_get_tlclk_khz(),
				10000));

	sckmode.raw_bits = 0;
	sckmode.pha = FU540_SPI_PHA_LOW;
	sckmode.pol = FU540_SPI_POL_LEADING;
	write32(&spictrl->sckmode.raw_bits, sckmode.raw_bits);

	write32(&spictrl->csdef, 0xffffffff);

	csmode.raw_bits = 0;
	csmode.mode = FU540_SPI_CSMODE_AUTO;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);

	fmt.raw_bits = 0;
	fmt.proto = FU540_SPI_PROTO_S;
	fmt.endian = FU540_SPI_ENDIAN_BIG;
	fmt.dir = 0;
	fmt.len = 8;
	write32(&spictrl->fmt.raw_bits, fmt.raw_bits);

	return 0;
}

struct spi_ctrlr fu540_spi_ctrlr = {
	.xfer  = spi_xfer_,
	.setup = spi_setup_,
	.claim_bus = spi_claim_bus_,
	.release_bus = spi_release_bus_,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.bus_start = 0,
		.bus_end = 2,
		.ctrlr = &fu540_spi_ctrlr,
	}
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);

int fu540_spi_setup(unsigned int bus, unsigned int cs,
		struct spi_slave *slave,
		struct fu540_spi_config *config)
{
	spi_reg_sckmode sckmode;
	spi_reg_csmode csmode;
	spi_reg_fmt fmt;

	if ((bus > 2) || (cs != 0))
		return -1;

	if ((config->pha > 1)
		|| (config->pol > 1)
		|| (config->protocol > 2)
		|| (config->endianness > 1)
		|| (config->bits_per_frame > 8))
		return -1;

	slave->bus = bus;
	slave->cs = cs;
	slave->ctrlr = &fu540_spi_ctrlr;

	struct spi_ctrl *spictrl = spictrls[slave->bus];

	write32(&spictrl->sckdiv, spi_min_clk_divisor(clock_get_tlclk_khz(),
			config->freq / 1000));

	sckmode.raw_bits = 0;
	sckmode.pha = config->pha;
	sckmode.pol = config->pol;
	write32(&spictrl->sckmode.raw_bits, sckmode.raw_bits);

	write32(&spictrl->csdef, 0xffffffff);

	csmode.raw_bits = 0;
	csmode.mode = FU540_SPI_CSMODE_AUTO;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);

	fmt.raw_bits = 0;
	fmt.proto = config->protocol;
	fmt.endian = config->endianness;
	fmt.dir = 0;
	fmt.len = config->bits_per_frame;
	write32(&spictrl->fmt.raw_bits, fmt.raw_bits);

	return 0;
}

int fu540_spi_mmap(
		const struct spi_slave *slave,
		const struct fu540_spi_mmap_config *config)
{
	spi_reg_fctrl fctrl;
	spi_reg_ffmt ffmt;

	if (slave->bus > 2)
		return -1;

	if ((config->cmd_en > 1)
		|| (config->addr_len > 4)
		|| (config->pad_cnt > 15)
		|| (config->cmd_proto > 2)
		|| (config->addr_proto > 2)
		|| (config->data_proto > 2)
		|| (config->cmd_code > 255)
		|| (config->pad_code > 255))
		return -1;

	struct spi_ctrl *spictrl = spictrls[slave->bus];

	/* disable direct memory-mapped spi flash mode */
	fctrl.raw_bits = 0;
	fctrl.en = 0;
	write32(&spictrl->fctrl.raw_bits, fctrl.raw_bits);

	/* reset spi flash chip */
	spi_tx(spictrl, 0x66);
	spi_tx(spictrl, 0x99);

	/* Pass the information of the flash read operation to the spi
	 * controller */
	ffmt.raw_bits = 0;
	ffmt.cmd_en = config->cmd_en;
	ffmt.addr_len = config->addr_len;
	ffmt.pad_cnt = config->pad_cnt;
	ffmt.command_proto = config->cmd_proto;
	ffmt.addr_proto = config->addr_proto;
	ffmt.data_proto = config->data_proto;
	ffmt.command_code = config->cmd_code;
	ffmt.pad_code = config->pad_code;
	write32(&spictrl->ffmt.raw_bits, ffmt.raw_bits);

	/* enable direct memory-mapped spi flash mode */
	fctrl.raw_bits = 0;
	fctrl.en = 1;
	write32(&spictrl->fctrl.raw_bits, fctrl.raw_bits);

	return 0;
}
