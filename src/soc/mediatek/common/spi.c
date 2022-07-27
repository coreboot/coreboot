/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <endian.h>
#include <gpio.h>
#include <soc/pll.h>
#include <soc/spi.h>
#include <timer.h>
#include <types.h>

#define MTK_SPI_DEBUG 0

enum {
	MTK_FIFO_DEPTH = 32,
	MTK_TXRX_TIMEOUT_US = 1000 * 1000,
	MTK_ARBITRARY_VALUE = 0xdeaddead
};

enum {
	MTK_SPI_IDLE = 0,
	MTK_SPI_PAUSE_IDLE = 1
};

enum {
	MTK_SPI_BUSY_STATUS = 1,
	MTK_SPI_PAUSE_FINISH_INT_STATUS = 3
};

static inline struct mtk_spi_bus *to_mtk_spi(const struct spi_slave *slave)
{
	assert(slave->bus < SPI_BUS_NUMBER);
	return &spi_bus[slave->bus];
}

void mtk_spi_set_timing(struct mtk_spi_regs *regs, u32 sck_ticks,
			u32 cs_ticks, unsigned int tick_dly)
{
	SET32_BITFIELDS(&regs->spi_cfg0_reg, SPI_CFG_CS_HOLD, cs_ticks - 1,
			SPI_CFG_CS_SETUP, cs_ticks - 1);

	SET32_BITFIELDS(&GET_SCK_REG(regs), SPI_CFG_SCK_LOW, sck_ticks - 1,
			SPI_CFG_SCK_HIGH, sck_ticks - 1);

	SET32_BITFIELDS(&regs->spi_cfg1_reg, SPI_CFG1_CS_IDLE, cs_ticks - 1);

	SET32_BITFIELDS(&GET_TICK_DLY_REG(regs), SPI_TICK_DLY, tick_dly);
}

static void spi_sw_reset(struct mtk_spi_regs *regs)
{
	setbits32(&regs->spi_cmd_reg, SPI_CMD_RST_EN);
	clrbits32(&regs->spi_cmd_reg, SPI_CMD_RST_EN);
}

void mtk_spi_init(unsigned int bus, enum spi_pad_mask pad_select,
		  unsigned int speed_hz, unsigned int tick_dly)
{
	u32 div, sck_ticks, cs_ticks;

	assert(bus < SPI_BUS_NUMBER);

	struct mtk_spi_bus *slave = &spi_bus[bus];
	struct mtk_spi_regs *regs = slave->regs;

	if (speed_hz < SPI_HZ / 2)
		div = DIV_ROUND_UP(SPI_HZ, speed_hz);
	else
		div = 1;

	sck_ticks = DIV_ROUND_UP(div, 2);
	cs_ticks = sck_ticks * 2;

	printk(BIOS_DEBUG, "SPI%u(PAD%u) initialized at %u Hz\n",
	       bus, pad_select, SPI_HZ / (sck_ticks * 2));

	mtk_spi_set_timing(regs, sck_ticks, cs_ticks, tick_dly);

	clrsetbits32(&regs->spi_cmd_reg,
		     (SPI_CMD_CPHA_EN | SPI_CMD_CPOL_EN |
		      SPI_CMD_TX_ENDIAN_EN | SPI_CMD_RX_ENDIAN_EN |
		      SPI_CMD_TX_DMA_EN | SPI_CMD_RX_DMA_EN |
		      SPI_CMD_PAUSE_EN | SPI_CMD_DEASSERT_EN),
		     (SPI_CMD_TXMSBF_EN | SPI_CMD_RXMSBF_EN |
		      SPI_CMD_FINISH_IE_EN | SPI_CMD_PAUSE_IE_EN));

	mtk_spi_set_gpio_pinmux(bus, pad_select);

	clrsetbits32(&regs->spi_pad_macro_sel_reg, SPI_PAD_SEL_MASK,
			pad_select);

	gpio_output(slave->cs_gpio, 1);
}

static void mtk_spi_dump_data(const char *name, const uint8_t *data, int size)
{
	if (MTK_SPI_DEBUG) {
		int i;

		printk(BIOS_DEBUG, "%s: 0x ", name);
		for (i = 0; i < size; i++)
			printk(BIOS_INFO, "%#x ", data[i]);
		printk(BIOS_DEBUG, "\n");
	}
}

static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;

	setbits32(&regs->spi_cmd_reg, 1 << SPI_CMD_PAUSE_EN_SHIFT);
	mtk_slave->state = MTK_SPI_IDLE;

	gpio_output(mtk_slave->cs_gpio, 0);

	return 0;
}

