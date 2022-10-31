/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <gpio.h>
#include <lib.h>
#include <soc/clock.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>
#include <soc/qupv3_i2c_common.h>
#include <stdint.h>

static void i2c_clk_configure(unsigned int bus, enum i2c_speed speed)
{
	int clk_div = 0, t_high = 0, t_low = 0, t_cycle = 0;
	struct qup_regs *regs = qup[bus].regs;

	switch (speed) {
	case I2C_SPEED_STANDARD:
		clk_div = 7;
		t_high = 10;
		t_low = 11;
		t_cycle = 26;
		break;
	case I2C_SPEED_FAST:
		clk_div = 2;
		t_high = 5;
		t_low = 12;
		t_cycle = 24;
		break;
	case I2C_SPEED_FAST_PLUS:
		clk_div = 1;
		t_high = 3;
		t_low = 9;
		t_cycle = 18;
		break;
	default:
		die("Unsupported I2C speed");
	}

	write32(&regs->geni_ser_m_clk_cfg, (clk_div << 4) | 1);
	/* Serial clock frequency is 19.2 MHz */
	write32(&regs->i2c_scl_counters, ((t_high << 20) | (t_low << 10)
								| t_cycle));
}

void i2c_init(unsigned int bus, enum i2c_speed speed)
{
	uint32_t proto;
	struct qup_regs *regs = qup[bus].regs;

	qupv3_se_fw_load_and_init(bus, SE_PROTOCOL_I2C, MIXED);
	clock_enable_qup(bus);
	i2c_clk_configure(bus, speed);

	proto = ((read32(&regs->geni_fw_revision_ro) &
					GENI_FW_REVISION_RO_PROTOCOL_MASK) >>
					GENI_FW_REVISION_RO_PROTOCOL_SHIFT);

	assert(proto == 3);

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

	/* GPIO Configuration */
	gpio_configure(qup[bus].pin[0], qup[bus].func[0], GPIO_PULL_UP,
				GPIO_2MA, GPIO_OUTPUT);
	gpio_configure(qup[bus].pin[1], qup[bus].func[1], GPIO_PULL_UP,
				GPIO_2MA, GPIO_OUTPUT);

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

static int i2c_do_xfer(unsigned int bus, struct i2c_msg segment,
				unsigned int prams)
{
	unsigned int cmd = (segment.flags & I2C_M_RD) ? 2 : 1;
	unsigned int master_cmd_reg_val = (cmd << M_OPCODE_SHFT);
	struct qup_regs *regs = qup[bus].regs;
	void *dout = NULL, *din = NULL;
	struct stopwatch timeout;

	if (!(segment.flags & I2C_M_RD)) {
		write32(&regs->i2c_tx_trans_len, segment.len);
		write32(&regs->geni_tx_watermark_reg, TX_WATERMARK);
		dout = segment.buf;
	} else {
		write32(&regs->i2c_rx_trans_len, segment.len);
		din = segment.buf;
	}

	master_cmd_reg_val |= (prams & M_PARAMS_MSK);
	write32(&regs->geni_m_cmd0, master_cmd_reg_val);

	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);
	return qup_handle_transfer(bus, dout, din, segment.len, &timeout);
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	struct i2c_msg *seg = segments;
	int ret = 0;

	while (!ret && seg_count--) {
		/* Stretch means end with repeated start, not stop */
		u32 stretch = (seg_count ? 1 : 0);
		u32 m_param = 0;

		m_param |= (stretch << 2);
		m_param |= ((seg->slave & 0x7F) << 9);
		ret = i2c_do_xfer(bus, *seg, m_param);
		seg++;
	}
	return ret;
}
