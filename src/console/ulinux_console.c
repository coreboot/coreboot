#include <console/console.h>
#include <ulinux.h>

static void linux_tx_byte(unsigned char byte)
{
	unsigned char c;
	c = byte;

	ulinux_write(1, &c, 1);
}


static const struct console_driver linux_console __console = {
	.init    = 0,
	.tx_byte = linux_tx_byte,
	.rx_byte = 0,
	.tst_byte = 0,
};
