/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <spi_bitbang.h>
#include <types.h>

/* Set to 1 to dump all SPI transfers to the UART. */
#define TRACE		0
/*
 * In theory, this should work fine with 0 delay since the bus is fully clocked
 * by the master and the slave just needs to follow clock transitions whenever
 * they happen. We're not going to be "too fast" by bit-banging anyway. However,
 * if something doesn't work right, try increasing this value to slow it down.
 */
#define HALF_CLOCK_US	0

int spi_bitbang_claim_bus(const struct spi_bitbang_ops *ops)
{
	ops->set_cs(ops, 0);
	return 0;
}

void spi_bitbang_release_bus(const struct spi_bitbang_ops *ops)
{
	ops->set_cs(ops, 1);
}

/* Implements a CPOL=0, CPH=0, MSB first 8-bit controller. */
int spi_bitbang_xfer(const struct spi_bitbang_ops *ops, const void *dout,
		     size_t bytes_out, void *din, size_t bytes_in)
{
	if (TRACE) {
		if (bytes_in && bytes_out)
			printk(BIOS_SPEW, "!");
		else if (bytes_in)
			printk(BIOS_SPEW, "<");
		else if (bytes_out)
			printk(BIOS_SPEW, ">");
	}

	while (bytes_out || bytes_in) {
		int i;
		uint8_t in_byte = 0, out_byte = 0;
		if (bytes_out) {
			out_byte = *(const uint8_t *)dout++;
			bytes_out--;
			if (TRACE)
				printk(BIOS_SPEW, "%02x", out_byte);
		}
		for (i = 7; i >= 0; i--) {
			ops->set_mosi(ops, !!(out_byte & (1 << i)));
			if (HALF_CLOCK_US)
				udelay(HALF_CLOCK_US);
			ops->set_clk(ops, 1);
			in_byte |= !!ops->get_miso(ops) << i;
			if (HALF_CLOCK_US)
				udelay(HALF_CLOCK_US);
			ops->set_clk(ops, 0);
		}
		if (bytes_in) {
			*(uint8_t *)din++ = in_byte;
			bytes_in--;
			if (TRACE)
				printk(BIOS_SPEW, "%02x", in_byte);
		}
	}

	if (TRACE)
		printk(BIOS_SPEW, "\n");
	return 0;
}
