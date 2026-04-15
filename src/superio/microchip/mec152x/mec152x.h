/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_MICROCHIP_MEC152X_H
#define SUPERIO_MICROCHIP_MEC152X_H

#include <stdint.h>

/**
  * Configures the MEC152x at port (usually 0x2e) to enable the specified UART
  * and set it to the base address uart_iobase (usually 0x3f8 for COM1).
  * To be called from bootblock or romstage.
  * uart_no can be 0, 1 or 2.
  */
void mec152x_enable_early_uart(uint16_t port, uint8_t uart_no, uint16_t uart_iobase);

#endif
