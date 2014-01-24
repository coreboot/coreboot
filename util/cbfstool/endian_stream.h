/*
 * Endian conversion helpers
 *
 * These helpers are based on the idea that outside data is a stream of well
 * defined structure (including byte order), and that no assumptions are made
 * about the internal representation of data on the host. For example, there is
 * no notion of byte order, structure packing on the host.
 *
 * They allow the proper serialization and deserialization of data from host
 * structs to data streams without relying on the notion of byte swaps.
 *
 * To enforce this philosophy, these endian conversion helpers always treat host
 * data as standard integers types, and external data as streams identified by
 * a pointer.
 *
 * So rather than extracting a structure in the following incorrect way:
 *         memcpy(host_struct, src, size);
 *         host_struct->member1 = ntoh(host_struct->member)
 *         host_struct->member2 = ntohl(host_struct->member2)
 *
 * These helpers make the process more portable and less compiler-dependent:
 *         host_struct->member1 = be16_to_h(src + 0);
 *         host_struct->member2 = be32_to_h(src + 2);
 *
 * Copyright (C) 2014  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef __CBFSTOOL_ENDIAN_STREAM_H
#define __CBFSTOOL_ENDIAN_STREAM_H

inline static uint8_t le8_to_h(const void *src)
{
	uint8_t *b = src;
	return b[0];
};

inline static uint16_t le16_to_h(const void *src)
{
	uint8_t *b = src;
	return ((b[1] << 8) | (b[0] << 0));
};

inline static uint32_t le32_to_h(const void *src)
{
	uint8_t *b = src;
	return ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0] << 0));
};

inline static void h_to_le8(uint8_t val8, void *dest)
{
	uint8_t *b = dest;
	b[0] = val8;
};

inline static void h_to_le16(uint16_t val16, void *dest)
{
	uint8_t *b = dest;
	b[0] = (val16 >> 0) & 0xff;
	b[1] = (val16 >> 8) & 0xff;
};

inline static void h_to_le32(uint32_t val32, void *dest)
{
	uint8_t *b = dest;
	b[0] = (val32 >> 0) & 0xff;
	b[1] = (val32 >> 8) & 0xff;
	b[2] = (val32 >> 16) & 0xff;
	b[3] = (val32 >> 24) & 0xff;
};

inline static uint8_t be8_to_h(const void *src)
{
	uint8_t *b = src;
	return b[0];
};

inline static uint16_t be16_to_h(const void *src)
{
	uint8_t *b = src;
	return ((b[1] << 0) | (b[0] << 16));
};

inline static uint32_t be32_to_h(const void *src)
{
	uint8_t *b = src;
	return ((b[3] << 0) | (b[2] << 8) | (b[1] << 16) | (b[0] << 24));
};

inline static void h_to_be8(uint8_t val8, void *dest)
{
	uint8_t *b = dest;
	b[0] = val8;
};

inline static void h_to_be16(uint16_t val16, void *dest)
{
	uint8_t *b = dest;
	b[0] = (val16 >> 8) & 0xff;
	b[1] = (val16 >> 0) & 0xff;
};

inline static void h_to_be32(uint32_t val32, void *dest)
{
	uint8_t *b = dest;
	b[0] = (val32 >> 24) & 0xff;
	b[1] = (val32 >> 16) & 0xff;
	b[2] = (val32 >> 8) & 0xff;
	b[3] = (val32 >> 0) & 0xff;
};

#endif				/* __CBFSTOOL_ENDIAN_STREAM_H */
