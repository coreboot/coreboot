/*
 * This file is part of the coreboot project.
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

#ifndef _CONSOLE_STREAMS_H_
#define _CONSOLE_STREAMS_H_

void console_hw_init(void);
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);

/* For remote GDB debugging. */
void gdb_hw_init(void);
void gdb_tx_byte(unsigned char byte);
void gdb_tx_flush(void);
unsigned char gdb_rx_byte(void);

#endif /* _CONSOLE_STREAMS_H_ */
