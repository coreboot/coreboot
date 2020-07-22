/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/cbmem_console.h>
#include <console/ne2k.h>
#include <console/qemu_debugcon.h>
#include <console/spkmodem.h>
#include <console/streams.h>
#include <console/uart.h>
#include <console/usb.h>
#include <console/spi.h>
#include <console/flash.h>
#include <console/system76_ec.h>

void console_hw_init(void)
{
	__cbmemc_init();
	__spkmodem_init();
	__qemu_debugcon_init();

	__uart_init();
	__ne2k_init();
	__usbdebug_init();
	__spiconsole_init();
	__flashconsole_init();
	__system76_ec_init();
}

void console_tx_byte(unsigned char byte)
{
	__cbmemc_tx_byte(byte);
	__spkmodem_tx_byte(byte);
	__qemu_debugcon_tx_byte(byte);

	/* Some consoles want newline conversion
	 * to keep terminals happy.
	 */
	if (byte == '\n') {
		__uart_tx_byte('\r');
		__usb_tx_byte('\r');
	}

	__uart_tx_byte(byte);
	__ne2k_tx_byte(byte);
	__usb_tx_byte(byte);
	__spiconsole_tx_byte(byte);
	__flashconsole_tx_byte(byte);
	__system76_ec_tx_byte(byte);
}

void console_tx_flush(void)
{
	__uart_tx_flush();
	__ne2k_tx_flush();
	__usb_tx_flush();
	__flashconsole_tx_flush();
	__system76_ec_tx_flush();
}

void console_write_line(uint8_t *buffer, size_t number_of_bytes)
{
	/* Finish displaying all of the console data if requested */
	if (number_of_bytes == 0) {
		console_tx_flush();
		return;
	}

	/* Output the console data */
	while (number_of_bytes--)
		console_tx_byte(*buffer++);
}


#if CONFIG(GDB_STUB) && (ENV_ROMSTAGE || ENV_RAMSTAGE)
void gdb_hw_init(void)
{
	__gdb_hw_init();
}

void gdb_tx_byte(unsigned char byte)
{
	__gdb_tx_byte(byte);
}

void gdb_tx_flush(void)
{
	__gdb_tx_flush();
}

unsigned char gdb_rx_byte(void)
{
	return __gdb_rx_byte();
}
#endif
