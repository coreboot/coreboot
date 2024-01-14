/* SPDX-License-Identifier: GPL-2.0-only */

//TODO needs work

#include <device/mmio.h>
#include <soc/spi.h>
#include <soc/clock.h>
#include <soc/addressmap.h>
#include <delay.h>
#include <lib.h>

#include "spi_internal.h"

static struct fu740_spi_ctrl *fu740_spi_ctrls[] = {
	(struct fu740_spi_ctrl *)FU740_QSPI0,
	(struct fu740_spi_ctrl *)FU740_QSPI1,
	(struct fu740_spi_ctrl *)FU740_QSPI2
};

// Wait until SPI is ready for transmission and transmit byte.
static void fu740_spi_tx(volatile struct fu740_spi_ctrl *spictrl_reg, uint8_t in)
{
#if __riscv_atomic
	int32_t r;
	do {
		asm volatile (
			"amoor.w %0, %2, %1\n"
			: "=r" (r), "+A" (spictrl_reg->txdata.raw_bits)
			: "r" (in)
		);
	} while (r < 0);
#else
	while ((int32_t) spictrl_reg->txdata.raw_bits < 0)
		;
	spictrl_reg->txdata.data = in;
#endif
}

// Wait until SPI receive queue has data and read byte.
static uint8_t fu740_spi_rx(volatile struct fu740_spi_ctrl *spictrl_reg)
{
	int32_t out;
	while ((out = (int32_t) spictrl_reg->rxdata.raw_bits) < 0)
		;
	return (uint8_t) out;
}

static int fu740_spi_xfer(const struct spi_slave *slave,
			  const void *dout, size_t bytesout,
			  void *din, size_t bytesin)
{
	printk(BIOS_DEBUG, "%s 0, bytesin: %zu, bytesout: %zu, din: %p\n", __func__, bytesin, bytesout, din);
	hexdump(dout, bytesout);
	struct fu740_spi_ctrl *spictrl_reg = fu740_spi_ctrls[slave->bus];
	union fu740_spi_reg_fmt fmt;
	fmt.raw_bits = read32(&spictrl_reg->fmt.raw_bits);
	if (fmt.proto == FU740_SPI_PROTO_S) {
		// working in full-duplex mode
		// receiving data needs to be triggered by sending data
		while (bytesout || bytesin) {
			uint8_t in, out = 0;
			if (bytesout) {
				out = *(uint8_t *)dout++;
				bytesout--;
			}
			fu740_spi_tx(spictrl_reg, out);
			in = fu740_spi_rx(spictrl_reg);
			if (bytesin) {
				*(uint8_t *)din++ = in;
				bytesin--;
			}
		}
	} else {
		// Working in half duplex
		// send and receive can be done separately
		if (dout && din)
			return -1;

		if (dout) {
			while (bytesout) {
				fu740_spi_tx(spictrl_reg, *(uint8_t *)dout++);
				bytesout--;
			}
		}

		if (din) {
			while (bytesin) {
				*(uint8_t *)din++ = fu740_spi_rx(spictrl_reg);
				bytesin--;
			}
		}
	}
	return 0;
}

static int fu740_spi_claim_bus(const struct spi_slave *slave)
{
	struct fu740_spi_ctrl *spictrl = fu740_spi_ctrls[slave->bus];
	union fu740_spi_reg_csmode csmode;
	csmode.raw_bits = 0;
	csmode.mode = FU740_SPI_CSMODE_HOLD;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);
	return 0;
}

static void fu740_spi_release_bus(const struct spi_slave *slave)
{
	struct fu740_spi_ctrl *spictrl = fu740_spi_ctrls[slave->bus];
	union fu740_spi_reg_csmode csmode;
	csmode.raw_bits = 0;
	csmode.mode = FU740_SPI_CSMODE_OFF;
	write32(&spictrl->csmode.raw_bits, csmode.raw_bits);
}

