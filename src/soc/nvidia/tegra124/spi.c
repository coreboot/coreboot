/*
 * NVIDIA Tegra SPI controller (T114 and later)
 *
 * Copyright (c) 2010-2013 NVIDIA Corporation
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
#include <cbfs.h>
#include <cbfs_core.h>
#include <inttypes.h>
#include <spi-generic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <delay.h>

#include "dma.h"
#include "spi.h"

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "TEGRA_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

/*
 * 64 packets in FIFO mode, BLOCK_SIZE packets in DMA mode. Packets can vary
 * in size from 4 to 32 bits. To keep things simple we'll use 8-bit packets.
 */
#define SPI_PACKET_SIZE_BYTES		1
#define SPI_MAX_TRANSFER_BYTES_FIFO	(64 * SPI_PACKET_SIZE_BYTES)
#define SPI_MAX_TRANSFER_BYTES_DMA	((65536 * SPI_PACKET_SIZE_BYTES) - \
							TEGRA_DMA_ALIGN_BYTES)

/* COMMAND1 */
#define SPI_CMD1_GO			(1 << 31)
#define SPI_CMD1_M_S			(1 << 30)
#define SPI_CMD1_MODE_MASK		0x3
#define SPI_CMD1_MODE_SHIFT		28
#define SPI_CMD1_CS_SEL_MASK		0x3
#define SPI_CMD1_CS_SEL_SHIFT		26
#define SPI_CMD1_CS_POL_INACTIVE3	(1 << 25)
#define SPI_CMD1_CS_POL_INACTIVE2	(1 << 24)
#define SPI_CMD1_CS_POL_INACTIVE1	(1 << 23)
#define SPI_CMD1_CS_POL_INACTIVE0	(1 << 22)
#define SPI_CMD1_CS_SW_HW		(1 << 21)
#define SPI_CMD1_CS_SW_VAL		(1 << 20)
#define SPI_CMD1_IDLE_SDA_MASK		0x3
#define SPI_CMD1_IDLE_SDA_SHIFT		18
#define SPI_CMD1_BIDIR			(1 << 17)
#define SPI_CMD1_LSBI_FE		(1 << 16)
#define SPI_CMD1_LSBY_FE		(1 << 15)
#define SPI_CMD1_BOTH_EN_BIT		(1 << 14)
#define SPI_CMD1_BOTH_EN_BYTE		(1 << 13)
#define SPI_CMD1_RX_EN			(1 << 12)
#define SPI_CMD1_TX_EN			(1 << 11)
#define SPI_CMD1_PACKED			(1 << 5)
#define SPI_CMD1_BIT_LEN_MASK		0x1f
#define SPI_CMD1_BIT_LEN_SHIFT		0

/* COMMAND2 */
#define SPI_CMD2_TX_CLK_TAP_DELAY	(1 << 6)
#define SPI_CMD2_TX_CLK_TAP_DELAY_MASK	(0x3F << 6)
#define SPI_CMD2_RX_CLK_TAP_DELAY	(1 << 0)
#define SPI_CMD2_RX_CLK_TAP_DELAY_MASK	(0x3F << 0)

/* SPI_TRANS_STATUS */
#define SPI_STATUS_RDY			(1 << 30)
#define SPI_STATUS_SLV_IDLE_COUNT_MASK	0xff
#define SPI_STATUS_SLV_IDLE_COUNT_SHIFT	16
#define SPI_STATUS_BLOCK_COUNT		0xffff
#define SPI_STATUS_BLOCK_COUNT_SHIFT	0

