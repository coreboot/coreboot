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

#include <console/streams.h>

void console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	console_tx_byte(digit);
}

void console_tx_hex8(unsigned char value)
{
	console_tx_nibble((value >>  4U) & 0x0fU);
	console_tx_nibble(value & 0x0fU);
}

void console_tx_hex16(unsigned short value)
{
	console_tx_nibble((value >> 12U) & 0x0fU);
	console_tx_nibble((value >>  8U) & 0x0fU);
	console_tx_nibble((value >>  4U) & 0x0fU);
	console_tx_nibble(value & 0x0fU);
}

void console_tx_hex32(unsigned int value)
{
	console_tx_nibble((value >> 28U) & 0x0fU);
	console_tx_nibble((value >> 24U) & 0x0fU);
	console_tx_nibble((value >> 20U) & 0x0fU);
	console_tx_nibble((value >> 16U) & 0x0fU);
	console_tx_nibble((value >> 12U) & 0x0fU);
	console_tx_nibble((value >>  8U) & 0x0fU);
	console_tx_nibble((value >>  4U) & 0x0fU);
	console_tx_nibble(value & 0x0fU);
}

void console_tx_string(const char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		if (ch == '\n')
			console_tx_byte('\r');
		console_tx_byte(ch);
	}
}
