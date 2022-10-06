/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Morgana */

#ifndef AMD_MORGANA_UART_H
#define AMD_MORGANA_UART_H

#include <types.h>

void set_uart_config(unsigned int idx); /* configure hardware of FCH UART selected by idx */
void clear_uart_legacy_config(void); /* disable legacy I/O decode for FCH UART */

#endif /* AMD_MORGANA_UART_H */
