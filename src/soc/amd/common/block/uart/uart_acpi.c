/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/uart.h>
#include <device/device.h>

/* This gets called for both enabled and disabled devices. */
void uart_inject_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));

	acpigen_write_STA(acpi_device_status(dev));

	acpigen_pop_len(); /* Scope */
}