static int do_transfer(const struct spi_slave *slave, void *in, const void *out,
		       size_t *bytes_in, size_t *bytes_out)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;
	uint32_t reg_val = 0;
	uint32_t i;
	struct stopwatch sw;
	size_t size;

	if (*bytes_out == 0)
		size = *bytes_in;
	else if (*bytes_in == 0)
		size = *bytes_out;
	else
		size = MIN(*bytes_in, *bytes_out);

	SET32_BITFIELDS(&regs->spi_cfg1_reg, SPI_CFG1_PACKET_LENGTH, size - 1,
			SPI_CFG1_PACKET_LOOP, 0);

	if (*bytes_out) {
		const uint8_t *outb = (const uint8_t *)out;
		for (i = 0; i < size; i++) {
			reg_val |= outb[i] << ((i % 4) * 8);
			if (i % 4 == 3) {
				write32(&regs->spi_tx_data_reg, reg_val);
				reg_val = 0;
			}
		}

		if (i % 4 != 0)
			write32(&regs->spi_tx_data_reg, reg_val);

		mtk_spi_dump_data("the outb data is",
				  (const uint8_t *)outb, size);
	} else {
		/* The SPI controller will transmit in full-duplex for RX,
		 * therefore we need arbitrary data on MOSI which the slave
		 * must ignore.
		 */
		uint32_t word_count = DIV_ROUND_UP(size, sizeof(u32));
		for (i = 0; i < word_count; i++)
			write32(&regs->spi_tx_data_reg, MTK_ARBITRARY_VALUE);
	}

	if (mtk_slave->state == MTK_SPI_IDLE) {
		setbits32(&regs->spi_cmd_reg, SPI_CMD_ACT_EN);
		mtk_slave->state = MTK_SPI_PAUSE_IDLE;
	} else if (mtk_slave->state == MTK_SPI_PAUSE_IDLE) {
		setbits32(&regs->spi_cmd_reg, SPI_CMD_RESUME_EN);
	}

	stopwatch_init_usecs_expire(&sw, MTK_TXRX_TIMEOUT_US);
	while ((read32(&regs->spi_status1_reg) & MTK_SPI_BUSY_STATUS) == 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "Timeout waiting for status1 status.\n");
			goto error;
		}
	}
	stopwatch_init_usecs_expire(&sw, MTK_TXRX_TIMEOUT_US);
	while ((read32(&regs->spi_status0_reg) &
	       MTK_SPI_PAUSE_FINISH_INT_STATUS) == 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR,
			       "Timeout waiting for status0 status.\n");
			goto error;
		}
	}

	if (*bytes_in) {
		uint8_t *inb = (uint8_t *)in;
		for (i = 0; i < size; i++) {
			if (i % 4 == 0)
				reg_val = read32(&regs->spi_rx_data_reg);
			inb[i] = (reg_val >> ((i % 4) * 8)) & 0xff;
		}
		mtk_spi_dump_data("the inb data is", inb, size);

		*bytes_in -= size;
	}

	if (*bytes_out)
		*bytes_out -= size;

	return 0;
error:
	spi_sw_reset(regs);
	mtk_slave->state = MTK_SPI_IDLE;
	return -1;
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			  size_t bytes_out, void *din, size_t bytes_in)
{
	while (bytes_out || bytes_in) {
		size_t in_now = MIN(bytes_in, MTK_FIFO_DEPTH);
		size_t out_now = MIN(bytes_out, MTK_FIFO_DEPTH);
		size_t in_rem = in_now;
		size_t out_rem = out_now;

		int ret = do_transfer(slave, din, dout, &in_rem, &out_rem);
		if (ret != 0)
			return ret;

		if (bytes_out) {
			size_t sent = out_now - out_rem;
			bytes_out -= sent;
			dout += sent;
		}

		if (bytes_in) {
			size_t received = in_now - in_rem;
			bytes_in -= received;
			din += received;
		}
	}

	return 0;
}

static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct mtk_spi_bus *mtk_slave = to_mtk_spi(slave);
	struct mtk_spi_regs *regs = mtk_slave->regs;

	clrbits32(&regs->spi_cmd_reg, SPI_CMD_PAUSE_EN);
	spi_sw_reset(regs);
	mtk_slave->state = MTK_SPI_IDLE;

	gpio_output(mtk_slave->cs_gpio, 1);
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	struct mtk_spi_bus *eslave = to_mtk_spi(slave);
	assert(read32(&eslave->regs->spi_cfg0_reg) != 0);
	spi_sw_reset(eslave->regs);
	return 0;
}

const struct spi_ctrlr spi_ctrlr = {
	.setup = spi_ctrlr_setup,
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = 65535,
};
