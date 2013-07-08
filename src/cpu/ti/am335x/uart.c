/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#include <config.h>
#include <types.h>
#include <uart.h>
#include <arch/io.h>

#include <console/console.h>	/* for __console definition */

#include <cpu/ti/am335x/uart.h>

#define EFR_ENHANCED_EN		(1 << 4)
#define FCR_FIFO_EN		(1 << 0)
#define MCR_TCR_TLR		(1 << 6)
#define SYSC_SOFTRESET		(1 << 1)
#define SYSS_RESETDONE		(1 << 0)

#define LSR_RXFIFOE		(1 << 0)
#define LSR_TXFIFOE		(1 << 5)

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void am335x_uart_init_dev(void)
{
	struct am335x_uart *uart = (struct am335x_uart *)
					CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
	uint16_t lcr_orig, efr_orig, mcr_orig;

	/* reset the UART */
	write16(uart->sysc | SYSC_SOFTRESET, &uart->sysc);
	while (!(read16(&uart->syss) & SYSS_RESETDONE))
		;

	/* 1. switch to register config mode B */
	lcr_orig = read16(&uart->lcr);
	write16(0xbf, &uart->lcr);

	/*
	 * 2. Set EFR ENHANCED_EN bit. To access this bit, registers must
	 * be in TCR_TLR submode, meaning EFR[4] = 1 and MCR[6] = 1.
	 */
	efr_orig = read16(&uart->efr);
	write16(efr_orig | EFR_ENHANCED_EN, &uart->efr);

	/* 3. Switch to register config mode A */
	write16(0x80, &uart->lcr);

	/* 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR */
	mcr_orig = read16(&uart->mcr);
	write16(mcr_orig | MCR_TCR_TLR, &uart->mcr);

	/* 5. Enable the FIFO. For now we'll ignore FIFO triggers and DMA */
	write16(FCR_FIFO_EN, &uart->fcr);

	/* 6. Switch to configuration mode B */
	write16(0xbf, &uart->lcr);
	/* Skip steps 7 and 8 (setting up FIFO triggers for DMA) */

	/* 9. Restore original EFR value */
	write16(efr_orig, &uart->efr);

	/* 10. Switch to config mode A */
	write16(0x80, &uart->lcr);

	/* 11. Restore original MCR value */
	write16(mcr_orig, &uart->mcr);

	/* 12. Restore original LCR value */
	write16(lcr_orig, &uart->lcr);

	/* Protocol, baud rate and interrupt settings */

	/* 1. Disable UART access to DLL and DLH registers */
	write16(read16(&uart->mdr1) | 0x7, &uart->mdr1);

	/* 2. Switch to config mode B */
	write16(0xbf, &uart->lcr);

	/* 3. Enable access to IER[7:4] */
	write16(efr_orig | EFR_ENHANCED_EN, &uart->efr);

	/* 4. Switch to operational mode */
	write16(0x0, &uart->lcr);

	/* 5. Clear IER */
	write16(0x0, &uart->ier);

	/* 6. Switch to config mode B */
	write16(0xbf, &uart->lcr);

	/* 7. Set dll and dlh to the desired values (table 19-25) */
	if (CONFIG_CONSOLE_SERIAL_9600) {
		write16(0x01, &uart->dlh);
		write16(0x38, &uart->dll);
	} else if (CONFIG_CONSOLE_SERIAL_19200) {
		write16(0x00, &uart->dlh);
		write16(0x9c, &uart->dll);
	} else if (CONFIG_CONSOLE_SERIAL_38400) {
		write16(0x00, &uart->dlh);
		write16(0x4e, &uart->dll);
	} else if (CONFIG_CONSOLE_SERIAL_57600) {
		write16(0x00, &uart->dlh);
		write16(0x34, &uart->dll);
	} else if (CONFIG_CONSOLE_SERIAL_115200) {
		write16(0x00, &uart->dlh);
		write16(0x1a, &uart->dll);
	} else {
		/* Unrecognized baud rate? */
	}

	/* 8. Switch to operational mode to access ier */
	write16(0x0, &uart->lcr);

	/* 9. Clear ier to disable all interrupts */
	write16(0x0, &uart->ier);

	/* 10. Switch to config mode B */
	write16(0xbf, &uart->lcr);

	/* 11. Restore efr */
	write16(efr_orig, &uart->efr);

	/* 12. Set protocol formatting 8n1 (8 bit data, no parity, 1 stop bit) */
	write16(0x3, &uart->lcr);

	/* 13. Load the new UART mode */
	write16(0x0, &uart->mdr1);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is successful, the character read is
 * written into its argument c.
 */
static unsigned char am335x_uart_rx_byte(void)
{
	struct am335x_uart *uart =
		(struct am335x_uart *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

	while (!(read16(&uart->lsr) & LSR_RXFIFOE));

	return read8(&uart->rhr);
}

/*
 * Output a single byte to the serial port.
 */
static void am335x_uart_tx_byte(unsigned char data)
{
	struct am335x_uart *uart =
		(struct am335x_uart *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

	while (!(read16(&uart->lsr) & LSR_TXFIFOE));

	return write8(data, &uart->thr);
}

uint32_t uartmem_getbaseaddr(void)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}

#if !defined(__PRE_RAM__)
static const struct console_driver exynos5_uart_console __console = {
	.init     = am335x_uart_init_dev,
	.tx_byte  = am335x_uart_tx_byte,
	.rx_byte  = am335x_uart_rx_byte,
};
#else
void uart_init(void)
{
	am335x_uart_init_dev();
}

unsigned char uart_rx_byte(void)
{
	return am335x_uart_rx_byte();
}

void uart_tx_byte(unsigned char data)
{
	am335x_uart_tx_byte(data);
}

void uart_tx_flush(void) {
}
#endif
