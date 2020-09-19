/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/qcom_qup_se.h>

struct qup qup[12] = {
	[0] = { .regs = (void *)QUP_SERIAL0_BASE,
		.pin = { GPIO(34), GPIO(35), GPIO(36), GPIO(37) },
		.func = { GPIO34_FUNC_QUP0_L0, GPIO35_FUNC_QUP0_L1,
			GPIO36_FUNC_QUP0_L2, GPIO37_FUNC_QUP0_L3 }
		},
	[1] = { .regs = (void *)QUP_SERIAL1_BASE,
		.pin = { GPIO(0), GPIO(1), GPIO(2), GPIO(3),
			GPIO(12), GPIO(94) },
		.func = { GPIO0_FUNC_QUP0_L0, GPIO1_FUNC_QUP0_L1,
			GPIO2_FUNC_QUP0_L2, GPIO3_FUNC_QUP0_L3,
			GPIO12_FUNC_QUP0_L4, GPIO94_FUNC_QUP0_L5 }
		},
	[2] = { .regs = (void *)QUP_SERIAL2_BASE,
		.pin = { GPIO(15), GPIO(16) },
		.func = { GPIO15_FUNC_QUP0_L0, GPIO16_FUNC_QUP0_L1 }
		},
	[3] = { .regs = (void *)QUP_SERIAL3_BASE,
		.pin = { GPIO(38), GPIO(39), GPIO(40), GPIO(41) },
		.func = { GPIO38_FUNC_QUP0_L0, GPIO39_FUNC_QUP0_L1,
			GPIO40_FUNC_QUP0_L2, GPIO41_FUNC_QUP0_L3 }
		},
	[4] = { .regs = (void *)QUP_SERIAL4_BASE,
		.pin = { GPIO(115), GPIO(116) },
		.func = { GPIO115_FUNC_QUP0_L0, GPIO116_FUNC_QUP0_L1 }
		},
	[5] = { .regs = (void *)QUP_SERIAL5_BASE,
		.pin = { GPIO(25), GPIO(26), GPIO(27), GPIO(28) },
		.func = { GPIO25_FUNC_QUP0_L0, GPIO26_FUNC_QUP0_L1,
			GPIO27_FUNC_QUP0_L2, GPIO28_FUNC_QUP0_L3 }
		},
	[6] = { .regs = (void *)QUP_SERIAL6_BASE,
		.pin = { GPIO(59), GPIO(60), GPIO(61), GPIO(62),
			GPIO(68), GPIO(72) },
		.func = { GPIO59_FUNC_QUP1_L0, GPIO60_FUNC_QUP1_L1,
			GPIO61_FUNC_QUP1_L2, GPIO62_FUNC_QUP1_L3,
			GPIO68_FUNC_QUP1_L4, GPIO72_FUNC_QUP1_L5 }
		},
	[7] = { .regs = (void *)QUP_SERIAL7_BASE,
		.pin = { GPIO(6), GPIO(7) },
		.func = { GPIO6_FUNC_QUP1_L0, GPIO7_FUNC_QUP1_L1 }
		},
	[8] = { .regs = (void *)QUP_SERIAL8_BASE,
		.pin = { GPIO(42), GPIO(43), GPIO(44), GPIO(45) },
		.func = { GPIO42_FUNC_QUP1_L0, GPIO43_FUNC_QUP1_L1,
			GPIO44_FUNC_QUP1_L2, GPIO45_FUNC_QUP1_L3 }
		},
	[9] = { .regs = (void *)QUP_SERIAL9_BASE,
		.pin = { GPIO(46), GPIO(47) },
		.func = { GPIO46_FUNC_QUP1_L0, GPIO47_FUNC_QUP1_L1 }
		},
	[10] = { .regs = (void *)QUP_SERIAL10_BASE,
		.pin = { GPIO(86), GPIO(87), GPIO(88), GPIO(89),
			GPIO(90), GPIO(91) },
		.func = { GPIO86_FUNC_QUP1_L0, GPIO87_FUNC_QUP1_L1,
			GPIO88_FUNC_QUP1_L2, GPIO89_FUNC_QUP1_L3,
			GPIO90_FUNC_QUP1_L4, GPIO91_FUNC_QUP1_L5 }
		},
	[11] = { .regs = (void *)QUP_SERIAL11_BASE,
		.pin = { GPIO(53), GPIO(54), GPIO(55), GPIO(56) },
		.func = { GPIO53_FUNC_QUP1_L0, GPIO54_FUNC_QUP1_L1,
			GPIO55_FUNC_QUP1_L2, GPIO56_FUNC_QUP1_L3 }
		},
};

