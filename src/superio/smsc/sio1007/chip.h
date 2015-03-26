/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef SUPERIO_SMSC_1007_CHIP_H
#define SUPERIO_SMSC_1007_CHIP_H

/* FIXME: wrong place for this! */
void sio1007_setreg(u16 lpc_port, u8 reg, u8 value, u8 mask);
int sio1007_enable_uart_at(u16 port);

#endif