/* SPI_FIFO_STATUS */
#define SPI_FIFO_STATUS_CS_INACTIVE	(1 << 31)
#define SPI_FIFO_STATUS_FRAME_END	(1 << 30)
#define SPI_FIFO_STATUS_RX_FIFO_FLUSH	(1 << 15)
#define SPI_FIFO_STATUS_TX_FIFO_FLUSH	(1 << 14)
#define SPI_FIFO_STATUS_ERR		(1 << 8)
#define SPI_FIFO_STATUS_TX_FIFO_OVF	(1 << 7)
#define SPI_FIFO_STATUS_TX_FIFO_UNR	(1 << 6)
#define SPI_FIFO_STATUS_RX_FIFO_OVF	(1 << 5)
#define SPI_FIFO_STATUS_RX_FIFO_UNR	(1 << 4)
#define SPI_FIFO_STATUS_TX_FIFO_FULL	(1 << 3)
#define SPI_FIFO_STATUS_TX_FIFO_EMPTY	(1 << 2)
#define SPI_FIFO_STATUS_RX_FIFO_FULL	(1 << 1)
#define SPI_FIFO_STATUS_RX_FIFO_EMPTY	(1 << 0)

/* SPI_DMA_CTL */
#define SPI_DMA_CTL_DMA			(1 << 31)
#define SPI_DMA_CTL_CONT		(1 << 30)
#define SPI_DMA_CTL_IE_RX		(1 << 29)
#define SPI_DMA_CTL_IE_TX		(1 << 28)
#define SPI_DMA_CTL_RX_TRIG_MASK	0x3
#define SPI_DMA_CTL_RX_TRIG_SHIFT	19
#define SPI_DMA_CTL_TX_TRIG_MASK	0x3
#define SPI_DMA_CTL_TX_TRIG_SHIFT	15

/* SPI_DMA_BLK */
#define SPI_DMA_CTL_BLOCK_SIZE_MASK	0xff
#define SPI_DMA_CTL_BLOCK_SIZE_SHIFT	0

struct tegra_spi_regs {
	u32 command1;		/* 0x000: SPI_COMMAND1 */
	u32 command2;		/* 0x004: SPI_COMMAND2 */
	u32 timing1;		/* 0x008: SPI_CS_TIM1 */
	u32 timing2;		/* 0x00c: SPI_CS_TIM2 */
	u32 trans_status;	/* 0x010: SPI_TRANS_STATUS */
	u32 fifo_status;	/* 0x014: SPI_FIFO_STATUS */
	u32 tx_data;		/* 0x018: SPI_TX_DATA */
	u32 rx_data;		/* 0x01c: SPI_RX_DATA */
	u32 dma_ctl;		/* 0x020: SPI_DMA_CTL */
	u32 dma_blk;		/* 0x024: SPI_DMA_BLK */
	u32 rsvd[56];		/* 0x028-0x107: reserved */
	u32 tx_fifo;		/* 0x108: SPI_FIFO1 */
	u32 rsvd2[31];		/* 0x10c-0x187 reserved */
	u32 rx_fifo;		/* 0x188: SPI_FIFO2 */
	u32 spare_ctl;		/* 0x18c: SPI_SPARE_CTRL */
} __attribute__((packed));

struct tegra_spi_channel {
	struct spi_slave slave;
	struct tegra_spi_regs *regs;
};

