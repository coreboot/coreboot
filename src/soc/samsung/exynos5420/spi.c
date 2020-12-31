/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/cpu.h>
#include <soc/spi.h>
#include <spi-generic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <symbols.h>

#define EXYNOS_SPI_MAX_TRANSFER_BYTES (65535)

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "EXYNOS_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

struct exynos_spi_slave {
	struct spi_slave slave;
	struct exynos_spi *regs;
	int initialized;
};

/* TODO(hungte) Move the SPI param list to per-board configuration, probably
 * Kconfig or mainboard.c */
static struct exynos_spi_slave exynos_spi_slaves[3] = {
	// SPI 0
	{
		.slave = { .bus = 0, },
		.regs = (void *)EXYNOS5_SPI0_BASE,
	},
	// SPI 1
	{
		.slave = { .bus = 1, },
		.regs = (void *)EXYNOS5_SPI1_BASE,
	},
	// SPI 2
	{
		.slave = { .bus = 2, },
		.regs = (void *)EXYNOS5_SPI2_BASE,
	},
};

static inline struct exynos_spi_slave *to_exynos_spi(const struct spi_slave *slave)
{
	return &exynos_spi_slaves[slave->bus];
}

static void spi_sw_reset(struct exynos_spi *regs, int word)
{
	const uint32_t orig_mode_cfg = read32(&regs->mode_cfg);
	uint32_t mode_cfg = orig_mode_cfg;
	const uint32_t orig_swap_cfg = read32(&regs->swap_cfg);
	uint32_t swap_cfg = orig_swap_cfg;

	mode_cfg &= ~(SPI_MODE_CH_WIDTH_MASK | SPI_MODE_BUS_WIDTH_MASK);
	if (word) {
		mode_cfg |= SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD;
		swap_cfg |= SPI_RX_SWAP_EN |
			    SPI_RX_BYTE_SWAP |
			    SPI_RX_HWORD_SWAP |
			    SPI_TX_SWAP_EN |
			    SPI_TX_BYTE_SWAP |
			    SPI_TX_HWORD_SWAP;
	} else {
		mode_cfg |= SPI_MODE_CH_WIDTH_BYTE | SPI_MODE_BUS_WIDTH_BYTE;
		swap_cfg = 0;
	}

	if (mode_cfg != orig_mode_cfg)
		write32(&regs->mode_cfg, mode_cfg);
	if (swap_cfg != orig_swap_cfg)
		write32(&regs->swap_cfg, swap_cfg);

	clrbits32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
	setbits32(&regs->ch_cfg, SPI_CH_RST);
	clrbits32(&regs->ch_cfg, SPI_CH_RST);
	setbits32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
}

static void exynos_spi_init(struct exynos_spi *regs)
{
	// Set FB_CLK_SEL.
	write32(&regs->fb_clk, SPI_FB_DELAY_180);
	// CPOL: Active high.
	clrbits32(&regs->ch_cfg, SPI_CH_CPOL_L);

	// Clear rx and tx channel if set previously.
	clrbits32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits32(&regs->swap_cfg,
		     SPI_RX_SWAP_EN | SPI_RX_BYTE_SWAP | SPI_RX_HWORD_SWAP);
	clrbits32(&regs->ch_cfg, SPI_CH_HS_EN);

	// Do a soft reset, which will also enable both channels.
	spi_sw_reset(regs, 1);
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	// TODO(hungte) Add some delay if too many transactions happen at once.
	clrbits32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
	return 0;
}

