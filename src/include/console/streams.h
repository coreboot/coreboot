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


#if defined(__BOOTBLOCK__) || defined(__PRE_RAM__) && defined(__ROMCC__)
#define ENV_BOOTBLOCK 1
#else
#define ENV_BOOTBLOCK 0
#endif

#if defined(__PRE_RAM__) && !defined(__ROMCC__)
#define ENV_ROMSTAGE 1
#else
#define ENV_ROMSTAGE 0
#endif

#if !defined(__PRE_RAM__) && !defined(__SMM__)
#define ENV_RAMSTAGE 1
#else
#define ENV_RAMSTAGE 0
#endif

#if defined(__SMM__)
#define ENV_SMM 1
#else
#define ENV_SMM 0
#endif

void console_hw_init(void);
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);

/* Helpers for ROMCC console. */
void console_tx_nibble(unsigned nibble);
void console_tx_hex8(unsigned char value);
void console_tx_hex16(unsigned short value);
void console_tx_hex32(unsigned int value);
void console_tx_string(const char *str);

#endif /* _CONSOLE_STREAMS_H_ */
