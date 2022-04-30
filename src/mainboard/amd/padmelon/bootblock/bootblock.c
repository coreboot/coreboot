/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/southbridge.h>
#include <amdblocks/lpc.h>
#include <device/pci_ops.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <drivers/uart/uart8250reg.h>
#include <arch/io.h>
#include "../gpio.h"

/* Enable IO access to port, then enable UART HW control pins */
static void enable_serial(unsigned int base_port, unsigned int io_enable)
{
	u8 reg;

	pci_or_config32(SOC_LPC_DEV, LPC_IO_PORT_DECODE_ENABLE, io_enable);

	/*
	 * Remove this section if HW handshake is not needed. This is needed
	 * only for those who don't have a modified serial cable (connecting
	 * DCD to DTR and DSR, plus connecting RTS to CTS). When you buy cables
	 * on any store, they don't have these modification.
	 */
	reg = inb(base_port + UART8250_MCR);
	reg |= UART8250_MCR_DTR | UART8250_MCR_RTS;
	outb(reg, base_port + UART8250_MCR);
}

void bootblock_mainboard_early_init(void)
{
	fch_clk_output_48Mhz(2);
	/*
	 * UARTs enabled by default at reset, just need RTS, CTS
	 * and access to the IO address.
	 */
	enable_serial(0x03f8, DECODE_ENABLE_SERIAL_PORT0);
	enable_serial(0x02f8, DECODE_ENABLE_SERIAL_PORT1);
}

void bootblock_mainboard_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	gpios = early_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