static void spi_transfer(struct exynos_spi *regs, void *in, const void *out,
			 size_t size)
{
	u8 *inb = in;
	const u8 *outb = out;

	size_t width = (size % 4) ? 1 : 4;

	while (size) {
		size_t packets = size / width;
		// The packet count field is 16 bits wide.
		packets = MIN(packets, (1 << 16) - 1);

		size_t out_bytes, in_bytes;
		out_bytes = in_bytes = packets * width;

		spi_sw_reset(regs, width == 4);
		write32(&regs->pkt_cnt, packets | SPI_PACKET_CNT_EN);

		while (out_bytes || in_bytes) {
			uint32_t spi_sts = read32(&regs->spi_sts);
			int rx_lvl = ((spi_sts >> 15) & 0x1ff);
			int tx_lvl = ((spi_sts >> 6) & 0x1ff);

			if (tx_lvl < 32 && tx_lvl < out_bytes) {
				uint32_t data = 0xffffffff;

				if (outb) {
					memcpy(&data, outb, width);
					outb += width;
				}
				write32(&regs->tx_data, data);

				out_bytes -= width;
			}

			if (rx_lvl >= width) {
				uint32_t data = read32(&regs->rx_data);

				if (inb) {
					memcpy(inb, &data, width);
					inb += width;
				}

				in_bytes -= width;
			}
		}

		size -= packets * width;
	}
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout, size_t bytes_out,
	     void *din, size_t bytes_in)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;

	if (bytes_out && bytes_in) {
		size_t min_size = MIN(bytes_out, bytes_in);

		spi_transfer(regs, din, dout, min_size);

		bytes_out -= min_size;
		bytes_in -= min_size;

		din = (uint8_t *)din + min_size;
		dout = (const uint8_t *)dout + min_size;
	}

	if (bytes_in)
		spi_transfer(regs, din, NULL, bytes_in);
	else if (bytes_out)
		spi_transfer(regs, NULL, dout, bytes_out);

	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	setbits32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	ASSERT(slave->bus < 3);
	struct exynos_spi_slave *eslave;

	eslave = to_exynos_spi(slave);
	if (!eslave->initialized) {
		exynos_spi_init(eslave->regs);
		eslave->initialized = 1;
	}
	return 0;
}

static const struct spi_ctrlr spi_ctrlr = {
	.setup = spi_ctrlr_setup,
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = SPI_CTRLR_DEFAULT_MAX_XFER_SIZE,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 2,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);

static int exynos_spi_read(struct spi_slave *slave, void *dest, uint32_t len,
			   uint32_t off)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	u32 command;
	spi_claim_bus(slave);

	// Send address.
	ASSERT(off < (1 << 24));
	command = htonl(SF_READ_DATA_CMD << 24 | off);
	spi_transfer(regs, NULL, &command, sizeof(command));

	// Read the data.
	spi_transfer(regs, dest, NULL, len);
	spi_release_bus(slave);

	return len;
}

static struct exynos_spi_slave *boot_slave;

static ssize_t exynos_spi_readat(const struct region_device *rdev, void *dest,
					size_t offset, size_t count)
{
	DEBUG_SPI("exynos_spi_cbfs_read(%u)\n", count);
	return exynos_spi_read(&boot_slave->slave, dest, count, offset);
}

static void *exynos_spi_map(const struct region_device *rdev,
					size_t offset, size_t count)
{
	DEBUG_SPI("exynos_spi_cbfs_map\n");
	// exynos: spi_rx_tx may work in 4 byte-width-transmission mode and
	// requires buffer memory address to be aligned.
	if (count % 4)
		count += 4 - (count % 4);
	return mmap_helper_rdev_mmap(rdev, offset, count);
}

static const struct region_device_ops exynos_spi_ops = {
	.mmap = exynos_spi_map,
	.munmap = mmap_helper_rdev_munmap,
	.readat = exynos_spi_readat,
};

static struct mmap_helper_region_device mdev =
	MMAP_HELPER_DEV_INIT(&exynos_spi_ops, 0, CONFIG_ROM_SIZE, &cbfs_cache);

void exynos_init_spi_boot_device(void)
{
	boot_slave = &exynos_spi_slaves[1];
}

const struct region_device *exynos_spi_boot_device(void)
{
	return &mdev.rdev;
}
