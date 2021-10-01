/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef COMMONLIB_IOBUF_H
#define COMMONLIB_IOBUF_H

#include <stdint.h>
#include <sys/types.h>

/*
 * Two types are provided to aid in dealing with automatic buffer management
 * for code that deals with serializing and deserializing data structures.
 * The ibuf (input buffer) is read from while the obuf (output buffer) is
 * written to. Both keep track of capacity of the buffer as well as current
 * read or write cursor.
 *
 * When splicing or splitting ibufs of obufs the source object doesn't track
 * reads or writes through the newly created objects back to the source object.
 *
 * Any function returning an int encodes the return values as < 0 on error
 * and 0 on success. Any function returning a pointer returns NULL on error
 * and non-NULL on success.
 */

struct ibuf {
	const uint8_t *b;
	size_t n_read;
	size_t capacity;
};

struct obuf {
	uint8_t *b;
	size_t n_written;
	size_t capacity;
};

/* Helper functions. */
static inline size_t ibuf_capacity(const struct ibuf *ib)
{
	return ib->capacity;
}

static inline size_t ibuf_nr_read(const struct ibuf *ib)
{
	return ib->n_read;
}

static inline size_t ibuf_remaining(const struct ibuf *ib)
{
	return ibuf_capacity(ib) - ibuf_nr_read(ib);
}

static inline size_t obuf_capacity(const struct obuf *ob)
{
	return ob->capacity;
}

static inline size_t obuf_nr_written(const struct obuf *ob)
{
	return ob->n_written;
}

static inline size_t obuf_remaining(const struct obuf *ob)
{
	return obuf_capacity(ob) - obuf_nr_written(ob);
}

/* Initialize an ibuf with buffer and size of data. */
void ibuf_init(struct ibuf *ib, const void *b, size_t sz);

/* Create a new ibuf based on a subregion of the src ibuf. */
int ibuf_splice(const struct ibuf *src, struct ibuf *dst, size_t off,
		size_t sz);

/* Same as ibuf_splice(), but start from last read byte offset. */
int ibuf_splice_current(const struct ibuf *src, struct ibuf *dst, size_t sz);

/* Split an ibuf into 2 new ibufs at provided boundary. */
int ibuf_split(const struct ibuf *src, struct ibuf *a, struct ibuf *b,
		size_t boundary);

/* Out-of-band drain of ibuf by returning pointer to data of specified size. */
const void *ibuf_oob_drain(struct ibuf *ib, size_t sz);

/* Read arbitrary data from input buffer. */
int ibuf_read(struct ibuf *ib, void *data, size_t sz);

/* Read big endian fixed size values. */
int ibuf_read_be8(struct ibuf *ib, uint8_t *v);
int ibuf_read_be16(struct ibuf *ib, uint16_t *v);
int ibuf_read_be32(struct ibuf *ib, uint32_t *v);
int ibuf_read_be64(struct ibuf *ib, uint64_t *v);

/* Read little endian fixed size values. */
int ibuf_read_le8(struct ibuf *ib, uint8_t *v);
int ibuf_read_le16(struct ibuf *ib, uint16_t *v);
int ibuf_read_le32(struct ibuf *ib, uint32_t *v);
int ibuf_read_le64(struct ibuf *ib, uint64_t *v);

/* Read native endian fixed size values. */
int ibuf_read_n8(struct ibuf *ib, uint8_t *v);
int ibuf_read_n16(struct ibuf *ib, uint16_t *v);
int ibuf_read_n32(struct ibuf *ib, uint32_t *v);
int ibuf_read_n64(struct ibuf *ib, uint64_t *v);

/* Helper to create an ibuf from an obuf after an entity has written data. */
void ibuf_from_obuf(struct ibuf *ib, const struct obuf *ob);

/* Initialize an obuf with buffer and maximum capacity. */
void obuf_init(struct obuf *ob, void *b, size_t sz);

/* Provide the buffer and size of the written contents. */
const void *obuf_contents(const struct obuf *ob, size_t *sz);

/* Create a new obuf based on a subregion of the src obuf. */
int obuf_splice(const struct obuf *src, struct obuf *dst, size_t off,
		size_t sz);

/* Same as obuf_splice(), but start from last written byte offset. */
int obuf_splice_current(const struct obuf *src, struct obuf *dst, size_t sz);

/* Split an obuf into 2 new obufs at provided boundary. */
int obuf_split(const struct obuf *src, struct obuf *a, struct obuf *b,
		size_t boundary);

/* Fill the buffer out-of-band. The size is accounted for. */
void *obuf_oob_fill(struct obuf *ob, size_t sz);

/* Write arbitrary data to output buffer. */
int obuf_write(struct obuf *ob, const void *data, size_t sz);

/* Write big endian fixed size values. */
int obuf_write_be8(struct obuf *ob, uint8_t v);
int obuf_write_be16(struct obuf *ob, uint16_t v);
int obuf_write_be32(struct obuf *ob, uint32_t v);
int obuf_write_be64(struct obuf *ob, uint64_t v);

/* Write little endian fixed size values. */
int obuf_write_le8(struct obuf *ob, uint8_t v);
int obuf_write_le16(struct obuf *ob, uint16_t v);
int obuf_write_le32(struct obuf *ob, uint32_t v);
int obuf_write_le64(struct obuf *ob, uint64_t v);

/* Write native endian fixed size values. */
int obuf_write_n8(struct obuf *ob, uint8_t v);
int obuf_write_n16(struct obuf *ob, uint16_t v);
int obuf_write_n32(struct obuf *ob, uint32_t v);
int obuf_write_n64(struct obuf *ob, uint64_t v);

#endif