static struct tegra_spi_channel tegra_spi_channels[] = {
	/*
	 * Note: Tegra pinmux must be setup for corresponding SPI channel in
	 * order for its registers to be accessible. If pinmux has not been
	 * set up, access to the channel's registers will simply hang.
	 *
	 * TODO(dhendrix): Clarify or remove this comment (is clock setup
	 * necessary first, or just pinmux, or both?)
	 */
	{
		.slave = { .bus = 1, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI1_BASE,
	},
	{
		.slave = { .bus = 2, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI2_BASE,
	},
	{
		.slave = { .bus = 3, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI3_BASE,
	},
	{
		.slave = { .bus = 4, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI4_BASE,
	},
	{
		.slave = { .bus = 5, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI5_BASE,
	},
	{
		.slave = { .bus = 6, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI6_BASE,
	},
};

static void flush_fifos(struct tegra_spi_regs *regs)
{
	setbits_le32(&regs->fifo_status, SPI_FIFO_STATUS_RX_FIFO_FLUSH |
					SPI_FIFO_STATUS_TX_FIFO_FLUSH);
	while (read32(&regs->fifo_status) &
		(SPI_FIFO_STATUS_RX_FIFO_FLUSH | SPI_FIFO_STATUS_TX_FIFO_FLUSH))
		;
}

void tegra_spi_init(unsigned int bus)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(tegra_spi_channels); i++) {
		struct tegra_spi_regs *regs;

		if (tegra_spi_channels[i].slave.bus == bus)
			regs = tegra_spi_channels[i].regs;
		else
			continue;

		/* software drives chip-select, set value to high */
		setbits_le32(&regs->command1,
				SPI_CMD1_CS_SW_HW | SPI_CMD1_CS_SW_VAL);

		/* 8-bit transfers, unpacked mode, most significant bit first */
		clrbits_le32(&regs->command1,
				SPI_CMD1_BIT_LEN_MASK | SPI_CMD1_PACKED);
		setbits_le32(&regs->command1, 7 << SPI_CMD1_BIT_LEN_SHIFT);

		flush_fifos(regs);
	}
	printk(BIOS_INFO, "Tegra SPI bus %d initialized.\n", bus);

}

static struct tegra_spi_channel * const to_tegra_spi(int bus) {
	return &tegra_spi_channels[bus - 1];
}

static unsigned int tegra_spi_speed(unsigned int bus)
{
	/* FIXME: implement this properly, for now use max value (50MHz) */
	return 50000000;
}

/*
 * This calls udelay() with a calculated value based on the SPI speed and
 * number of bytes remaining to be transferred. It assumes that if the
 * calculated delay period is less than MIN_DELAY_US then it is probably
 * not worth the overhead of yielding.
 */
#define MIN_DELAY_US 250
static void tegra_spi_delay(struct tegra_spi_channel *spi,
				unsigned int bytes_remaining)
{
	unsigned int ns_per_byte, delay_us;

	ns_per_byte = 1000000000 / (tegra_spi_speed(spi->slave.bus) / 8);
	delay_us = (ns_per_byte * bytes_remaining) / 1000;

	if (delay_us < MIN_DELAY_US)
		return;

	udelay(delay_us);
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct tegra_spi_regs *regs = to_tegra_spi(slave->bus)->regs;
	u32 val;

	val = read32(&regs->command1);

	/* select appropriate chip-select line */
	val &= ~(SPI_CMD1_CS_SEL_MASK << SPI_CMD1_CS_SEL_SHIFT);
	val |= (slave->cs << SPI_CMD1_CS_SEL_SHIFT);

	/* drive chip-select with the inverse of the "inactive" value */
	if (val & (SPI_CMD1_CS_POL_INACTIVE0 << slave->cs))
		val &= ~SPI_CMD1_CS_SW_VAL;
	else
		val |= SPI_CMD1_CS_SW_VAL;

	write32(val, &regs->command1);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct tegra_spi_regs *regs = to_tegra_spi(slave->bus)->regs;
	u32 val;

	val = read32(&regs->command1);

	if (val & (SPI_CMD1_CS_POL_INACTIVE0 << slave->cs))
		val |= SPI_CMD1_CS_SW_VAL;
	else
		val &= ~SPI_CMD1_CS_SW_VAL;

	write32(val, &regs->command1);
}

static void dump_fifo_status(struct tegra_spi_channel *spi)
{
	u32 status = read32(&spi->regs->fifo_status);

	printk(BIOS_INFO, "Raw FIFO status: 0x%08x\n", status);
	if (status & SPI_FIFO_STATUS_TX_FIFO_OVF)
		printk(BIOS_INFO, "\tTx overflow detected\n");
	if (status & SPI_FIFO_STATUS_TX_FIFO_UNR)
		printk(BIOS_INFO, "\tTx underrun detected\n");
	if (status & SPI_FIFO_STATUS_RX_FIFO_OVF)
		printk(BIOS_INFO, "\tRx overflow detected\n");
	if (status & SPI_FIFO_STATUS_RX_FIFO_UNR)
		printk(BIOS_INFO, "\tRx underrun detected\n");

	printk(BIOS_INFO, "TX_FIFO: 0x%08x, TX_DATA: 0x%08x\n",
		read32(&spi->regs->tx_fifo), read32(&spi->regs->tx_data));
	printk(BIOS_INFO, "RX_FIFO: 0x%08x, RX_DATA: 0x%08x\n",
		read32(&spi->regs->rx_fifo), read32(&spi->regs->rx_data));
}

static void clear_fifo_status(struct tegra_spi_channel *spi)
{
	clrbits_le32(&spi->regs->fifo_status,
				SPI_FIFO_STATUS_ERR |
				SPI_FIFO_STATUS_TX_FIFO_OVF |
				SPI_FIFO_STATUS_TX_FIFO_UNR |
				SPI_FIFO_STATUS_RX_FIFO_OVF |
				SPI_FIFO_STATUS_RX_FIFO_UNR);
}

static void dump_spi_regs(struct tegra_spi_channel *spi)
{
	printk(BIOS_INFO, "SPI regs:\n"
			"\tdma_blk: 0x%08x\n"
			"\tcommand1: 0x%08x\n"
			"\tdma_ctl: 0x%08x\n"
			"\ttrans_status: 0x%08x\n",
			read32(&spi->regs->dma_blk),
			read32(&spi->regs->command1),
			read32(&spi->regs->dma_ctl),
			read32(&spi->regs->trans_status));
}

static void dump_dma_regs(struct apb_dma_channel *dma)
{
	printk(BIOS_INFO, "DMA regs:\n"
			"\tahb_ptr: 0x%08x\n"
			"\tapb_ptr: 0x%08x\n"
			"\tahb_seq: 0x%08x\n"
			"\tapb_seq: 0x%08x\n"
			"\tcsr: 0x%08x\n"
			"\tcsre: 0x%08x\n"
			"\twcount: 0x%08x\n"
			"\tdma_byte_sta: 0x%08x\n"
			"\tword_transfer: 0x%08x\n",
			read32(&dma->regs->ahb_ptr),
			read32(&dma->regs->apb_ptr),
			read32(&dma->regs->ahb_seq),
			read32(&dma->regs->apb_seq),
			read32(&dma->regs->csr),
			read32(&dma->regs->csre),
			read32(&dma->regs->wcount),
			read32(&dma->regs->dma_byte_sta),
			read32(&dma->regs->word_transfer));
}

static void dump_regs(struct tegra_spi_channel *spi,
			struct apb_dma_channel *dma)
{
	if (dma)
		dump_dma_regs(dma);
	if (spi) {
		dump_spi_regs(spi);
		dump_fifo_status(spi);
	}
}

static int fifo_error(struct tegra_spi_channel *spi)
{
	return read32(&spi->regs->fifo_status) & SPI_FIFO_STATUS_ERR ? 1 : 0;
}

static inline unsigned int spi_byte_count(struct tegra_spi_channel *spi)
{
	/* FIXME: Make this take total packet size into account */
	return read32(&spi->regs->trans_status) &
		(SPI_STATUS_BLOCK_COUNT << SPI_STATUS_BLOCK_COUNT_SHIFT);
}

static int tegra_spi_fifo_receive(struct tegra_spi_channel *spi,
			u8 *din, unsigned int in_bytes)
{
	unsigned int received = 0, remaining = in_bytes;

	printk(BIOS_SPEW, "%s: Receiving %d bytes\n", __func__, in_bytes);
	setbits_le32(&spi->regs->command1, SPI_CMD1_RX_EN);

	while (remaining) {
		unsigned int from_fifo, count;

		from_fifo = MIN(in_bytes, SPI_MAX_TRANSFER_BYTES_FIFO);
		remaining -= from_fifo;

		/* BLOCK_SIZE in SPI_DMA_BLK register applies to both DMA and
		 * PIO transfers */
		write32(from_fifo - 1, &spi->regs->dma_blk);

		setbits_le32(&spi->regs->trans_status, SPI_STATUS_RDY);
		setbits_le32(&spi->regs->command1, SPI_CMD1_GO);

		while ((count = spi_byte_count(spi)) != from_fifo) {
			tegra_spi_delay(spi, from_fifo - count);
			if (fifo_error(spi))
				goto done;
		}

		received += from_fifo;
		while (from_fifo) {
			*din = read8(&spi->regs->rx_fifo);
			din++;
			from_fifo--;
		}
	}

done:
	clrbits_le32(&spi->regs->command1, SPI_CMD1_RX_EN);
	if ((received != in_bytes) || fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR: Received %u bytes, expected %u\n",
				__func__, received, in_bytes);
		dump_regs(spi, NULL);
		return -1;
	}
	return in_bytes;
}

static int tegra_spi_fifo_send(struct tegra_spi_channel *spi,
			const u8 *dout, unsigned int out_bytes)
{
	unsigned int sent = 0, remaining = out_bytes;

	printk(BIOS_SPEW, "%s: Sending %d bytes\n", __func__, out_bytes);
	setbits_le32(&spi->regs->command1, SPI_CMD1_TX_EN);

	while (remaining) {
		unsigned int to_fifo, tmp;

		to_fifo = MIN(out_bytes, SPI_MAX_TRANSFER_BYTES_FIFO);

		/* BLOCK_SIZE in SPI_DMA_BLK register applies to both DMA and
		 * PIO transfers */
		write32(to_fifo - 1, &spi->regs->dma_blk);

		tmp = to_fifo;
		while (tmp) {
			write32(*dout, &spi->regs->tx_fifo);
			dout++;
			tmp--;
		}

		setbits_le32(&spi->regs->trans_status, SPI_STATUS_RDY);
		setbits_le32(&spi->regs->command1, SPI_CMD1_GO);

		while (!(read32(&spi->regs->fifo_status) &
				SPI_FIFO_STATUS_TX_FIFO_EMPTY)) {
			tegra_spi_delay(spi, to_fifo - spi_byte_count(spi));
			if (fifo_error(spi))
				goto done;
		}

		remaining -= to_fifo;
		sent += to_fifo;
	}

done:
	clrbits_le32(&spi->regs->command1, SPI_CMD1_TX_EN);
	if ((sent != out_bytes) || fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR: Sent %u bytes, expected "
				"to send %u\n", __func__, sent, out_bytes);
		dump_regs(spi, NULL);
		return -1;
	}
	return out_bytes;
}

static void tegra2_spi_dma_setup(struct apb_dma_channel *dma)
{
	/* APB bus width = 8-bits, address wrap for each word */
	clrbits_le32(&dma->regs->apb_seq, 0x7 << 28);
	/* AHB 1 word burst, bus width = 32 bits (fixed in hardware),
	 * no address wrapping */
	clrsetbits_le32(&dma->regs->ahb_seq,
			(0x7 << 24) | (0x7 << 16), 0x4 << 24);
	/* Set ONCE mode to transfer one "blocK" at a time (64KB). */
	setbits_le32(&dma->regs->csr, 1 << 27);
}

/*
 * Notes for DMA transmit and receive, experimentally determined (need to
 * verify):
 * - WCOUNT seems to be an "n-1" count, but the documentation does not
 *   make this clear. Without the -1 dma_byte_sta will show 1 AHB word
 *   (4 bytes) higher than it should and Tx overrun / Rx underrun will
 *   likely occur.
 *
 * - dma_byte_sta is always a multiple 4, so we check for
 *   dma_byte_sta < length
 *
 * - The RDY bit in SPI_TRANS_STATUS needs to be cleared manually
 *   (set bit to clear) between each transaction. Otherwise the next
 *   transaction does not start.
 */

static int tegra_spi_dma_receive(struct tegra_spi_channel *spi,
		const void *din, unsigned int in_bytes)
{
	struct apb_dma_channel *dma;

	dma = dma_claim();
	if (!dma) {
		printk(BIOS_ERR, "%s: Unable to claim DMA channel\n", __func__);
		return -1;
	}

	printk(BIOS_SPEW, "%s: Receiving %d bytes\n", __func__, in_bytes);
	tegra2_spi_dma_setup(dma);

	/* set AHB & APB address pointers */
	write32((u32)din, &dma->regs->ahb_ptr);
	write32((u32)&spi->regs->rx_fifo, &dma->regs->apb_ptr);

	setbits_le32(&spi->regs->command1, SPI_CMD1_RX_EN);

	/* FIXME: calculate word count so that it corresponds to bus width */
	write32(in_bytes - 1, &dma->regs->wcount);

	/* specify BLOCK_SIZE in SPI_DMA_BLK */
	write32(in_bytes - 1, &spi->regs->dma_blk);

	/* Set DMA direction for APB (SPI) --> AHB (DRAM) */
	clrbits_le32(&dma->regs->csr, 1 << 28);

	/* write to SPI_TRANS_STATUS RDY bit to clear it */
	setbits_le32(&spi->regs->trans_status, SPI_STATUS_RDY);

	/* set DMA bit in SPI_DMA_CTL to start */
	setbits_le32(&spi->regs->dma_ctl, SPI_DMA_CTL_DMA);

	/* start APBDMA after SPI DMA so we don't read empty bytes
	 * from Rx FIFO */
	dma_start(dma);

	while (spi_byte_count(spi) != in_bytes)
		tegra_spi_delay(spi, in_bytes - spi_byte_count(spi));
	clrbits_le32(&spi->regs->command1, SPI_CMD1_RX_EN);

	while ((read32(&dma->regs->dma_byte_sta) < in_bytes) || dma_busy(dma))
		;	/* this shouldn't take long, no udelay */
	dma_stop(dma);
	dma_release(dma);

	if ((spi_byte_count(spi) != in_bytes) || fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR: Received %u bytes, expected %u\n",
				__func__, spi_byte_count(spi), in_bytes);
		dump_regs(spi, dma);
		return -1;
	}

	return in_bytes;
}

static int tegra_spi_dma_send(struct tegra_spi_channel *spi,
		const u8 *dout, unsigned int out_bytes)
{
	struct apb_dma_channel *dma;
	unsigned int count;

	dma = dma_claim();
	if (!dma) {
		printk(BIOS_ERR, "%s: Unable to claim DMA channel\n", __func__);
		return -1;
	}

	printk(BIOS_SPEW, "%s: Sending %d bytes\n", __func__, out_bytes);
	tegra2_spi_dma_setup(dma);

	/* set AHB & APB address pointers */
	write32((u32)dout, &dma->regs->ahb_ptr);
	write32((u32)&spi->regs->tx_fifo, &dma->regs->apb_ptr);

	setbits_le32(&spi->regs->command1, SPI_CMD1_TX_EN);

	/* FIXME: calculate word count so that it corresponds to bus width */
	write32(out_bytes - 1, &dma->regs->wcount);

	/* specify BLOCK_SIZE in SPI_DMA_BLK */
	write32(out_bytes - 1, &spi->regs->dma_blk);

	/* Set DMA direction for AHB (DRAM) --> APB (SPI) */
	setbits_le32(&dma->regs->csr, (1 << 28));

	/* write to SPI_TRANS_STATUS RDY bit to clear it */
	setbits_le32(&spi->regs->trans_status, SPI_STATUS_RDY);

	dma_start(dma);
	/* set DMA bit in SPI_DMA_CTL to start */
	setbits_le32(&spi->regs->dma_ctl, SPI_DMA_CTL_DMA);

	while ((read32(&dma->regs->dma_byte_sta) < out_bytes) || dma_busy(dma))
		tegra_spi_delay(spi, out_bytes - spi_byte_count(spi));
	dma_stop(dma);

	while ((count = spi_byte_count(spi)) != out_bytes)
		tegra_spi_delay(spi, out_bytes - count);
	clrbits_le32(&spi->regs->command1, SPI_CMD1_TX_EN);

	dma_release(dma);

	if ((spi_byte_count(spi) != out_bytes) || fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR: Sent %u bytes, expected %u\n",
				__func__, spi_byte_count(spi), out_bytes);
		dump_regs(spi, dma);
		return -1;
	}

