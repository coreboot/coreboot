/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <lib.h>
#include <soc/clock.h>
#include <soc/gpio.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>
#include <soc/qupv3_spi_common.h>
#include <types.h>

/* SE_SPI_LOOPBACK register fields */
#define LOOPBACK_ENABLE	0x1

/* SE_SPI_WORD_LEN register fields */
#define WORD_LEN_MSK	QC_GENMASK(9, 0)
#define MIN_WORD_LEN	4

/* SPI_TX/SPI_RX_TRANS_LEN fields */
#define TRANS_LEN_MSK	QC_GENMASK(23, 0)

/* M_CMD OP codes for SPI */
#define SPI_TX_ONLY	1
#define SPI_RX_ONLY	2
#define SPI_FULL_DUPLEX	3
#define SPI_TX_RX	7
#define SPI_CS_ASSERT	8
#define SPI_CS_DEASSERT	9
#define SPI_SCK_ONLY	10

/* M_CMD params for SPI */
/* If fragmentation bit is set then CS will not toggle after each transfer */
#define M_CMD_FRAGMENTATION	BIT(2)

#define BITS_PER_BYTE	8
#define BITS_PER_WORD	8
#define TX_WATERMARK	1

#define IRQ_TRIGGER (M_RX_FIFO_WATERMARK_EN | M_RX_FIFO_LAST_EN | \
	M_TX_FIFO_WATERMARK_EN | M_CMD_DONE_EN | \
	M_CMD_CANCEL_EN | M_CMD_ABORT_EN)

static void setup_fifo_params(const struct spi_slave *slave)
{
	unsigned int se_bus = slave->bus;
	struct qup_regs *regs = qup[se_bus].regs;
	u32 word_len = 0;

	/* Disable loopback mode */
	write32(&regs->proto_loopback_cfg, 0);

	write32(&regs->spi_demux_sel, slave->cs);
	word_len = ((BITS_PER_WORD - MIN_WORD_LEN) & WORD_LEN_MSK);
	write32(&regs->spi_word_len, word_len);

	/* FIFO PACKING CONFIGURATION */
	write32(&regs->geni_tx_packing_cfg0, PACK_VECTOR0
						| (PACK_VECTOR1 << 10));
	write32(&regs->geni_tx_packing_cfg1, PACK_VECTOR2
						| (PACK_VECTOR3 << 10));
	write32(&regs->geni_rx_packing_cfg0, PACK_VECTOR0
						| (PACK_VECTOR1 << 10));
	write32(&regs->geni_rx_packing_cfg1, PACK_VECTOR2
						| (PACK_VECTOR3 << 10));
	write32(&regs->geni_byte_granularity, (log2(BITS_PER_WORD) - 3));
}

static void qup_setup_m_cmd(unsigned int se_bus, u32 cmd, u32 params)
{
	struct qup_regs *regs = qup[se_bus].regs;
	u32 m_cmd = (cmd << M_OPCODE_SHFT);

	m_cmd |= (params & M_PARAMS_MSK);
	write32(&regs->geni_m_cmd0, m_cmd);
}

int qup_spi_xfer(const struct spi_slave *slave, const void *dout,
	size_t bytes_out, void *din, size_t bytes_in)
{
	u32 m_cmd = 0;
	u32 m_param = M_CMD_FRAGMENTATION;
	int size;
	unsigned int se_bus = slave->bus;
	struct qup_regs *regs = qup[se_bus].regs;
	struct stopwatch timeout;

	if ((bytes_in == 0) && (bytes_out == 0))
		return 0;

	setup_fifo_params(slave);

	if (!bytes_out) {
		size = bytes_in;
		m_cmd = SPI_RX_ONLY;
		dout = NULL;
	} else if (!bytes_in) {
		size = bytes_out;
		m_cmd = SPI_TX_ONLY;
		din = NULL;
	} else {
		size = MIN(bytes_in, bytes_out);
		m_cmd = SPI_FULL_DUPLEX;
	}

	/* Check for maximum permissible transfer length */
	assert(!(size & ~TRANS_LEN_MSK));

	if (bytes_out) {
		write32(&regs->spi_tx_trans_len, size);
		write32(&regs->geni_tx_watermark_reg, TX_WATERMARK);
	}
	if (bytes_in)
		write32(&regs->spi_rx_trans_len, size);

	qup_setup_m_cmd(se_bus, m_cmd, m_param);

	stopwatch_init_msecs_expire(&timeout, 1000);
	if (qup_handle_transfer(se_bus, dout, din, size, &timeout))
		return -1;

	qup_spi_xfer(slave, dout + size, MAX((int)bytes_out - size, 0),
			din + size, MAX((int)bytes_in - size, 0));

	return 0;
}

