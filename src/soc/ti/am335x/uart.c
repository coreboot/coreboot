/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <types.h>
#include <console/uart.h>
#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <soc/ti/am335x/uart.h>

#define EFR_ENHANCED_EN		(1 << 4)
#define FCR_FIFO_EN		(1 << 0)
#define MCR_TCR_TLR		(1 << 6)
#define SYSC_SOFTRESET		(1 << 1)
#define SYSS_RESETDONE		(1 << 0)

#define LSR_RXFIFOE		(1 << 0)
#define LSR_TXFIFOE		(1 << 5)

/*
 * Initialise the serial port with the given baudrate divisor. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void am335x_uart_init(struct am335x_uart *uart, uint16_t div)
{
	uint16_t lcr_orig, efr_orig, mcr_orig;

	/* reset the UART */
	write16(&uart->sysc, uart->sysc | SYSC_SOFTRESET);
	while (!(read16(&uart->syss) & SYSS_RESETDONE))
		;

	/* 1. switch to register config mode B */
	lcr_orig = read16(&uart->lcr);
	write16(&uart->lcr, 0xbf);

	/*
	 * 2. Set EFR ENHANCED_EN bit. To access this bit, registers must
	 * be in TCR_TLR submode, meaning EFR[4] = 1 and MCR[6] = 1.
	 */
	efr_orig = read16(&uart->efr);
	write16(&uart->efr, efr_orig | EFR_ENHANCED_EN);

	/* 3. Switch to register config mode A */
	write16(&uart->lcr, 0x80);

	/* 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR */
	mcr_orig = read16(&uart->mcr);
	write16(&uart->mcr, mcr_orig | MCR_TCR_TLR);

	/* 5. Enable the FIFO. For now we'll ignore FIFO triggers and DMA */
	write16(&uart->fcr, FCR_FIFO_EN);

	/* 6. Switch to configuration mode B */
	write16(&uart->lcr, 0xbf);
	/* Skip steps 7 and 8 (setting up FIFO triggers for DMA) */

	/* 9. Restore original EFR value */
	write16(&uart->efr, efr_orig);

	/* 10. Switch to config mode A */
	write16(&uart->lcr, 0x80);

	/* 11. Restore original MCR value */
	write16(&uart->mcr, mcr_orig);

	/* 12. Restore original LCR value */
	write16(&uart->lcr, lcr_orig);

	/* Protocol, baud rate and interrupt settings */

	/* 1. Disable UART access to DLL and DLH registers */
	write16(&uart->mdr1, read16(&uart->mdr1) | 0x7);

	/* 2. Switch to config mode B */
	write16(&uart->lcr, 0xbf);

	/* 3. Enable access to IER[7:4] */
	write16(&uart->efr, efr_orig | EFR_ENHANCED_EN);

	/* 4. Switch to operational mode */
	write16(&uart->lcr, 0x0);

	/* 5. Clear IER */
	write16(&uart->ier, 0x0);

	/* 6. Switch to config mode B */
	write16(&uart->lcr, 0xbf);

	/* 7. Set dll and dlh to the desired values (table 19-25) */
	write16(&uart->dlh, (div >> 8));
	write16(&uart->dll, (div & 0xff));

	/* 8. Switch to operational mode to access ier */
	write16(&uart->lcr, 0x0);

	/* 9. Clear ier to disable all interrupts */
	write16(&uart->ier, 0x0);

	/* 10. Switch to config mode B */
	write16(&uart->lcr, 0xbf);

	/* 11. Restore efr */
	write16(&uart->efr, efr_orig);

	/* 12. Set protocol formatting 8n1 (8 bit data, no parity, 1 stop bit) */
	write16(&uart->lcr, 0x3);

	/* 13. Load the new UART mode */
	write16(&uart->mdr1, 0x0);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is successful, the character read is
 * written into its argument c.
 */
static unsigned char am335x_uart_rx_byte(struct am335x_uart *uart)
{
	while (!(read16(&uart->lsr) & LSR_RXFIFOE));

	return read8(&uart->rhr);
}

/*
 * Output a single byte to the serial port.
 */
static void am335x_uart_tx_byte(struct am335x_uart *uart, unsigned char data)
{
	while (!(read16(&uart->lsr) & LSR_TXFIFOE));

	return write8(&uart->thr, data);
}

unsigned int uart_platform_refclk(void)
{
	return 48000000;
}

uintptr_t uart_platform_base(unsigned int idx)
{
	const unsigned int bases[] = {
		0x44e09000, 0x48022000, 0x48024000,
		0x481a6000, 0x481a8000, 0x481aa000
	};
	if (idx < ARRAY_SIZE(bases))
		return bases[idx];
	return 0;
}

void uart_init(unsigned int idx)
{
	struct am335x_uart *uart = uart_platform_baseptr(idx);
	uint16_t div = (uint16_t)uart_baudrate_divisor(
		get_uart_baudrate(), uart_platform_refclk(), 16);
	am335x_uart_init(uart, div);
}

unsigned char uart_rx_byte(unsigned int idx)
{
	struct am335x_uart *uart = uart_platform_baseptr(idx);
	return am335x_uart_rx_byte(uart);
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	struct am335x_uart *uart = uart_platform_baseptr(idx);
	am335x_uart_tx_byte(uart, data);
}

void uart_tx_flush(unsigned int idx)
{
}

enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial->baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial->baud = get_uart_baudrate();
	serial->regwidth = 2;
	serial->input_hertz = uart_platform_refclk();

	return CB_SUCCESS;
}
