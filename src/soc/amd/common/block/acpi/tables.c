/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <console/uart.h>
#include <types.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	current = acpi_write_hpet(device, current, rsdp);

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		current = acpi_16550_mmio32_write_dbg2_uart(rsdp, current,
			uart_platform_base(get_uart_for_console()), NULL);

	return current;
}
