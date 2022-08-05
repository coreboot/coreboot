/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_MENDOCINO_UART_H
#define AMD_MENDOCINO_UART_H

#include <types.h>

void set_uart_config(unsigned int idx); /* configure hardware of FCH UART selected by idx */
void clear_uart_legacy_config(void); /* disable legacy I/O decode for FCH UART */

#endif /* AMD_MENDOCINO_UART_H */
