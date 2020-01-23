/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_SMSC_SIO1007_H
#define SUPERIO_SMSC_SIO1007_H

#include <stdint.h>

void sio1007_setreg(u16 lpc_port, u8 reg, u8 value, u8 mask);
int sio1007_enable_uart_at(u16 port);

#endif