	return out_bytes;
}

int spi_xfer(struct spi_slave *slave, const void *dout, unsigned int bitsout,
	     void *din, unsigned int bitsin)
{
	unsigned int out_bytes = bitsout / 8, in_bytes = bitsin / 8;
	struct tegra_spi_channel *spi = to_tegra_spi(slave->bus);
	int ret = 0;
	u8 *out_buf = (u8 *)dout;
	u8 *in_buf = (u8 *)din;

	ASSERT(bitsout % 8 == 0 && bitsin % 8 == 0);

	/* tegra bus numbers start at 1 */
	ASSERT(slave->bus >= 1 && slave->bus <= ARRAY_SIZE(tegra_spi_channels));

	flush_fifos(spi->regs);

	/*
	 * DMA operates on 4 bytes at a time, so to avoid accessing memory
	 * outside the specified buffers we'll only use DMA for 4-byte aligned
	 * transactions accesses and transfer remaining bytes manually using
	 * the Rx/Tx FIFOs.
	 */

	while (out_bytes > 0) {
		unsigned int dma_out, fifo_out;

		dma_out = MIN(out_bytes, SPI_MAX_TRANSFER_BYTES_DMA);
		fifo_out = dma_out % TEGRA_DMA_ALIGN_BYTES;
		dma_out -= fifo_out;

		if (dma_out) {
			ret = tegra_spi_dma_send(spi, out_buf, dma_out);
			if (ret != dma_out) {
				ret = -1;
				goto spi_xfer_exit;
			}
			out_buf += dma_out;
			out_bytes -= dma_out;
		}
		if (fifo_out) {
			ret = tegra_spi_fifo_send(spi, out_buf, fifo_out);
			if (ret != fifo_out) {
				ret = -1;
				goto spi_xfer_exit;
			}
			out_buf += fifo_out;
			out_bytes -= fifo_out;
		}
	}

	while (in_bytes > 0) {
		unsigned int dma_in, fifo_in;

		dma_in = MIN(in_bytes, SPI_MAX_TRANSFER_BYTES_DMA);
		fifo_in = dma_in % TEGRA_DMA_ALIGN_BYTES;
		dma_in -= fifo_in;

		if (dma_in) {
			ret = tegra_spi_dma_receive(spi, in_buf, dma_in);
			if (ret != dma_in) {
				ret = -1;
				goto spi_xfer_exit;
			}
			in_buf += dma_in;
			in_bytes -= dma_in;
		}
		if (fifo_in) {
			ret = tegra_spi_fifo_receive(spi, in_buf, fifo_in);
			if (ret != fifo_in) {
				ret = -1;
				goto spi_xfer_exit;
			}
			in_buf += fifo_in;
			in_bytes -= fifo_in;
		}
	}

	ret = 0;

spi_xfer_exit:
	if (ret < 0)
		clear_fifo_status(spi);
	return ret;
}