u32 qup_wait_for_m_irq(unsigned int bus)
{
	struct stopwatch sw;
	unsigned int m_irq = 0;
	struct qup_regs *regs = qup[bus].regs;

	stopwatch_init_usecs_expire(&sw, 25);
	while (!stopwatch_expired(&sw)) {
		m_irq = read32(&regs->geni_m_irq_status);
		if (m_irq)
			break;
	}
	return m_irq;
}

u32 qup_wait_for_s_irq(unsigned int bus)
{
	struct stopwatch sw;
	unsigned int s_irq = 0;
	struct qup_regs *regs = qup[bus].regs;

	stopwatch_init_usecs_expire(&sw, 25);
	while (!stopwatch_expired(&sw)) {
		s_irq = read32(&regs->geni_s_irq_status);
		if (s_irq)
			break;
	}
	return s_irq;
}

static int handle_tx(unsigned int bus, const u8 *dout,
	unsigned int tx_rem_bytes)
{
	int max_bytes = 0;
	struct qup_regs *regs = qup[bus].regs;

	max_bytes = (FIFO_DEPTH - TX_WATERMARK) * BYTES_PER_FIFO_WORD;
	max_bytes = MIN(tx_rem_bytes, max_bytes);

	buffer_to_fifo32((void *)dout, max_bytes, &regs->geni_tx_fifon,
					0, BYTES_PER_FIFO_WORD);

	if (tx_rem_bytes == max_bytes)
		write32(&regs->geni_tx_watermark_reg, 0);
	return max_bytes;
}

static int handle_rx(unsigned int bus, u8 *din, unsigned int rx_rem_bytes)
{
	struct qup_regs *regs = qup[bus].regs;
	u32 rx_fifo_status = read32(&regs->geni_rx_fifo_status);
	int rx_bytes = 0;

	rx_bytes = (rx_fifo_status & RX_FIFO_WC_MSK) * BYTES_PER_FIFO_WORD;
	rx_bytes = MIN(rx_rem_bytes, rx_bytes);

	buffer_from_fifo32(din, rx_bytes, &regs->geni_rx_fifon,
					0, BYTES_PER_FIFO_WORD);
	return rx_bytes;
}

void qup_m_cancel_and_abort(unsigned int bus)
{
	struct qup_regs *regs = qup[bus].regs;
	struct stopwatch sw;
	unsigned int m_irq;

	write32(&regs->geni_tx_watermark_reg, 0);
	write32(&regs->geni_m_cmd_ctrl_reg, M_GENI_CMD_CANCEL);

	stopwatch_init_msecs_expire(&sw, 100);
	do {
		m_irq = qup_wait_for_m_irq(bus);
		if (m_irq & M_CMD_CANCEL_EN) {
			write32(&regs->geni_m_irq_clear, m_irq);
			break;
		}
		write32(&regs->geni_m_irq_clear, m_irq);
	} while (!stopwatch_expired(&sw));

	if (!(m_irq & M_CMD_CANCEL_EN)) {
		printk(BIOS_INFO, "%s:Cancel failed, Abort the operation\n",
								__func__);

		write32(&regs->geni_m_cmd_ctrl_reg, M_GENI_CMD_ABORT);
		stopwatch_init_msecs_expire(&sw, 100);
		do {
			m_irq = qup_wait_for_m_irq(bus);
			if (m_irq & M_CMD_ABORT_EN) {
				write32(&regs->geni_m_irq_clear, m_irq);
				break;
			}
			write32(&regs->geni_m_irq_clear, m_irq);
		} while (!stopwatch_expired(&sw));

		if (!(m_irq & M_CMD_ABORT_EN))
			printk(BIOS_INFO, "%s:Abort failed\n", __func__);
	}
}

