/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/periph.h>
#include <soc/uart.h>
#include <types.h>

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

static void serial_setbrg_dev(struct s5p_uart *uart)
{
	u32 uclk;
	u32 val;

	// All UARTs share the same clock.
	uclk = clock_get_periph_rate(PERIPH_ID_UART3);
	val = uclk / get_uart_baudrate();

	write32(&uart->ubrdiv, val / 16 - 1);
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void exynos5_init_dev(struct s5p_uart *uart)
{
	/* enable FIFOs */
	write32(&uart->ufcon, 0x1);
	write32(&uart->umcon, 0);
	/* 8N1 */
	write32(&uart->ulcon, 0x3);
	/* No interrupts, no DMA, pure polling */
	write32(&uart->ucon, 0x245);

	serial_setbrg_dev(uart);
}

static int exynos5_uart_err_check(struct s5p_uart *uart, int op)
{
	unsigned int mask;

	/*
	 * UERSTAT
	 * Break Detect	[3]
	 * Frame Err	[2] : receive operation
	 * Parity Err	[1] : receive operation
	 * Overrun Err	[0] : receive operation
	 */
	if (op)
		mask = 0x8;
	else
		mask = 0xf;

	return read32(&uart->uerstat) & mask;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is successful, the character read is
 * written into its argument c.
 */
static unsigned char exynos5_uart_rx_byte(struct s5p_uart *uart)
{
	/* wait for character to arrive */
	while (!(read32(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					 RX_FIFO_FULL_MASK))) {
		if (exynos5_uart_err_check(uart, 0))
			return 0;
	}

	return read8(&uart->urxh) & 0xff;
}

/*
 * Output a single byte to the serial port.
 */
static void exynos5_uart_tx_byte(struct s5p_uart *uart, unsigned char data)
{
	/* wait for room in the tx FIFO */
	while ((read32(&uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(uart, 1))
			return;
	}

	write8(&uart->utxh, data);
}

uintptr_t uart_platform_base(unsigned int idx)
{
	if (idx < 4)
		return 0x12c00000 + idx * 0x10000;
	else
		return 0;
}

void uart_init(unsigned int idx)
{
	struct s5p_uart *uart = uart_platform_baseptr(idx);
	exynos5_init_dev(uart);
}

unsigned char uart_rx_byte(unsigned int idx)
{
	struct s5p_uart *uart = uart_platform_baseptr(idx);
	return exynos5_uart_rx_byte(uart);
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	struct s5p_uart *uart = uart_platform_baseptr(idx);
	exynos5_uart_tx_byte(uart, data);
}

void uart_tx_flush(unsigned int idx)
{
	/* Exynos5250 implements this too. */
}

void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = get_uart_baudrate();
	serial.regwidth = 4;
	serial.input_hertz = uart_platform_refclk();
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
