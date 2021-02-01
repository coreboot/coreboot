/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <soc/clk.h>
#include <soc/gpio.h>
#include <soc/spi.h>
#include <stddef.h>
#include <stdint.h>
#include <symbols.h>

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "EXYNOS_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

static void exynos_spi_rx_tx(struct exynos_spi *regs, int todo,
			     void *dinp, void const *doutp, int i)
{
	int rx_lvl, tx_lvl;
	unsigned int *rxp = (unsigned int *)(dinp + (i * (32 * 1024)));
	unsigned int out_bytes, in_bytes;

	// TODO In current implementation, every read/write must be aligned to
	// 4 bytes, otherwise you may get timeout or other unexpected results.
	ASSERT(todo % 4 == 0);

	out_bytes = in_bytes = todo;
	setbits32(&regs->ch_cfg, SPI_CH_RST);
	clrbits32(&regs->ch_cfg, SPI_CH_RST);
	write32(&regs->pkt_cnt, ((todo * 8) / 32) | SPI_PACKET_CNT_EN);

	while (in_bytes) {
		uint32_t spi_sts;
		int temp;

		spi_sts = read32(&regs->spi_sts);
		rx_lvl = ((spi_sts >> 15) & 0x7f);
		tx_lvl = ((spi_sts >> 6) & 0x7f);
		while (tx_lvl < 32 && out_bytes) {
			// TODO The "writing" (tx) is not supported now; that's
			// why we write garbage to keep driving FIFO clock.
			temp = 0xffffffff;
			write32(&regs->tx_data, temp);
			out_bytes -= 4;
			tx_lvl += 4;
		}
		while (rx_lvl >= 4 && in_bytes) {
			temp = read32(&regs->rx_data);
			if (rxp)
				*rxp++ = temp;
			in_bytes -= 4;
			rx_lvl -= 4;
		}
	}
}

/* set up SPI channel */
int exynos_spi_open(struct exynos_spi *regs)
{
	/* set the spi1 GPIO */

	/* set pktcnt and enable it */
	write32(&regs->pkt_cnt, 4 | SPI_PACKET_CNT_EN);
	/* set FB_CLK_SEL */
	write32(&regs->fb_clk, SPI_FB_DELAY_180);
	/* set CH_WIDTH and BUS_WIDTH as word */
	setbits32(&regs->mode_cfg,
		     SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD);
	clrbits32(&regs->ch_cfg, SPI_CH_CPOL_L); /* CPOL: active high */

	/* clear rx and tx channel if set previously */
	clrbits32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits32(&regs->swap_cfg,
		  SPI_RX_SWAP_EN | SPI_RX_BYTE_SWAP | SPI_RX_HWORD_SWAP);

	/* do a soft reset */
	setbits32(&regs->ch_cfg, SPI_CH_RST);
	clrbits32(&regs->ch_cfg, SPI_CH_RST);

	/* now set rx and tx channel ON */
	setbits32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON | SPI_CH_HS_EN);
	return 0;
}

int exynos_spi_read(struct exynos_spi *regs, void *dest, u32 len, u32 off)
{
	int upto, todo;
	int i;
	clrbits32(&regs->cs_reg, SPI_SLAVE_SIG_INACT); /* CS low */

	/* Send read instruction (0x3h) followed by a 24 bit addr */
	write32(&regs->tx_data, (SF_READ_DATA_CMD << 24) | off);

	/* waiting for TX done */
	while (!(read32(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < len; upto += todo, i++) {
		todo = MIN(len - upto, (1 << 15));
		exynos_spi_rx_tx(regs, todo, dest, (void *)(off), i);
	}

	setbits32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	return len;
}

int exynos_spi_close(struct exynos_spi *regs)
{
	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits32(&regs->mode_cfg,
		  SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD);
	write32(&regs->swap_cfg, 0);

	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrsetbits32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits32(&regs->ch_cfg, SPI_CH_RST);
	clrbits32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
	return 0;
}

static struct exynos_spi *boot_slave_regs;

static ssize_t exynos_spi_readat(const struct region_device *rdev, void *dest,
					size_t offset, size_t count)
{
	int bytes;
	DEBUG_SPI("exynos_spi_cbfs_read(%u)\n", count);
	exynos_spi_open(boot_slave_regs);
	bytes = exynos_spi_read(boot_slave_regs, dest, count, offset);
	exynos_spi_close(boot_slave_regs);
	return bytes;
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
	MMAP_HELPER_REGION_INIT(&exynos_spi_ops, 0, CONFIG_ROM_SIZE);

void exynos_init_spi_boot_device(void)
{
	boot_slave_regs = (void *)EXYNOS5_SPI1_BASE;

	mmap_helper_device_init(&mdev, _cbfs_cache, REGION_SIZE(cbfs_cache));
}

const struct region_device *exynos_spi_boot_device(void)
{
	return &mdev.rdev;
}
