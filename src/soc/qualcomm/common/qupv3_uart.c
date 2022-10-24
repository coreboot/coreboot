/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <soc/clock.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>
#include <types.h>

/* COMMON STATUS/CONFIGURATION REGISTERS AND MASKS */

#define GENI_STATUS_M_GENI_CMD_ACTIVE_MASK	0x1
#define GENI_STATUS_S_GENI_CMD_ACTIVE_MASK	0x1000

#define UART_TX_WATERMARK_MARGIN	4	/* Represented in words */
#define UART_RX_WATERMARK_MARGIN	8	/* Represented in words */
#define UART_RX_RFR_WATERMARK_MARGIN	4	/* Represented in words */
#define UART_TX_BITS_PER_WORD	8
#define UART_RX_BITS_PER_WORD	8
#define START_UART_TX		0x8000000
#define START_UART_RX		0x8000000

/* UART FIFO Packing Configuration. */
/* Start_idx:0, direction:0, len:7, stop:0 */
#define UART_TX_PACK_VECTOR0	0x0E
/* Start_idx:8, direction:0, len:7, stop:0 */
#define UART_TX_PACK_VECTOR1	0x10E
/* Start_idx:16, direction:0, len:7, stop:0 */
#define UART_TX_PACK_VECTOR2	0x20E
/* Start_idx:24, direction:0, len:7, stop:1 */
#define UART_TX_PACK_VECTOR3	0x30F
/* Start_idx:0, direction:0, len:7, stop:1 */
#define UART_RX_PACK_VECTOR0	0xF
#define UART_RX_PACK_VECTOR2	0x00

void uart_tx_flush(unsigned int idx)
{
	struct qup_regs *regs = qup[idx].regs;

	while (read32(&regs->geni_status) &
	       GENI_STATUS_M_GENI_CMD_ACTIVE_MASK)
		;
}

void uart_init(unsigned int idx)
{
	struct qup_regs *regs = qup[idx].regs;
	unsigned int reg_value;
	unsigned int div, baud_rate, uart_freq;

	/*
	 * If the RX (secondary) sequencer is already active, it means the core
	 * has been already initialized in the previous stage. Skip
	 * configuration
	 */
	if (read32(&regs->geni_status) & GENI_STATUS_S_GENI_CMD_ACTIVE_MASK)
		return;

	qupv3_se_fw_load_and_init(idx, SE_PROTOCOL_UART, FIFO);
	clock_enable_qup(idx);

	reg_value = read32(&regs->geni_fw_revision_ro);
	reg_value &= GENI_FW_REVISION_RO_PROTOCOL_MASK;
	reg_value >>= GENI_FW_REVISION_RO_PROTOCOL_SHIFT;

	assert(reg_value == SE_PROTOCOL_UART);

	baud_rate = get_uart_baudrate();

	/*requires 16 clock pulses to sample 1 bit of data */
	uart_freq = baud_rate * 16;

	div = DIV_ROUND_CLOSEST(SRC_XO_HZ, uart_freq);
	write32(&regs->geni_ser_m_clk_cfg, (div << 4) | 1);
	write32(&regs->geni_ser_s_clk_cfg, (div << 4) | 1);

	/* GPIO Configuration */
	gpio_configure(qup[idx].pin[2], qup[idx].func[2], GPIO_PULL_UP,
				GPIO_2MA, GPIO_OUTPUT);
	gpio_configure(qup[idx].pin[3], qup[idx].func[3], GPIO_PULL_UP,
				GPIO_2MA, GPIO_INPUT);

	write32(&regs->geni_tx_watermark_reg, UART_TX_WATERMARK_MARGIN);
	write32(&regs->geni_rx_watermark_reg, FIFO_DEPTH
					- UART_RX_WATERMARK_MARGIN);
	write32(&regs->geni_rx_rfr_watermark_reg,
			FIFO_DEPTH - UART_RX_RFR_WATERMARK_MARGIN);

	write32(&regs->uart_tx_word_len, UART_TX_BITS_PER_WORD);
	write32(&regs->uart_rx_word_len, UART_RX_BITS_PER_WORD);

	/* Disable TX parity calculation */
	write32(&regs->uart_tx_parity_cfg, 0x0);
	/* Ignore CTS line status for TX communication */
	write32(&regs->uart_tx_trans_cfg_reg, 0x2);
	/* Disable RX parity calculation */
	write32(&regs->uart_rx_parity_cfg, 0x0);
	/* Disable parity, framing and break check on received word */
	write32(&regs->uart_rx_trans_cfg, 0x0);
	/* Set UART TX stop bit len to one UART bit length */
	write32(&regs->uart_tx_stop_bit_len, 0x0);
	write32(&regs->uart_rx_stale_cnt, 0x16 * 10);

	write32(&regs->geni_tx_packing_cfg0, UART_TX_PACK_VECTOR0 |
						(UART_TX_PACK_VECTOR1 << 10));
	write32(&regs->geni_tx_packing_cfg1, UART_TX_PACK_VECTOR2 |
						(UART_TX_PACK_VECTOR3 << 10));
	write32(&regs->geni_rx_packing_cfg0, UART_RX_PACK_VECTOR0);
	write32(&regs->geni_rx_packing_cfg1, UART_RX_PACK_VECTOR2);

	/* Start RX */
	write32(&regs->geni_s_cmd0, START_UART_RX);
}

unsigned char uart_rx_byte(unsigned int idx)
{
	struct qup_regs *regs = qup[idx].regs;

	if (read32(&regs->geni_rx_fifo_status) & RX_FIFO_WC_MSK)
		return read32(&regs->geni_rx_fifon) & 0xFF;
	return 0;
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	struct qup_regs *regs = qup[idx].regs;

	uart_tx_flush(idx);

	write32(&regs->uart_tx_trans_len, 1);
	/* Start TX */
	write32(&regs->geni_m_cmd0, START_UART_TX);
	write32(&regs->geni_tx_fifon, data);
}

uintptr_t uart_platform_base(unsigned int idx)
{
	return (uintptr_t)qup[idx].regs;
}

enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial->baseaddr = (uint32_t)uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial->baud = get_uart_baudrate();
	serial->regwidth = 4;
	serial->input_hertz = SRC_XO_HZ;

	return CB_SUCCESS;
}
