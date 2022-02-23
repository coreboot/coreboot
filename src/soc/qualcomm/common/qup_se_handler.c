/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qcom_qup_se.h>

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
			/* Read whatever data available in FIFO */
			handle_rx(bus, buf, 64);
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

int qup_handle_transfer(unsigned int bus, const void *dout, void *din, int size,
			struct stopwatch *timeout)
{
	unsigned int m_irq;
	unsigned int rx_rem_bytes = din ? size : 0;
	unsigned int tx_rem_bytes = dout ? size : 0;
	struct qup_regs *regs = qup[bus].regs;

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
	} while (!stopwatch_expired(timeout));

	if (!(m_irq & M_CMD_DONE_EN) || tx_rem_bytes || rx_rem_bytes) {
		printk(BIOS_INFO, "%s:Error: Transfer failed\n", __func__);
		qup_m_cancel_and_abort(bus);
		return -1;
	}
	return 0;
}