// reset spi flash chip
static void fu740_spi_reset(volatile struct fu740_spi_ctrl *spictrl_reg)
{
	fu740_spi_tx(spictrl_reg, 0x66);
	fu740_spi_tx(spictrl_reg, 0x99);
}

// setup the ffmt (SPI flash instruction format) register
__maybe_unused static int fu740_spi_setup_ffmt(volatile struct fu740_spi_ctrl *spictrl_reg,
			 const struct fu740_spi_config *config)
{
	//union fu740_spi_reg_fctrl fctrl;
	union fu740_spi_reg_ffmt ffmt;

	printk(BIOS_DEBUG, "config->data_proto: %d, config->cmd_code: %d\n",
			config->ffmt_config.data_proto,
			config->ffmt_config.cmd_code);

	//TODO test without this here
	fu740_spi_reset(spictrl_reg);

	ffmt.raw_bits   = 0;
	ffmt.cmd_en     = 1; // enable sending of command
	ffmt.addr_len   = 3; // number of address bytes (0-4)
	ffmt.pad_cnt    = 0; // number of dummy cycles TODO maybe not working
	ffmt.cmd_proto  = FU740_SPI_PROTO_S; // protocol for transmitting command
	ffmt.addr_proto = FU740_SPI_PROTO_S; // protocol for transmitting address and padding
	ffmt.data_proto = config->ffmt_config.data_proto; // protocol for receiving data bytes
	ffmt.cmd_code   = config->ffmt_config.cmd_code;   // value of command byte
	ffmt.pad_code   = 0; // First 8 bits to transmit during dummy cycles
	write32(&spictrl_reg->ffmt.raw_bits, ffmt.raw_bits);

	return 0;
}

int fu740_spi_setup(const struct spi_slave *slave)
{
	union fu740_spi_reg_csmode csmode;
	union fu740_spi_reg_fctrl fctrl;

	struct fu740_spi_ctrl *spictrl_reg = fu740_spi_ctrls[slave->bus];
	struct fu740_spi_config *config = &fu740_spi_configs[slave->bus];

	if ((config->pha > 1)
		|| (config->pol > 1)
		|| (config->fmt_config.protocol > 2)
		|| (config->fmt_config.endianness > 1)
		|| (config->fmt_config.bits_per_frame > 8))
		return -1;

	write32(&spictrl_reg->sckdiv, fu740_spi_min_clk_divisor(clock_get_pclk(), config->freq));

	/* disable direct memory-mapped spi flash mode */
	//TODO test if we need to disable it before changing the settings
	fctrl.raw_bits = 0;
	fctrl.en = 0;
	write32(&spictrl_reg->fctrl.raw_bits, fctrl.raw_bits);

	csmode.raw_bits = 0;
	csmode.mode = FU740_SPI_CSMODE_HOLD;
	write32(&spictrl_reg->csmode.raw_bits, csmode.raw_bits);
	char din[10];
	char dout[10] = { 0x66 };
	slave->ctrlr->xfer(slave, dout, 1, din, 0);
	dout[0] = 0x99;
	slave->ctrlr->xfer(slave, dout, 1, din, 0);
	int addr = 0x200;
	dout[0] = 0x03;
	dout[1] = (addr >> 16) & 0xFF;
	dout[2] = (addr >> 8) & 0xFF;
	dout[3] = addr & 0xFF;
	slave->ctrlr->xfer(slave, dout, 4, din, 10);
	csmode.mode = FU740_SPI_CSMODE_AUTO;
	write32(&spictrl_reg->csmode.raw_bits, csmode.raw_bits);
	din[9] = 0;
	return 0;
}

struct spi_ctrlr fu740_spi_ctrlr = {
	.xfer  = fu740_spi_xfer,
	.setup = fu740_spi_setup,
	.claim_bus = fu740_spi_claim_bus,
	.release_bus = fu740_spi_release_bus,
	.max_xfer_size = SPI_CTRLR_DEFAULT_MAX_XFER_SIZE,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.bus_start = 0,
		.bus_end = 2,
		.ctrlr = &fu740_spi_ctrlr,
	}
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
