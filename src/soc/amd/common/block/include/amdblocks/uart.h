/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_UART_H
#define AMD_BLOCK_UART_H

#include <device/device.h>
#include <types.h>

uintptr_t get_uart_base(unsigned int idx); /* get MMIO base address of FCH UART */
void uart_inject_ssdt(const struct device *dev);

#endif /* AMD_BLOCK_UART_H */