/* SPI as CBFS media. */
struct tegra_spi_media {
	struct spi_slave *slave;
	struct cbfs_simple_buffer buffer;
};

static int tegra_spi_cbfs_open(struct cbfs_media *media)
{
	DEBUG_SPI("tegra_spi_cbfs_open\n");
	return 0;
}

static int tegra_spi_cbfs_close(struct cbfs_media *media)
{
	DEBUG_SPI("tegra_spi_cbfs_close\n");
	return 0;
}

#define JEDEC_READ		0x03
#define JEDEC_READ_OUTSIZE	0x04
/*      JEDEC_READ_INSIZE : any length */

static size_t tegra_spi_cbfs_read(struct cbfs_media *media, void *dest,
				   size_t offset, size_t count)
{
	struct tegra_spi_media *spi = (struct tegra_spi_media *)media->context;
	u8 spi_read_cmd[JEDEC_READ_OUTSIZE];
	int ret = count;

	/* TODO: Dual mode (BOTH_EN_BIT) and packed mode */
	spi_read_cmd[0] = JEDEC_READ;
	spi_read_cmd[1] = (offset >> 16) & 0xff;
	spi_read_cmd[2] = (offset >> 8) & 0xff;
	spi_read_cmd[3] = offset & 0xff;

	/* assert /CS */
	spi_cs_activate(spi->slave);

	if (spi_xfer(spi->slave, spi_read_cmd,
			sizeof(spi_read_cmd) * 8, NULL, 0) < 0) {
		ret = -1;
		printk(BIOS_ERR, "%s: Failed to transfer %u bytes\n",
				__func__, sizeof(spi_read_cmd));
		goto tegra_spi_cbfs_read_exit;
	}

	if (spi_xfer(spi->slave, NULL, 0, dest, count * 8)) {
		ret = -1;
		printk(BIOS_ERR, "%s: Failed to transfer %u bytes\n",
				__func__, count);
	}

tegra_spi_cbfs_read_exit:
	/* de-assert /CS */
	spi_cs_deactivate(spi->slave);
	return (ret < 0) ? 0 : ret;
}

