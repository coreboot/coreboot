/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_UART_H__
#define __PICASSO_UART_H__

#include <types.h>

void set_uart_config(unsigned int idx); /* configure hardware of FCH UART selected by idx */
void clear_uart_legacy_config(void); /* disable legacy I/O decode for FCH UART */

uintptr_t get_uart_base(unsigned int idx); /* get MMIO base address of FCH UART */

#endif /* __PICASSO_UART_H__ */
