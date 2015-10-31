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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include "common.h"

size_t bgets(struct buffer *input, void *output, size_t len)
{
	len = input->size < len ? input->size : len;
	memmove(output, input->data, len);
	input->data += len;
	input->size -= len;
	return len;
}

size_t bputs(struct buffer *b, const void *data, size_t len)
{
	memmove(&b->data[b->size], data, len);
	b->size += len;
	return len;
}

/* The assumption in all this code is that we're given a pointer to enough data.
 * Hence, we do not check for underflow.
 */
static uint8_t get8(struct buffer *input)
{
	uint8_t ret = *input->data++;
	input->size--;
	return ret;
}

static uint16_t get16be(struct buffer *input)
{
	uint16_t ret;
	ret = get8(input) << 8;
	ret |= get8(input);
	return ret;
}

static uint32_t get32be(struct buffer *input)
{
	uint32_t ret;
	ret = get16be(input) << 16;
	ret |= get16be(input);
	return ret;
}

static uint64_t get64be(struct buffer *input)
{
	uint64_t ret;
	ret = get32be(input);
	ret <<= 32;
	ret |= get32be(input);
	return ret;
}

static void put8(struct buffer *input, uint8_t val)
{
	input->data[input->size] = val;
	input->size++;
}

static void put16be(struct buffer *input, uint16_t val)
{
	put8(input, val >> 8);
	put8(input, val);
}

static void put32be(struct buffer *input, uint32_t val)
{
	put16be(input, val >> 16);
	put16be(input, val);
}

static void put64be(struct buffer *input, uint64_t val)
{
	put32be(input, val >> 32);
	put32be(input, val);
}

static uint16_t get16le(struct buffer *input)
{
	uint16_t ret;
	ret = get8(input);
	ret |= get8(input) << 8;
	return ret;
}

static uint32_t get32le(struct buffer *input)
{
	uint32_t ret;
	ret = get16le(input);
	ret |= get16le(input) << 16;
	return ret;
}

static uint64_t get64le(struct buffer *input)
{
	uint64_t ret;
	uint32_t low;
	low = get32le(input);
	ret = get32le(input);
	ret <<= 32;
	ret |= low;
	return ret;
}

static void put16le(struct buffer *input, uint16_t val)
{
	put8(input, val);
	put8(input, val >> 8);
}

static void put32le(struct buffer *input, uint32_t val)
{
	put16le(input, val);
	put16le(input, val >> 16);
}

static void put64le(struct buffer *input, uint64_t val)
{
	put32le(input, val);
	put32le(input, val >> 32);
}

struct xdr xdr_be = {
	get8, get16be, get32be, get64be,
	put8, put16be, put32be, put64be
};

struct xdr xdr_le = {
	get8, get16le, get32le, get64le,
	put8, put16le, put32le, put64le
};
