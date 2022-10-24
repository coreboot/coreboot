/* Source : APQ8064 LK boot */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <delay.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/blsp.h>
#include <soc/ipq_uart.h>
#include <stdint.h>

#define FIFO_DATA_SIZE	4

typedef struct {
	void *uart_dm_base;
	uart_clk_mnd_t mnd_value;
	unsigned int blsp_uart;
	gpio_func_data_t dbg_uart_gpio[NO_OF_DBG_UART_GPIOS];
} uart_params_t;

static const uart_params_t uart_board_param = {
	.uart_dm_base = UART1_DM_BASE,
	.mnd_value = { 24, 625, 313 },
	.blsp_uart = BLSP1_UART1,
	.dbg_uart_gpio = {
		{
#if CONFIG(IPQ_QFN_PART)
			.gpio = 60,
			.func = 2,
#else	/* bga */
			.gpio = 16,
			.func = 1,
#endif
			.dir = GPIO_INPUT,
			.pull = GPIO_NO_PULL,
			.enable = GPIO_ENABLE
		},
		{
#if CONFIG(IPQ_QFN_PART)
			.gpio = 61,
			.func = 2,
#else	/* bga */
			.gpio = 17,
			.func = 1,
#endif
			.dir = GPIO_OUTPUT,
			.pull = GPIO_NO_PULL,
			.enable = GPIO_ENABLE
		},
	},
};

/**
 * @brief msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @param uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(void *uart_dm_base)
{
	/* Reset receiver */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CMD_RESET_RX);

	/* Enable receiver */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CR_RX_ENABLE);
	write32(MSM_BOOT_UART_DM_DMRX(uart_dm_base),
		MSM_BOOT_UART_DM_DMRX_DEF_VALUE);

	/* Clear stale event */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CMD_RES_STALE_INT);

	/* Enable stale event */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT);

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

static unsigned int msm_boot_uart_dm_init(void  *uart_dm_base);

/* Received data is valid or not */
static int valid_data = 0;

/* Received data */
static unsigned int word = 0;

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	int num_of_chars = 1;
	void *base = uart_board_param.uart_dm_base;

	/* Wait until transmit FIFO is empty. */
	while (!(read32(MSM_BOOT_UART_DM_SR(base)) &
		 MSM_BOOT_UART_DM_SR_TXEMT))
		udelay(1);
	/*
	 * TX FIFO is ready to accept new character(s). First write number of
	 * characters to be transmitted.
	 */
	write32(MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base), num_of_chars);

	/* And now write the character(s) */
	write32(MSM_BOOT_UART_DM_TF(base, 0), data);
}

/**
 * @brief msm_boot_uart_dm_reset - resets UART controller
 * @param base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(void *base)
{
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RESET_RX);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RESET_TX);
	write32(MSM_BOOT_UART_DM_CR(base),
		MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RES_TX_ERR);
	write32(MSM_BOOT_UART_DM_CR(base), MSM_BOOT_UART_DM_CMD_RES_STALE_INT);

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * @brief msm_boot_uart_dm_init - initilaizes UART controller
 * @param uart_dm_base: UART controller base address
 */
unsigned int msm_boot_uart_dm_init(void  *uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	write32(MSM_BOOT_UART_DM_MR1(uart_dm_base), 0x0);

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	write32(MSM_BOOT_UART_DM_MR2(uart_dm_base),
		MSM_BOOT_UART_DM_8_N_1_MODE);

	/* Configure Interrupt Mask register IMR */
	write32(MSM_BOOT_UART_DM_IMR(uart_dm_base),
		MSM_BOOT_UART_DM_IMR_ENABLED);

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	write32(MSM_BOOT_UART_DM_TFWR(uart_dm_base),
		MSM_BOOT_UART_DM_TFW_VALUE);

	/* RX watermark value */
	write32(MSM_BOOT_UART_DM_RFWR(uart_dm_base),
		MSM_BOOT_UART_DM_RFW_VALUE);

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	write32(MSM_BOOT_UART_DM_IPR(uart_dm_base),
		MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB);

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	write32(MSM_BOOT_UART_DM_IRDA(uart_dm_base), 0x0);

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	write32(MSM_BOOT_UART_DM_HCR(uart_dm_base), 0x0);

	/*
	 * Configure Rx FIFO base address
	 * Both TX/RX shares same SRAM and default is half-n-half.
	 * Sticking with default value now.
	 * As such RAM size is (2^RAM_ADDR_WIDTH, 32-bit entries).
	 * We have found RAM_ADDR_WIDTH = 0x7f
	 */

	/* Issue soft reset command */
	msm_boot_uart_dm_reset(uart_dm_base);

	/* Enable/Disable Rx/Tx DM interfaces */
	/* Data Mover not currently utilized. */
	write32(MSM_BOOT_UART_DM_DMEN(uart_dm_base), 0x0);

	/* Enable transmitter */
	write32(MSM_BOOT_UART_DM_CR(uart_dm_base),
		MSM_BOOT_UART_DM_CR_TX_ENABLE);

	/* Initialize Receive Path */
	msm_boot_uart_dm_init_rx_transfer(uart_dm_base);

	return 0;
}

/**
 * @brief ipq40xx_uart_init - initializes UART
 *
 * Initializes clocks, GPIO and UART controller.
 */
void uart_init(unsigned int idx)
{
	/* Note int idx isn't used in this driver. */
	void *dm_base;

	dm_base = uart_board_param.uart_dm_base;

	if (read32(MSM_BOOT_UART_DM_CSR(dm_base)) == UART_DM_CLK_RX_TX_BIT_RATE)
		return; /* UART must have been already initialized. */

	ipq_configure_gpio(uart_board_param.dbg_uart_gpio,
			   NO_OF_DBG_UART_GPIOS);

	/* Configure the uart clock */
	uart_clock_config(uart_board_param.blsp_uart,
		uart_board_param.mnd_value.m_value,
		uart_board_param.mnd_value.n_value,
		uart_board_param.mnd_value.d_value);

	write32(MSM_BOOT_UART_DM_CSR(dm_base), UART_DM_CLK_RX_TX_BIT_RATE);

	/* Initialize UART_DM */
	msm_boot_uart_dm_init(dm_base);
}

/* for the benefit of non-console uart init */
void ipq40xx_uart_init(void)
{
	uart_init(0);
}

/**
 * @brief uart_tx_flush - transmits a string of data
 * @param idx: string to transmit
 */
void uart_tx_flush(unsigned int idx)
{
	void *base = uart_board_param.uart_dm_base;

	while (!(read32(MSM_BOOT_UART_DM_SR(base)) &
		 MSM_BOOT_UART_DM_SR_TXEMT))
		;
}

/**
 * ipq40xx_serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
uint8_t uart_rx_byte(unsigned int idx)
{
	uint8_t byte;

	byte = (uint8_t)(word & 0xff);
	word = word >> 8;
	valid_data--;

	return byte;
}

/* TODO: Implement function */
enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial->baseaddr = (uint32_t)UART1_DM_BASE;
	serial->baud = get_uart_baudrate();
	serial->regwidth = 1;
	serial->input_hertz = uart_platform_refclk();

	return CB_SUCCESS;
}
