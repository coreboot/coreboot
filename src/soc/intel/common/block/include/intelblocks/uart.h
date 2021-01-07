/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_UART_H
#define SOC_INTEL_COMMON_BLOCK_UART_H

#include <console/uart.h>
#include <device/device.h>
#include <intelblocks/gpio.h>
#include <stdint.h>

#define MAX_GPIO_PAD_PER_UART		2

struct uart_controller_config {
	int console_index;
	/* devfn in PCI_DEVFN() format */
	unsigned int devfn;
	struct pad_config gpios[MAX_GPIO_PAD_PER_UART];
};

/*
 * While using this common UART block for any SOC following is expected from soc
 * 1. SOC will define proper UART_BASE which is base address for UART console.
 * 2. SOC will return correct device pointer based on console index
 * 3. SOC will provide appropriate GPIO pad configuration for UART console
 * 4. SOC will allow common code to set UART into legacy mode if supported.
 */

/*
 * Check if UART debug controller is initialized
 * Returns:
 * true = If debug controller PCI config space is initialized and device is
 *        out of reset
 * false = otherwise
 */
bool uart_is_controller_initialized(void);

/*
 * Check if dev corresponds to UART debug port controller.
 *
 * Returns:
 * true: UART dev is debug port
 * false: otherwise
 */
bool uart_is_debug_controller(struct device *dev);

/*
 * BootBlock pre initialization of UART console
 */
void uart_bootblock_init(void);

/*
 * Get UART debug controller device structure
 *
 * Returns:
 * Pointer to device structure = If device has a UART debug controller.
 * NULL = otherwise
 */
const struct device *uart_get_device(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_UART_H */
