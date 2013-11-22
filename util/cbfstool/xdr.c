 /*
 * cbfstool, CLI utility for CBFS file manipulation
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include "common.h"

/* The assumption in all this code is that we're given a pointer to enough data.
 * Hence, we do not check for underflow.
 */
uint8_t get8(struct buffer *input)
{
	uint8_t ret = *input->data++;
	input->size--;
	return ret;
}

uint16_t get16be(struct buffer *input)
{
	uint16_t ret;
	ret = get8(input)<<8 | get8(input);
	return ret;
}

uint32_t get32be(struct buffer *input)
{
	uint32_t ret;
	ret = get16be(input)<<16 | get16be(input);
	return ret;
}

uint64_t get64be(struct buffer *input)
{
	uint64_t ret;
	ret = get32be(input);
	ret <<= 32;
	ret |= get32be(input);
	return ret;
}

void put8(struct buffer *input, uint8_t val)
{
	*input->data++ = val;
	input->size++;
}

void put16be(struct buffer *input, uint16_t val)
{
	put8(input, val>>8);
	put8(input, val);
}

void put32be(struct buffer *input, uint32_t val)
{
	put16be(input, val>>16);
	put16be(input, val);
}

void put64be(struct buffer *input, uint64_t val)
{
	put32be(input, val>>32);
	put32be(input, val);
}

uint16_t get16le(struct buffer *input)
{
	uint16_t ret;
	ret = get8(input) | get8(input) << 8;
	return ret;
}

uint32_t get32le(struct buffer *input)
{
	uint32_t ret;
	ret = get16le(input) | get16le(input) << 16;
	return ret;
}

uint64_t get64le(struct buffer *input)
{
	uint64_t ret;
	uint32_t low;
	low = get32le(input);
	ret = get32le(input);
	ret <<= 32;
	ret |= low;
	return ret;
}

void put16le(struct buffer *input, uint16_t val)
{
	put8(input, val);
	put8(input, val>>8);
}

void put32le(struct buffer *input, uint32_t val)
{
	put16le(input, val);
	put16le(input, val>>16);
}

void put64le(struct buffer *input, uint64_t val)
{
	put32le(input, val);
	put32le(input, val>>32);
}

struct xdr xdr_be = {
	get16le, get32le, get64le,
	put16be, put32be, put64be
};

struct xdr xdr_le = {
	get16le, get32le, get64le,
	put16le, put32le, put64le
};