static void *tegra_spi_cbfs_map(struct cbfs_media *media, size_t offset,
				 size_t count)
{
	struct tegra_spi_media *spi = (struct tegra_spi_media*)media->context;
	void *map;
	DEBUG_SPI("tegra_spi_cbfs_map\n");
	map = cbfs_simple_buffer_map(&spi->buffer, media, offset, count);
	printk(BIOS_INFO, "%s: map: 0x%p\n", __func__, map);
	return map;
}

static void *tegra_spi_cbfs_unmap(struct cbfs_media *media,
				   const void *address)
{
	struct tegra_spi_media *spi = (struct tegra_spi_media*)media->context;
	DEBUG_SPI("tegra_spi_cbfs_unmap\n");
	return cbfs_simple_buffer_unmap(&spi->buffer, address);
}

int initialize_tegra_spi_cbfs_media(struct cbfs_media *media,
				     void *buffer_address,
				     size_t buffer_size)
{
	// TODO Replace static variable to support multiple streams.
	static struct tegra_spi_media context;
	static struct tegra_spi_channel *channel;

	channel = &tegra_spi_channels[CONFIG_BOOT_MEDIA_SPI_BUS - 1];
	channel->slave.cs = CONFIG_BOOT_MEDIA_SPI_CHIP_SELECT;

	DEBUG_SPI("Initializing CBFS media on SPI\n");

	context.slave = &channel->slave;
	context.buffer.allocated = context.buffer.last_allocate = 0;
	context.buffer.buffer = buffer_address;
	context.buffer.size = buffer_size;
	media->context = (void*)&context;
	media->open = tegra_spi_cbfs_open;
	media->close = tegra_spi_cbfs_close;
	media->read = tegra_spi_cbfs_read;
	media->map = tegra_spi_cbfs_map;
	media->unmap = tegra_spi_cbfs_unmap;

	return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	struct tegra_spi_channel *channel = to_tegra_spi(bus);
	if (!channel)
		return NULL;

	return &channel->slave;
}

int spi_claim_bus(struct spi_slave *slave)
{
	tegra_spi_init(slave->bus);
	spi_cs_activate(slave);
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	spi_cs_deactivate(slave);
}