void qup_s_cancel_and_abort(unsigned int bus)
{
	struct qup_regs *regs = qup[bus].regs;
	struct stopwatch sw;
	unsigned int s_irq;
	u32 rx_fifo_status;
	u8 buf[64]; /* FIFO size */

	write32(&regs->geni_tx_watermark_reg, 0);
	write32(&regs->geni_s_cmd_ctrl_reg, S_GENI_CMD_CANCEL);

	stopwatch_init_msecs_expire(&sw, 100);
	do {
		s_irq = qup_wait_for_s_irq(bus);
		rx_fifo_status  = read32(&regs->geni_rx_fifo_status);
		if (rx_fifo_status & RX_LAST)
			handle_rx(bus, buf, 64); /* Read whatever data available in FIFO */
		if (s_irq & S_CMD_CANCEL_EN) {
			write32(&regs->geni_s_irq_clear, s_irq);
			break;
		}
		write32(&regs->geni_s_irq_clear, s_irq);
	} while (!stopwatch_expired(&sw));

	if (!(s_irq & S_CMD_CANCEL_EN)) {
		printk(BIOS_INFO, "%s:Cancel failed, Abort the operation\n",
								__func__);

		write32(&regs->geni_s_cmd_ctrl_reg, S_GENI_CMD_ABORT);
		stopwatch_init_msecs_expire(&sw, 100);
		do {
			s_irq = qup_wait_for_s_irq(bus);
			if (s_irq & S_CMD_ABORT_EN) {
				write32(&regs->geni_s_irq_clear, s_irq);
				break;
			}
			write32(&regs->geni_s_irq_clear, s_irq);
		} while (!stopwatch_expired(&sw));

		if (!(s_irq & S_CMD_ABORT_EN))
			printk(BIOS_INFO, "%s:Abort failed\n", __func__);
	}
}

int qup_handle_transfer(unsigned int bus, const void *dout, void *din, int size)
{
	unsigned int m_irq;
	struct stopwatch sw;
	unsigned int rx_rem_bytes = din ? size : 0;
	unsigned int tx_rem_bytes = dout ? size : 0;
	struct qup_regs *regs = qup[bus].regs;

	stopwatch_init_msecs_expire(&sw, 1000);
	do {
		m_irq = qup_wait_for_m_irq(bus);
		if ((m_irq & M_RX_FIFO_WATERMARK_EN) ||
					(m_irq & M_RX_FIFO_LAST_EN))
			rx_rem_bytes -= handle_rx(bus, din + size
						- rx_rem_bytes, rx_rem_bytes);
		if (m_irq & M_TX_FIFO_WATERMARK_EN)
			tx_rem_bytes -= handle_tx(bus, dout + size
						- tx_rem_bytes, tx_rem_bytes);
		if (m_irq & M_CMD_DONE_EN) {
			write32(&regs->geni_m_irq_clear, m_irq);
			break;
		}
		write32(&regs->geni_m_irq_clear, m_irq);
	} while (!stopwatch_expired(&sw));

	if (!(m_irq & M_CMD_DONE_EN) || tx_rem_bytes || rx_rem_bytes) {
		printk(BIOS_INFO, "%s:Error: Transfer failed\n", __func__);
		qup_m_cancel_and_abort(bus);
		return -1;
	}
	return 0;
}
