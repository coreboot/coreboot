/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_UART_H
#define AMD_BLOCK_UART_H

#include <amdblocks/gpio.h>
#include <device/device.h>
#include <types.h>

struct soc_uart_ctrlr_info {
	uintptr_t base;
	unsigned int aoac_device;
	const char *acpi_name;
	struct soc_amd_gpio mux[2];
};

uintptr_t get_uart_base(unsigned int idx); /* get MMIO base address of FCH UART */
void uart_inject_ssdt(const struct device *dev);

#endif /* AMD_BLOCK_UART_H */
