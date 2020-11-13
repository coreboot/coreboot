/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_UART_H
#define AMD_PICASSO_UART_H

#include <types.h>

void set_uart_config(unsigned int idx); /* configure hardware of FCH UART selected by idx */
void clear_uart_legacy_config(void); /* disable legacy I/O decode for FCH UART */

uintptr_t get_uart_base(unsigned int idx); /* get MMIO base address of FCH UART */

#endif /* AMD_PICASSO_UART_H */
