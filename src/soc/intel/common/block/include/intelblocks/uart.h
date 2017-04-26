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

void uart_common_init(device_t dev, uintptr_t baseaddr,
		uint32_t clk_m_val, uint32_t clk_n_val);

void pch_uart_read_resources(struct device *dev);

#endif	/* SOC_INTEL_COMMON_BLOCK_UART_H */
