/* SPDX-License-Identifier: GPL-2.0-only */
/* NVIDIA Tegra SPI controller (T114 and later) */

#include <arch/cache.h>
#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <soc/addressmap.h>
#include <soc/dma.h>
#include <soc/spi.h>
#include <symbols.h>
#include <types.h>

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
#define SPI_MAX_TRANSFER_BYTES_DMA	(65535 * SPI_PACKET_SIZE_BYTES)

/*
 * This is used to workaround an issue seen where it may take some time for
 * packets to show up in the FIFO after they have been received and the
 * BLOCK_COUNT has been incremented.
 */
#define SPI_FIFO_XFER_TIMEOUT_US	1000

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
#define SPI_FIFO_STATUS_CS_INACTIVE			(1 << 31)
#define SPI_FIFO_STATUS_FRAME_END			(1 << 30)
#define SPI_FIFO_STATUS_RX_FIFO_FULL_COUNT_MASK		0x7f
#define SPI_FIFO_STATUS_RX_FIFO_FULL_COUNT_SHIFT	23
#define SPI_FIFO_STATUS_TX_FIFO_EMPTY_COUNT_MASK	0x7f
#define SPI_FIFO_STATUS_TX_FIFO_EMPTY_COUNT_SHIFT	16
#define SPI_FIFO_STATUS_RX_FIFO_FLUSH			(1 << 15)
#define SPI_FIFO_STATUS_TX_FIFO_FLUSH			(1 << 14)
#define SPI_FIFO_STATUS_ERR				(1 << 8)
#define SPI_FIFO_STATUS_TX_FIFO_OVF			(1 << 7)
#define SPI_FIFO_STATUS_TX_FIFO_UNR			(1 << 6)
#define SPI_FIFO_STATUS_RX_FIFO_OVF			(1 << 5)
#define SPI_FIFO_STATUS_RX_FIFO_UNR			(1 << 4)
#define SPI_FIFO_STATUS_TX_FIFO_FULL			(1 << 3)
#define SPI_FIFO_STATUS_TX_FIFO_EMPTY			(1 << 2)
#define SPI_FIFO_STATUS_RX_FIFO_FULL			(1 << 1)
#define SPI_FIFO_STATUS_RX_FIFO_EMPTY			(1 << 0)

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
#define SPI_DMA_CTL_BLOCK_SIZE_MASK	0xffff
#define SPI_DMA_CTL_BLOCK_SIZE_SHIFT	0

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
		.req_sel = APBDMA_SLAVE_SL2B1,
	},
	{
		.slave = { .bus = 2, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI2_BASE,
		.req_sel = APBDMA_SLAVE_SL2B2,
	},
	{
		.slave = { .bus = 3, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI3_BASE,
		.req_sel = APBDMA_SLAVE_SL2B3,
	},
	{
		.slave = { .bus = 4, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI4_BASE,
		.req_sel = APBDMA_SLAVE_SL2B4,
	},
	{
		.slave = { .bus = 5, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI5_BASE,
		.req_sel = APBDMA_SLAVE_SL2B5,
	},
	{
		.slave = { .bus = 6, },
		.regs = (struct tegra_spi_regs *)TEGRA_SPI6_BASE,
		.req_sel = APBDMA_SLAVE_SL2B6,
	},
	{
		.slave = { .bus = 7, },
		.regs = (struct tegra_spi_regs *)TEGRA_QSPI_BASE,
		.req_sel = APBDMA_SLAVE_QSPI,
	},
};

enum spi_direction {
	SPI_SEND,
	SPI_RECEIVE,
};

struct tegra_spi_channel *tegra_spi_init(unsigned int bus)
{
	int i;
	struct tegra_spi_channel *spi = NULL;

	for (i = 0; i < ARRAY_SIZE(tegra_spi_channels); i++) {
		if (tegra_spi_channels[i].slave.bus == bus) {
			spi = &tegra_spi_channels[i];
			break;
		}
	}
	if (!spi)
		return NULL;

	/* software drives chip-select, set value to high */
	setbits32(&spi->regs->command1,
			SPI_CMD1_CS_SW_HW | SPI_CMD1_CS_SW_VAL);

	/* 8-bit transfers, unpacked mode, most significant bit first */
	clrbits32(&spi->regs->command1,
			SPI_CMD1_BIT_LEN_MASK | SPI_CMD1_PACKED);
	setbits32(&spi->regs->command1, 7 << SPI_CMD1_BIT_LEN_SHIFT);

	return spi;
}

static struct tegra_spi_channel * const to_tegra_spi(int bus) {
	return &tegra_spi_channels[bus - 1];
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct tegra_spi_regs *regs = to_tegra_spi(slave->bus)->regs;
	u32 val;

	tegra_spi_init(slave->bus);

	val = read32(&regs->command1);

	/* select appropriate chip-select line */
	val &= ~(SPI_CMD1_CS_SEL_MASK << SPI_CMD1_CS_SEL_SHIFT);
	val |= (slave->cs << SPI_CMD1_CS_SEL_SHIFT);

	/* drive chip-select with the inverse of the "inactive" value */
	if (val & (SPI_CMD1_CS_POL_INACTIVE0 << slave->cs))
		val &= ~SPI_CMD1_CS_SW_VAL;
	else
		val |= SPI_CMD1_CS_SW_VAL;

	write32(&regs->command1, val);
	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct tegra_spi_regs *regs = to_tegra_spi(slave->bus)->regs;
	u32 val;

	val = read32(&regs->command1);

	if (val & (SPI_CMD1_CS_POL_INACTIVE0 << slave->cs))
		val |= SPI_CMD1_CS_SW_VAL;
	else
		val &= ~SPI_CMD1_CS_SW_VAL;

	write32(&regs->command1, val);
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
	clrbits32(&spi->regs->fifo_status,
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
	if (dma) {
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
}

static inline unsigned int spi_byte_count(struct tegra_spi_channel *spi)
{
	/* FIXME: Make this take total packet size into account */
	return read32(&spi->regs->trans_status) &
		(SPI_STATUS_BLOCK_COUNT << SPI_STATUS_BLOCK_COUNT_SHIFT);
}

static void tegra_spi_wait(struct tegra_spi_channel *spi)
{
	uint32_t dma_blk_count = 1 + (read32(&spi->regs->dma_blk) &
				      (SPI_DMA_CTL_BLOCK_SIZE_MASK <<
				       SPI_DMA_CTL_BLOCK_SIZE_SHIFT));

	while ((read32(&spi->regs->trans_status) & SPI_STATUS_RDY) !=
	       SPI_STATUS_RDY)
		;

	/*
	 * If RDY bit is set, we should never encounter the condition that
	 * blocks processed is not equal to the number programmed in dma_blk
	 * register.
	 */
	ASSERT(spi_byte_count(spi) == dma_blk_count);
}

static int fifo_error(struct tegra_spi_channel *spi)
{
	return read32(&spi->regs->fifo_status) & SPI_FIFO_STATUS_ERR ? 1 : 0;
}

static void flush_fifos(struct tegra_spi_channel *spi)
{
	const uint32_t flush_mask = SPI_FIFO_STATUS_TX_FIFO_FLUSH |
		SPI_FIFO_STATUS_RX_FIFO_FLUSH;

	uint32_t fifo_status = read32(&spi->regs->fifo_status);
	fifo_status |= flush_mask;
	write32(&spi->regs->fifo_status, fifo_status);

	while (read32(&spi->regs->fifo_status) & flush_mask)
		;
}

static int tegra_spi_pio_prepare(struct tegra_spi_channel *spi,
			unsigned int bytes, enum spi_direction dir)
{
	u8 *p = spi->out_buf;
	unsigned int todo = MIN(bytes, SPI_MAX_TRANSFER_BYTES_FIFO);
	u32 enable_mask;

	flush_fifos(spi);

	if (dir == SPI_SEND)
		enable_mask = SPI_CMD1_TX_EN;
	else
		enable_mask = SPI_CMD1_RX_EN;

	/*
	 * BLOCK_SIZE in SPI_DMA_BLK register applies to both DMA and
	 * PIO transfers. And, it should be programmed before RX_EN or
	 * TX_EN is set.
	 */
	write32(&spi->regs->dma_blk, todo - 1);

	setbits32(&spi->regs->command1, enable_mask);

	if (dir == SPI_SEND) {
		unsigned int to_fifo = bytes;
		while (to_fifo) {
			write32(&spi->regs->tx_fifo, *p);
			p++;
			to_fifo--;
		}
	}

	return todo;
}

static void tegra_spi_pio_start(struct tegra_spi_channel *spi)
{
	setbits32(&spi->regs->trans_status, SPI_STATUS_RDY);
	/*
	 * Need to stabilize other reg bit before GO bit set.
	 *
	 * From IAS:
	 * For successful operation at various freq combinations, min of 4-5
	 * spi_clk cycle delay might be required before enabling PIO or DMA bit.
	 * This is needed to overcome the MCP between core and pad_macro.
	 * The worst case delay calculation can be done considering slowest
	 * qspi_clk as 1 MHz. based on that 1 us delay should be enough before
	 * enabling pio or dma.
	 */
	udelay(2);
	setbits32(&spi->regs->command1, SPI_CMD1_GO);
	/* Need to wait a few cycles before command1 register is read */
	udelay(1);
	/* Make sure the write to command1 completes. */
	read32(&spi->regs->command1);
}

static inline u32 rx_fifo_count(struct tegra_spi_channel *spi)
{
	return (read32(&spi->regs->fifo_status) >>
		SPI_FIFO_STATUS_RX_FIFO_FULL_COUNT_SHIFT) &
		SPI_FIFO_STATUS_RX_FIFO_FULL_COUNT_MASK;
}

static int tegra_spi_pio_finish(struct tegra_spi_channel *spi)
{
	u8 *p = spi->in_buf;

	clrbits32(&spi->regs->command1, SPI_CMD1_RX_EN | SPI_CMD1_TX_EN);

	ASSERT(rx_fifo_count(spi) == spi_byte_count(spi));

	if (p) {
		while (!(read32(&spi->regs->fifo_status) &
			 SPI_FIFO_STATUS_RX_FIFO_EMPTY)) {
			*p = read8(&spi->regs->rx_fifo);
			p++;
		}
	}

	if (fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR:\n", __func__);
		dump_spi_regs(spi);
		dump_fifo_status(spi);
		return -1;
	}

	return 0;
}

static void setup_dma_params(struct tegra_spi_channel *spi,
				struct apb_dma_channel *dma)
{
	/* APB bus width = 8-bits, address wrap for each word */
	clrbits32(&dma->regs->apb_seq,
			APB_BUS_WIDTH_MASK << APB_BUS_WIDTH_SHIFT);
	/* AHB 1 word burst, bus width = 32 bits (fixed in hardware),
	 * no address wrapping */
	clrsetbits32(&dma->regs->ahb_seq,
			(AHB_BURST_MASK << AHB_BURST_SHIFT),
			4 << AHB_BURST_SHIFT);

	/* Set ONCE mode to transfer one "block" at a time (64KB) and enable
	 * flow control. */
	clrbits32(&dma->regs->csr,
			APB_CSR_REQ_SEL_MASK << APB_CSR_REQ_SEL_SHIFT);
	setbits32(&dma->regs->csr, APB_CSR_ONCE | APB_CSR_FLOW |
			(spi->req_sel << APB_CSR_REQ_SEL_SHIFT));
}

static int tegra_spi_dma_prepare(struct tegra_spi_channel *spi,
		unsigned int bytes, enum spi_direction dir)
{
	unsigned int todo, wcount;

	/*
	 * For DMA we need to think of things in terms of word count.
	 * AHB width is fixed at 32-bits. To avoid overrunning
	 * the in/out buffers we must align down. (Note: lowest 2-bits
	 * in WCOUNT register are ignored, and WCOUNT seems to count
	 * words starting at n-1)
	 *
	 * Example: If "bytes" is 7 and we are transferring 1-byte at a time,
	 * WCOUNT should be 4. The remaining 3 bytes must be transferred
	 * using PIO.
	 */
	todo = MIN(bytes, SPI_MAX_TRANSFER_BYTES_DMA - TEGRA_DMA_ALIGN_BYTES);
	todo = ALIGN_DOWN(todo, TEGRA_DMA_ALIGN_BYTES);
	wcount = ALIGN_DOWN(todo - TEGRA_DMA_ALIGN_BYTES, TEGRA_DMA_ALIGN_BYTES);

	flush_fifos(spi);

	if (dir == SPI_SEND) {
		spi->dma_out = dma_claim();
		if (!spi->dma_out)
			return -1;

		/* ensure bytes to send will be visible to DMA controller */
		dcache_clean_by_mva(spi->out_buf, bytes);

		write32(&spi->dma_out->regs->apb_ptr,
			(uintptr_t)&spi->regs->tx_fifo);
		write32(&spi->dma_out->regs->ahb_ptr, (uintptr_t)spi->out_buf);
		setbits32(&spi->dma_out->regs->csr, APB_CSR_DIR);
		setup_dma_params(spi, spi->dma_out);
		write32(&spi->dma_out->regs->wcount, wcount);
	} else {
		spi->dma_in = dma_claim();
		if (!spi->dma_in)
			return -1;

		/* avoid data collisions */
		dcache_clean_invalidate_by_mva(spi->in_buf, bytes);

		write32(&spi->dma_in->regs->apb_ptr,
			(uintptr_t)&spi->regs->rx_fifo);
		write32(&spi->dma_in->regs->ahb_ptr, (uintptr_t)spi->in_buf);
		clrbits32(&spi->dma_in->regs->csr, APB_CSR_DIR);
		setup_dma_params(spi, spi->dma_in);
		write32(&spi->dma_in->regs->wcount, wcount);
	}

	/* BLOCK_SIZE starts at n-1 */
	write32(&spi->regs->dma_blk, todo - 1);
	return todo;
}

static void tegra_spi_dma_start(struct tegra_spi_channel *spi)
{
	/*
	 * The RDY bit in SPI_TRANS_STATUS needs to be cleared manually
	 * (set bit to clear) between each transaction. Otherwise the next
	 * transaction does not start.
	 */
	setbits32(&spi->regs->trans_status, SPI_STATUS_RDY);

	struct apb_dma * const apb_dma = (struct apb_dma *)TEGRA_APB_DMA_BASE;

	/*
	 * The DMA triggers have units of packets. As each packet is currently
	 * 1 byte the triggers need to be set to 4 packets (0b01) to match
	 * the AHB 32-bit (4 byte) tranfser. Otherwise the FIFO errors can
	 * occur.
	 */
	if (spi->dma_out) {
		/* Enable secure access for the channel. */
		setbits32(&apb_dma->security_reg,
			     SECURITY_EN_BIT(spi->dma_out->num));
		clrsetbits32(&spi->regs->dma_ctl,
			SPI_DMA_CTL_TX_TRIG_MASK << SPI_DMA_CTL_TX_TRIG_SHIFT,
			1 << SPI_DMA_CTL_TX_TRIG_SHIFT);
		setbits32(&spi->regs->command1, SPI_CMD1_TX_EN);
	}
	if (spi->dma_in) {
		/* Enable secure access for the channel. */
		setbits32(&apb_dma->security_reg,
			     SECURITY_EN_BIT(spi->dma_in->num));
		clrsetbits32(&spi->regs->dma_ctl,
			SPI_DMA_CTL_RX_TRIG_MASK << SPI_DMA_CTL_RX_TRIG_SHIFT,
			1 << SPI_DMA_CTL_RX_TRIG_SHIFT);
		setbits32(&spi->regs->command1, SPI_CMD1_RX_EN);
	}

	/*
	 * To avoid underrun conditions, enable APB DMA before SPI DMA for
	 * Tx and enable SPI DMA before APB DMA before Rx.
	 */
	if (spi->dma_out)
		dma_start(spi->dma_out);
	setbits32(&spi->regs->dma_ctl, SPI_DMA_CTL_DMA);
	if (spi->dma_in)
		dma_start(spi->dma_in);
}

static int tegra_spi_dma_finish(struct tegra_spi_channel *spi)
{
	int ret;
	unsigned int todo;

	struct apb_dma * const apb_dma = (struct apb_dma *)TEGRA_APB_DMA_BASE;

	if (spi->dma_in) {
		todo = read32(&spi->dma_in->regs->wcount);

		while ((read32(&spi->dma_in->regs->dma_byte_sta) < todo) ||
				dma_busy(spi->dma_in))
			;
		dma_stop(spi->dma_in);
		clrbits32(&spi->regs->command1, SPI_CMD1_RX_EN);
		/* Disable secure access for the channel. */
		clrbits32(&apb_dma->security_reg,
			     SECURITY_EN_BIT(spi->dma_in->num));
		dma_release(spi->dma_in);
	}

	if (spi->dma_out) {
		todo = read32(&spi->dma_out->regs->wcount);

		while ((read32(&spi->dma_out->regs->dma_byte_sta) < todo) ||
				dma_busy(spi->dma_out))
			;
		clrbits32(&spi->regs->command1, SPI_CMD1_TX_EN);
		dma_stop(spi->dma_out);
		/* Disable secure access for the channel. */
		clrbits32(&apb_dma->security_reg,
			     SECURITY_EN_BIT(spi->dma_out->num));
		dma_release(spi->dma_out);
	}

	if (fifo_error(spi)) {
		printk(BIOS_ERR, "%s: ERROR:\n", __func__);
		dump_dma_regs(spi->dma_out);
		dump_dma_regs(spi->dma_in);
		dump_spi_regs(spi);
		dump_fifo_status(spi);
		ret = -1;
		goto done;
	}

	ret = 0;
done:
	spi->dma_in = NULL;
	spi->dma_out = NULL;
	return ret;
}

/*
 * xfer_setup() prepares a transfer. It does sanity checking, alignment, and
 * sets transfer mode used by this channel (if not set already).
 *
 * A few caveats to watch out for:
 * - The number of bytes which can be transferred may be smaller than the
 *   number of bytes the caller specifies. The number of bytes ready for
 *   a transfer will be returned (unless an error occurs).
 *
 * - Only one mode can be used for both RX and TX. The transfer mode of the
 *   SPI channel (spi->xfer_mode) is checked each time this function is called.
 *   If conflicting modes are detected, spi->xfer_mode will be set to
 *   XFER_MODE_NONE and an error will be returned.
 *
 * Returns bytes ready for transfer if successful, <0 to indicate error.
 */
static int xfer_setup(struct tegra_spi_channel *spi, void *buf,
		unsigned int bytes, enum spi_direction dir)
{
	unsigned int line_size = dcache_line_bytes();
	unsigned int align;
	int ret = -1;

	if (!bytes)
		return 0;

	if (dir == SPI_SEND)
		spi->out_buf = buf;
	else if (dir == SPI_RECEIVE)
		spi->in_buf = buf;

	/*
	 * Alignment consideratons:
	 * When we enable caching we'll need to clean/invalidate portions of
	 * memory. So we need to be careful about memory alignment. Also, DMA
	 * likes to operate on 4-bytes at a time on the AHB side. So for
	 * example, if we only want to receive 1 byte, 4 bytes will be
	 * written in memory even if those extra 3 bytes are beyond the length
	 * we want.
	 *
	 * For now we'll use PIO to send/receive unaligned bytes. We may
	 * consider setting aside some space for a kind of bounce buffer to
	 * stay in DMA mode once we have a chance to benchmark the two
	 * approaches.
	 */

	if (bytes < line_size) {
		if (spi->xfer_mode == XFER_MODE_DMA) {
			spi->xfer_mode = XFER_MODE_NONE;
			ret = -1;
		} else {
			spi->xfer_mode = XFER_MODE_PIO;
			ret = tegra_spi_pio_prepare(spi, bytes, dir);
		}
		goto done;
	}

	/* transfer bytes before the aligned boundary */
	align = line_size - ((uintptr_t)buf % line_size);
	if ((align != 0) && (align != line_size)) {
		if (spi->xfer_mode == XFER_MODE_DMA) {
			spi->xfer_mode = XFER_MODE_NONE;
			ret = -1;
		} else {
			spi->xfer_mode = XFER_MODE_PIO;
			ret = tegra_spi_pio_prepare(spi, align, dir);
		}
		goto done;
	}

	/* do aligned DMA transfer */
	align = (((uintptr_t)buf + bytes) % line_size);
	if (bytes - align > 0) {
		unsigned int dma_bytes = bytes - align;

		if (spi->xfer_mode == XFER_MODE_PIO) {
			spi->xfer_mode = XFER_MODE_NONE;
			ret = -1;
		} else {
			spi->xfer_mode = XFER_MODE_DMA;
			ret = tegra_spi_dma_prepare(spi, dma_bytes, dir);
		}

		goto done;
	}

	/* transfer any remaining unaligned bytes */
	if (align) {
		if (spi->xfer_mode == XFER_MODE_DMA) {
			spi->xfer_mode = XFER_MODE_NONE;
			ret = -1;
		} else {
			spi->xfer_mode = XFER_MODE_PIO;
			ret = tegra_spi_pio_prepare(spi, align, dir);
		}
		goto done;
	}

done:
	return ret;
}

static void xfer_start(struct tegra_spi_channel *spi)
{
	if (spi->xfer_mode == XFER_MODE_DMA)
		tegra_spi_dma_start(spi);
	else
		tegra_spi_pio_start(spi);
}

static void xfer_wait(struct tegra_spi_channel *spi)
{
	tegra_spi_wait(spi);
}

static int xfer_finish(struct tegra_spi_channel *spi)
{
	int ret;

	if (spi->xfer_mode == XFER_MODE_DMA)
		ret = tegra_spi_dma_finish(spi);
	else
		ret = tegra_spi_pio_finish(spi);

	spi->xfer_mode = XFER_MODE_NONE;
	return ret;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			  size_t out_bytes, void *din, size_t in_bytes)
{
	struct tegra_spi_channel *spi = to_tegra_spi(slave->bus);
	u8 *out_buf = (u8 *)dout;
	u8 *in_buf = (u8 *)din;
	size_t todo;
	int ret = 0;

	/* tegra bus numbers start at 1 */
	ASSERT(slave->bus >= 1 && slave->bus <= ARRAY_SIZE(tegra_spi_channels));

	while (out_bytes || in_bytes) {
		int x = 0;

		if (out_bytes == 0)
			todo = in_bytes;
		else if (in_bytes == 0)
			todo = out_bytes;
		else
			todo = MIN(out_bytes, in_bytes);

		if (out_bytes) {
			x = xfer_setup(spi, out_buf, todo, SPI_SEND);
			if (x < 0) {
				if (spi->xfer_mode == XFER_MODE_NONE) {
					spi->xfer_mode = XFER_MODE_PIO;
					continue;
				} else {
					ret = -1;
					break;
				}
			}
		}
		if (in_bytes) {
			x = xfer_setup(spi, in_buf, todo, SPI_RECEIVE);
			if (x < 0) {
				if (spi->xfer_mode == XFER_MODE_NONE) {
					spi->xfer_mode = XFER_MODE_PIO;
					continue;
				} else {
					ret = -1;
					break;
				}
			}
		}

		/*
		 * Note: Some devices (such as Chrome EC) are sensitive to
		 * delays, so be careful when adding debug prints not to
		 * cause timeouts between transfers.
		 */
		xfer_start(spi);
		xfer_wait(spi);
		if (xfer_finish(spi)) {
			ret = -1;
			break;
		}

		/* Post-processing. */
		if (out_bytes) {
			out_bytes -= x;
			out_buf += x;
		}
		if (in_bytes) {
			in_bytes -= x;
			in_buf += x;
		}
	}

	if (ret < 0) {
		printk(BIOS_ERR, "%s: Error detected\n", __func__);
		printk(BIOS_ERR, "Transaction size: %zu, bytes remaining: "
				"%zu out / %zu in\n", todo, out_bytes, in_bytes);
		clear_fifo_status(spi);
	}
	return ret;
}

static const struct spi_ctrlr spi_ctrlr = {
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = SPI_CTRLR_DEFAULT_MAX_XFER_SIZE,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 1,
		.bus_end = ARRAY_SIZE(tegra_spi_channels)
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
