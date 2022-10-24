/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <device/device.h>
#include <delay.h>
#include <stdint.h>
#include "uart8250reg.h"

/* Should support 8250, 16450, 16550, 16550A type UARTs */

/* Expected character delay at 1200bps is 9ms for a working UART
 * and no flow-control. Assume UART as stuck if shift register
 * or FIFO takes more than 50ms per character to appear empty.
 */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)
#define FIFO_TIMEOUT		(16 * SINGLE_CHAR_TIMEOUT)

#if CONFIG(DRIVERS_UART_8250MEM_32)
static uint8_t uart8250_read(void *base, uint8_t reg)
{
	return read32(base + 4 * reg) & 0xff;
}

static void uart8250_write(void *base, uint8_t reg, uint8_t data)
{
	write32(base + 4 * reg, data);
}
#else
static uint8_t uart8250_read(void *base, uint8_t reg)
{
	return read8(base + reg);
}

static void uart8250_write(void *base, uint8_t reg, uint8_t data)
{
	write8(base + reg, data);
}
#endif

static int uart8250_mem_can_tx_byte(void *base)
{
	return uart8250_read(base, UART8250_LSR) & UART8250_LSR_THRE;
}

static void uart8250_mem_tx_byte(void *base, unsigned char data)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !uart8250_mem_can_tx_byte(base))
		udelay(1);
	uart8250_write(base, UART8250_TBR, data);
}

static void uart8250_mem_tx_flush(void *base)
{
	unsigned long int i = FIFO_TIMEOUT;
	while (i-- && !(uart8250_read(base, UART8250_LSR) & UART8250_LSR_TEMT))
		udelay(1);
}

static int uart8250_mem_can_rx_byte(void *base)
{
	return uart8250_read(base, UART8250_LSR) & UART8250_LSR_DR;
}

static unsigned char uart8250_mem_rx_byte(void *base)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i && !uart8250_mem_can_rx_byte(base)) {
		udelay(1);
		i--;
	}
	if (i)
		return uart8250_read(base, UART8250_RBR);
	else
		return 0x0;
}

static void uart8250_mem_init(void *base, unsigned int divisor)
{
	/* Disable interrupts */
	uart8250_write(base, UART8250_IER, 0x0);
	/* Enable FIFOs */
	uart8250_write(base, UART8250_FCR, UART8250_FCR_FIFO_EN);

	/* Assert DTR and RTS so the other end is happy */
	uart8250_write(base, UART8250_MCR, UART8250_MCR_DTR | UART8250_MCR_RTS);

	/* DLAB on */
	uart8250_write(base, UART8250_LCR, UART8250_LCR_DLAB | CONFIG_TTYS0_LCS);

	uart8250_write(base, UART8250_DLL, divisor & 0xFF);
	uart8250_write(base, UART8250_DLM, (divisor >> 8) & 0xFF);

	/* Set to 3 for 8N1 */
	uart8250_write(base, UART8250_LCR, CONFIG_TTYS0_LCS);
}

void uart_init(unsigned int idx)
{
	void *base = uart_platform_baseptr(idx);
	if (!base)
		return;

	unsigned int div;
	div = uart_baudrate_divisor(get_uart_baudrate(),
		uart_platform_refclk(), uart_input_clock_divider());
	uart8250_mem_init(base, div);
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	void *base = uart_platform_baseptr(idx);
	if (!base)
		return;
	uart8250_mem_tx_byte(base, data);
}

unsigned char uart_rx_byte(unsigned int idx)
{
	void *base = uart_platform_baseptr(idx);
	if (!base)
		return 0xff;
	return uart8250_mem_rx_byte(base);
}

void uart_tx_flush(unsigned int idx)
{
	void *base = uart_platform_baseptr(idx);
	if (!base)
		return;
	uart8250_mem_tx_flush(base);
}

void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	if (!serial.baseaddr)
		return;
	serial.baud = get_uart_baudrate();
	if (CONFIG(DRIVERS_UART_8250MEM_32))
		serial.regwidth = sizeof(uint32_t);
	else
		serial.regwidth = sizeof(uint8_t);
	serial.input_hertz = uart_platform_refclk();
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
