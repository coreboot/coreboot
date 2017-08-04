/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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
#include <device/device.h>
#include <stdint.h>

/*
 * Common routine to initialize UART controller PCI config space, take it out of
 * reset and configure M/N dividers.
 */
void uart_common_init(device_t dev, uintptr_t baseaddr);

/*
 * Check if UART debug controller is initialized
 * Returns:
 * true = If debug controller PCI config space is initialized and device is
 *        out of reset
 * false = otherwise
 */
bool uart_debug_controller_is_initialized(void);

/*
 * Check if dev corresponds to UART debug port controller.
 *
 * Returns:
 * true: UART dev is debug port
 * false: otherwise
 */
bool uart_is_debug_controller(struct device *dev);

/**************************** SoC callbacks ***********************************/

void pch_uart_read_resources(struct device *dev);

/*
 * Check if UART debug port controller needs to be initialized on resume.
 *
 * Returns:
 * true = when SoC wants common code to do the UART debug port initialization
 * false = otherwise
 */
bool pch_uart_init_debug_controller_on_resume(void);

/*
 * Get UART debug controller device structure
 *
 * Returns:
 * Pointer to device structure = If device has a UART debug controller.
 * NULL = otherwise
 */
device_t pch_uart_get_debug_controller(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_UART_H */
