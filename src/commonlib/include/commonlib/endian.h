/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef _COMMONLIB_ENDIAN_H_
#define _COMMONLIB_ENDIAN_H_

#include <stddef.h>
#include <stdint.h>

/* Endian agnostic functions working on single byte. */

static inline uint8_t read_ble8(const void *src)
{
	const uint8_t *s = src;
	return *s;
}

static inline uint8_t read_at_ble8(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_ble8(s);
}

static inline void write_ble8(void *dest, uint8_t val)
{
	*(uint8_t *)dest = val;
}

static inline void write_at_ble8(void *dest, uint8_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_ble8(d, val);
}

/* Big Endian functions. */

static inline uint8_t read_be8(const void *src)
{
	return read_ble8(src);
}

static inline uint8_t read_at_be8(const void *src, size_t offset)
{
	return read_at_ble8(src, offset);
}

static inline void write_be8(void *dest, uint8_t val)
{
	write_ble8(dest, val);
}

static inline void write_at_be8(void *dest, uint8_t val, size_t offset)
{
	write_at_ble8(dest, val, offset);
}

static inline uint16_t read_be16(const void *src)
{
	const uint8_t *s = src;
	return (((uint16_t)s[0]) << 8) | (((uint16_t)s[1]) << 0);
}

static inline uint16_t read_at_be16(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_be16(s);
}

static inline void write_be16(void *dest, uint16_t val)
{
	write_be8(dest, val >> 8);
	write_at_be8(dest, val >> 0, sizeof(uint8_t));
}

static inline void write_at_be16(void *dest, uint16_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_be16(d, val);
}

static inline uint32_t read_be32(const void *src)
{
	const uint8_t *s = src;
	return (((uint32_t)s[0]) << 24) | (((uint32_t)s[1]) << 16) |
		(((uint32_t)s[2]) << 8) | (((uint32_t)s[3]) << 0);
}

static inline uint32_t read_at_be32(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_be32(s);
}

static inline void write_be32(void *dest, uint32_t val)
{
	write_be16(dest, val >> 16);
	write_at_be16(dest, val >> 0, sizeof(uint16_t));
}

static inline void write_at_be32(void *dest, uint32_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_be32(d, val);
}

static inline uint64_t read_be64(const void *src)
{
	uint64_t val;
	val = read_be32(src);
	val <<= 32;
	val |= read_at_be32(src, sizeof(uint32_t));
	return val;
}

static inline uint64_t read_at_be64(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_be64(s);
}

static inline void write_be64(void *dest, uint64_t val)
{
	write_be32(dest, val >> 32);
	write_at_be32(dest, val >> 0, sizeof(uint32_t));
}

static inline void write_at_be64(void *dest, uint64_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_be64(d, val);
}

/* Little Endian functions. */

static inline uint8_t read_le8(const void *src)
{
	return read_ble8(src);
}

static inline uint8_t read_at_le8(const void *src, size_t offset)
{
	return read_at_ble8(src, offset);
}

static inline void write_le8(void *dest, uint8_t val)
{
	write_ble8(dest, val);
}

static inline void write_at_le8(void *dest, uint8_t val, size_t offset)
{
	write_at_ble8(dest, val, offset);
}

static inline uint16_t read_le16(const void *src)
{
	const uint8_t *s = src;
	return (((uint16_t)s[1]) << 8) | (((uint16_t)s[0]) << 0);
}

static inline uint16_t read_at_le16(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_le16(s);
}

static inline void write_le16(void *dest, uint16_t val)
{
	write_le8(dest, val >> 0);
	write_at_le8(dest, val >> 8, sizeof(uint8_t));
}

static inline void write_at_le16(void *dest, uint16_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_le16(d, val);
}

static inline uint32_t read_le32(const void *src)
{
	const uint8_t *s = src;
	return (((uint32_t)s[3]) << 24) | (((uint32_t)s[2]) << 16) |
		(((uint32_t)s[1]) << 8) | (((uint32_t)s[0]) << 0);
}

static inline uint32_t read_at_le32(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_le32(s);
}

static inline void write_le32(void *dest, uint32_t val)
{
	write_le16(dest, val >> 0);
	write_at_le16(dest, val >> 16, sizeof(uint16_t));
}

static inline void write_at_le32(void *dest, uint32_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_le32(d, val);
}

static inline uint64_t read_le64(const void *src)
{
	uint64_t val;
	val = read_at_le32(src, sizeof(uint32_t));
	val <<= 32;
	val |= read_le32(src);
	return val;
}

static inline uint64_t read_at_le64(const void *src, size_t offset)
{
	const uint8_t *s = src;
	s += offset;
	return read_le64(s);
}

static inline void write_le64(void *dest, uint64_t val)
{
	write_le32(dest, val >> 0);
	write_at_le32(dest, val >> 32, sizeof(uint32_t));
}

static inline void write_at_le64(void *dest, uint64_t val, size_t offset)
{
	uint8_t *d = dest;
	d += offset;
	write_le64(d, val);
}

#endif
