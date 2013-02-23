/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <console/ne2k.h>

static void ne2k_tx_byte(unsigned char data)
{
	ne2k_append_data(&data, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
}

static void ne2k_tx_flush(void)
{
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
}

static const struct console_driver ne2k_console __console = {
	.tx_byte = ne2k_tx_byte,
	.tx_flush = ne2k_tx_flush,
};