static int spi_qup_set_cs(const struct spi_slave *slave, bool enable)
{
	u32 m_cmd = 0;
	u32 m_irq = 0;
	unsigned int se_bus = slave->bus;
	struct stopwatch sw;

	m_cmd = (enable) ? SPI_CS_ASSERT : SPI_CS_DEASSERT;
	qup_setup_m_cmd(se_bus, m_cmd, 0);

	stopwatch_init_usecs_expire(&sw, 100);
	do {
		m_irq = qup_wait_for_m_irq(se_bus);
		if (m_irq & M_CMD_DONE_EN) {
			write32(&qup[se_bus].regs->geni_m_irq_clear, m_irq);
			break;
		}
		write32(&qup[se_bus].regs->geni_m_irq_clear, m_irq);
	} while (!stopwatch_expired(&sw));

	if (!(m_irq & M_CMD_DONE_EN)) {
		printk(BIOS_INFO, "%s:Failed to %s chip\n", __func__,
					(enable) ? "Assert" : "Deassert");
		qup_m_cancel_and_abort(se_bus);
		return -1;
	}
	return 0;
}

void qup_spi_init(unsigned int bus, unsigned int speed_hz)
{
	u32 m_clk_cfg = 0, div = DEFAULT_SE_CLK / speed_hz;
	struct qup_regs *regs = qup[bus].regs;

	/* Make sure div can hit target frequency within +/- 1KHz range */
	assert(((DEFAULT_SE_CLK - speed_hz * div) <= div * KHz) && (div > 0));
	qupv3_se_fw_load_and_init(bus, SE_PROTOCOL_SPI, MIXED);
	clock_enable_qup(bus);
	m_clk_cfg |= ((div << CLK_DIV_SHFT) | SER_CLK_EN);
	write32(&regs->geni_ser_m_clk_cfg, m_clk_cfg);
	/* Mode:0, cpha=0, cpol=0 */
	write32(&regs->spi_cpha, 0);
	write32(&regs->spi_cpol, 0);

	/* Serial engine IO initialization */
	write32(&regs->geni_cgc_ctrl, DEFAULT_CGC_EN);
	write32(&regs->dma_general_cfg,
		(AHB_SEC_SLV_CLK_CGC_ON | DMA_AHB_SLV_CFG_ON
		| DMA_TX_CLK_CGC_ON | DMA_RX_CLK_CGC_ON));
	write32(&regs->geni_output_ctrl,
			DEFAULT_IO_OUTPUT_CTRL_MSK);
	write32(&regs->geni_force_default_reg, FORCE_DEFAULT);

	/* Serial engine IO set mode */
	write32(&regs->se_irq_en, (GENI_M_IRQ_EN |
			GENI_S_IRQ_EN | DMA_TX_IRQ_EN | DMA_RX_IRQ_EN));
	write32(&regs->se_gsi_event_en, 0);

	/* Set RX and RFR watermark */
	write32(&regs->geni_rx_watermark_reg, 0);
	write32(&regs->geni_rx_rfr_watermark_reg, FIFO_DEPTH - 2);

	/* GPIO Configuration */
	gpio_configure(qup[bus].pin[0], qup[bus].func[0], GPIO_NO_PULL,
				GPIO_6MA, GPIO_INPUT);   /* MISO */
	gpio_configure(qup[bus].pin[1], qup[bus].func[1], GPIO_NO_PULL,
				GPIO_6MA, GPIO_OUTPUT);  /* MOSI */
	gpio_configure(qup[bus].pin[2], qup[bus].func[2], GPIO_NO_PULL,
				GPIO_6MA, GPIO_OUTPUT);  /* CLK */
	gpio_configure(qup[bus].pin[3], qup[bus].func[3], GPIO_NO_PULL,
				GPIO_6MA, GPIO_OUTPUT);  /* CS */

	/* Select and setup FIFO mode */
	write32(&regs->geni_m_irq_clear, 0xFFFFFFFF);
	write32(&regs->geni_s_irq_clear, 0xFFFFFFFF);
	write32(&regs->dma_tx_irq_clr, 0xFFFFFFFF);
	write32(&regs->dma_rx_irq_clr, 0xFFFFFFFF);
	write32(&regs->geni_m_irq_enable, (M_COMMON_GENI_M_IRQ_EN |
			M_CMD_DONE_EN |	M_TX_FIFO_WATERMARK_EN |
			M_RX_FIFO_WATERMARK_EN | M_RX_FIFO_LAST_EN));
	write32(&regs->geni_s_irq_enable, (S_COMMON_GENI_S_IRQ_EN
						| S_CMD_DONE_EN));
	clrbits32(&regs->geni_dma_mode_en, GENI_DMA_MODE_EN);
}

int qup_spi_claim_bus(const struct spi_slave *slave)
{
	return spi_qup_set_cs(slave, 1);
}

void qup_spi_release_bus(const struct spi_slave *slave)
{
	spi_qup_set_cs(slave, 0);
}
