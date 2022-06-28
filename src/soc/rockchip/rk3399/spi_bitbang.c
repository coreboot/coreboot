/* SPDX-License-Identifier: GPL-2.0-only */

/* Compile this driver in place of common/spi.c for bitbang testing.
   NOTE: Also need to adjust board-specific code for GPIO pinmux! */

#include <assert.h>
#include <gpio.h>
#include <soc/spi.h>
#include <spi_bitbang.h>
#include <spi_flash.h>
#include <stddef.h>

struct rockchip_bitbang_slave {
	struct spi_bitbang_ops ops;
	gpio_t miso;
	gpio_t mosi;
	gpio_t clk;
	gpio_t cs;
};

static int get_miso(const struct spi_bitbang_ops *ops)
{
	const struct rockchip_bitbang_slave *slave =
		container_of(ops, const struct rockchip_bitbang_slave, ops);
	return gpio_get(slave->miso);
}

static void set_mosi(const struct spi_bitbang_ops *ops, int value)
{
	const struct rockchip_bitbang_slave *slave =
		container_of(ops, const struct rockchip_bitbang_slave, ops);
	gpio_set(slave->mosi, value);
}

static void set_clk(const struct spi_bitbang_ops *ops, int value)
{
	const struct rockchip_bitbang_slave *slave =
		container_of(ops, const struct rockchip_bitbang_slave, ops);
	gpio_set(slave->clk, value);
}

static void set_cs(const struct spi_bitbang_ops *ops, int value)
{
	const struct rockchip_bitbang_slave *slave =
		container_of(ops, const struct rockchip_bitbang_slave, ops);
	gpio_set(slave->cs, value);
}

/* Can't use GPIO() here because of bug in GCC version used by ChromiumOS. */
static const struct rockchip_bitbang_slave slaves[] = {
	[0] = {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 3, .bank = GPIO_A, .idx = 4 },
		.mosi	= { .port = 3, .bank = GPIO_A, .idx = 5 },
		.clk	= { .port = 3, .bank = GPIO_A, .idx = 6 },
		.cs	= { .port = 3, .bank = GPIO_A, .idx = 7 },
	},
	[1]	= {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 1, .bank = GPIO_A, .idx = 7 },
		.mosi	= { .port = 1, .bank = GPIO_B, .idx = 0 },
		.clk	= { .port = 1, .bank = GPIO_B, .idx = 1 },
		.cs	= { .port = 1, .bank = GPIO_B, .idx = 2 },
	},
	[2]	= {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 2, .bank = GPIO_B, .idx = 1 },
		.mosi	= { .port = 2, .bank = GPIO_B, .idx = 2 },
		.clk	= { .port = 2, .bank = GPIO_B, .idx = 3 },
		.cs	= { .port = 2, .bank = GPIO_B, .idx = 4 },
	},
	[3]	= {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 1, .bank = GPIO_B, .idx = 7 },
		.mosi	= { .port = 1, .bank = GPIO_C, .idx = 0 },
		.clk	= { .port = 1, .bank = GPIO_C, .idx = 1 },
		.cs	= { .port = 1, .bank = GPIO_C, .idx = 2 },
	},
	[4]	= {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 3, .bank = GPIO_A, .idx = 0 },
		.mosi	= { .port = 3, .bank = GPIO_A, .idx = 1 },
		.clk	= { .port = 3, .bank = GPIO_A, .idx = 2 },
		.cs	= { .port = 3, .bank = GPIO_A, .idx = 3 },
	},
	[5]	= {
		.ops = { get_miso, set_mosi, set_clk, set_cs },
		.miso	= { .port = 2, .bank = GPIO_C, .idx = 4 },
		.mosi	= { .port = 2, .bank = GPIO_C, .idx = 5 },
		.clk	= { .port = 2, .bank = GPIO_C, .idx = 6 },
		.cs	= { .port = 2, .bank = GPIO_C, .idx = 7 },
	},
};

void rockchip_spi_init(unsigned int bus, unsigned int ignored_speed_hz)
{
	assert(bus >= 0 && bus < ARRAY_SIZE(slaves));

	gpio_output(slaves[bus].cs, 1);
	gpio_output(slaves[bus].clk, 0);
	gpio_input(slaves[bus].miso);
	gpio_output(slaves[bus].mosi, 0);
}

void rockchip_spi_set_sample_delay(unsigned int bus, unsigned int delay_ns)
{
	/* not supported, and not necessary for slow bitbang speeds */
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	assert(slave->bus >= 0 && slave->bus < ARRAY_SIZE(slaves));
	return spi_bitbang_claim_bus(&slaves[slave->bus].ops);
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	assert(slave->bus >= 0 && slave->bus < ARRAY_SIZE(slaves));
	spi_bitbang_release_bus(&slaves[slave->bus].ops);
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			  size_t bytes_out, void *din, size_t bytes_in)
{
	assert(slave->bus >= 0 && slave->bus < ARRAY_SIZE(slaves));
	return spi_bitbang_xfer(&slaves[slave->bus].ops,
				dout, bytes_out, din, bytes_in);
}

static const struct spi_ctrlr spi_ctrlr = {
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = 65535,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = ARRAY_SIZE(slaves) - 1,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
