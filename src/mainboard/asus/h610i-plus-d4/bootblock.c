/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>

// This board actually uses an NCT6798D, but NCT6791D seems compatible
#include <superio/nuvoton/nct6791d/nct6791d.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6791D_SP1)
#define P80_UART_DEV PNP_DEV(0x2e, NCT6791D_PORT80)
#define ACPI_DEV PNP_DEV(0x2e, NCT6791D_ACPI)
#define WDTMEM_DEV PNP_DEV(0x2e, NCT6791D_BCLK_WDT2_WDTMEM)
#define CIRWUP_DEV PNP_DEV(0x2e, NCT6791D_CIRWUP)
#define DS_DEV PNP_DEV(0x2e, NCT6791D_DS)

void bootblock_mainboard_early_init(void)
{
	// Start configuring the SIO
	nuvoton_pnp_enter_conf_state(P80_UART_DEV);

	// Replicate vendor global config LDN
	pnp_write_config(P80_UART_DEV, 0x07, 0x0b);
	pnp_write_config(P80_UART_DEV, 0x10, 0xcf);
	pnp_write_config(P80_UART_DEV, 0x11, 0xc3);
	pnp_write_config(P80_UART_DEV, 0x13, 0x0c);
	pnp_write_config(P80_UART_DEV, 0x14, 0xb8);
	pnp_write_config(P80_UART_DEV, 0x1a, 0x10);
	pnp_write_config(P80_UART_DEV, 0x1b, 0x00);
	pnp_write_config(P80_UART_DEV, 0x1c, 0x10);
	pnp_write_config(P80_UART_DEV, 0x24, 0x00);
	pnp_write_config(P80_UART_DEV, 0x27, 0x00);
	pnp_write_config(P80_UART_DEV, 0x2a, 0x58);
	pnp_write_config(P80_UART_DEV, 0x2b, 0x02); // Enable the Port 80 UART
	pnp_write_config(P80_UART_DEV, 0x2c, 0x0a);
	pnp_write_config(P80_UART_DEV, 0x2d, 0x30);

	// Configure the ACPI LD to match stock
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe5, 0x02);
	pnp_write_config(ACPI_DEV, 0xe6, 0x1a);
	pnp_write_config(ACPI_DEV, 0xf2, 0x5d);

	// Configure BCLK/WDT2/WDTMEM
	pnp_set_logical_device(WDTMEM_DEV);
	pnp_write_config(WDTMEM_DEV, 0xe2, 0x03);
	pnp_write_config(WDTMEM_DEV, 0xf0, 0x80);

	// Configure CIR Wakeup
	pnp_set_logical_device(CIRWUP_DEV);
	pnp_write_config(CIRWUP_DEV, 0x30, 0x00);

	// Configure Deep Sleep
	pnp_set_logical_device(DS_DEV);
	pnp_write_config(DS_DEV, 0x30, 0xa0); // Disable deep S5

	nuvoton_pnp_exit_conf_state(ACPI_DEV);

	// Enable serial port
	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
