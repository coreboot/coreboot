/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_UART_H
#define SOC_INTEL_COMMON_BLOCK_UART_H

#include <arch/io.h>
#include <console/uart.h>
#include <device/device.h>
#include <intelblocks/gpio.h>
#include <stdint.h>

#define MAX_GPIO_PAD_PER_UART		2

struct uart_gpio_pad_config {
	int console_index;
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
 * Common routine to initialize UART controller PCI config space, take it out of
 * reset and configure M/N dividers.
 */
void uart_common_init(struct device *dev, uintptr_t baseaddr);

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
struct device *uart_get_device(void);

/**************************** SoC callbacks ***********************************/

/*
 * SoC should implement soc_uart_console_to_device() function to
 * get UART debug controller device structure based on console number
 * Caller needs to check proper UART console index supported by SoC.
 * If wrong UART console index is passed to function, it'll return NULL.
 *
 * Input:
 * UART console index selected in config
 *
 * Returns:
 * Pointer to device structure = If device has a UART debug controller.
 * NULL = otherwise
 */
struct device *soc_uart_console_to_device(int uart_console);

/*
 * Set UART to legacy mode
 * Put UART in byte access mode for 16550 compatibility
 */
void soc_uart_set_legacy_mode(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_UART_H */
