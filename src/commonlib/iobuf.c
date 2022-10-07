/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/endian.h>
#include <commonlib/iobuf.h>
#include <string.h>
#include <stdint.h>

static int ibuf_check_size(const struct ibuf *ib, size_t sz)
{
	if (ibuf_remaining(ib) < sz)
		return -1;

	return 0;
}

void ibuf_init(struct ibuf *ib, const void *b, size_t sz)
{
	ib->b = b;
	ib->n_read = 0;
	ib->capacity = sz;
}

void ibuf_from_obuf(struct ibuf *ib, const struct obuf *ob)
{
	ibuf_init(ib, ob->b, ob->n_written);
}

int ibuf_splice(const struct ibuf *src, struct ibuf *dst, size_t off, size_t sz)
{
	size_t end = off + sz;
	size_t capacity = ibuf_capacity(src);
	size_t nr_read = ibuf_nr_read(src);

	if (end < off || end < sz || end > capacity)
		return -1;

	ibuf_init(dst, &src->b[off], sz);

	/* Handle previously read data in src. */
	if (off < nr_read)
		dst->n_read = nr_read - off;

	return 0;
}

int ibuf_splice_current(const struct ibuf *src, struct ibuf *dst, size_t sz)
{
	return ibuf_splice(src, dst, ibuf_nr_read(src), sz);
}

int ibuf_split(const struct ibuf *src, struct ibuf *a, struct ibuf *b,
		size_t boundary)
{
	if (ibuf_splice(src, a, 0, boundary))
		return -1;

	return ibuf_splice(src, b, boundary, ibuf_capacity(src) - boundary);
}

const void *ibuf_oob_drain(struct ibuf *ib, size_t sz)
{
	const void *b;

	if (ibuf_check_size(ib, sz))
		return NULL;

	b = &ib->b[ib->n_read];
	ib->n_read += sz;

	return b;
}

int ibuf_read(struct ibuf *ib, void *data, size_t sz)
{
	const void *b = ibuf_oob_drain(ib, sz);

	if (b == NULL)
		return -1;

	memcpy(data, b, sz);

	return 0;
}

int ibuf_read_be8(struct ibuf *ib, uint8_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_be8(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_be16(struct ibuf *ib, uint16_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_be16(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_be32(struct ibuf *ib, uint32_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_be32(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_be64(struct ibuf *ib, uint64_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_be64(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_le8(struct ibuf *ib, uint8_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_le8(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_le16(struct ibuf *ib, uint16_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_le16(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_le32(struct ibuf *ib, uint32_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_le32(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_le64(struct ibuf *ib, uint64_t *v)
{
	size_t sz = sizeof(*v);

	if (ibuf_check_size(ib, sz))
		return -1;

	*v = read_at_le64(ib->b, ib->n_read);
	ib->n_read += sz;

	return 0;
}

int ibuf_read_n8(struct ibuf *ib, uint8_t *v)
{
	return ibuf_read(ib, v, sizeof(*v));
}

int ibuf_read_n16(struct ibuf *ib, uint16_t *v)
{
	return ibuf_read(ib, v, sizeof(*v));
}

int ibuf_read_n32(struct ibuf *ib, uint32_t *v)
{
	return ibuf_read(ib, v, sizeof(*v));
}

int ibuf_read_n64(struct ibuf *ib, uint64_t *v)
{
	return ibuf_read(ib, v, sizeof(*v));
}

static int obuf_check_size(const struct obuf *ob, size_t sz)
{
	if (obuf_remaining(ob) < sz)
		return -1;

	return 0;
}

void obuf_init(struct obuf *ob, void *b, size_t sz)
{
	ob->b = b;
	ob->n_written = 0;
	ob->capacity = sz;
}

int obuf_splice(const struct obuf *src, struct obuf *dst, size_t off, size_t sz)
{
	size_t end = off + sz;
	size_t capacity = obuf_capacity(src);
	size_t nr_written = obuf_nr_written(src);

	if (end < off || end < sz || end > capacity)
		return -1;

	obuf_init(dst, &src->b[off], sz);

	/* Handle previously written data in src. */
	if (off < nr_written)
		dst->n_written = nr_written - off;

	return 0;
}

int obuf_splice_current(const struct obuf *src, struct obuf *dst, size_t sz)
{
	return obuf_splice(src, dst, obuf_nr_written(src), sz);
}

int obuf_split(const struct obuf *src, struct obuf *a, struct obuf *b,
		size_t boundary)
{
	if (obuf_splice(src, a, 0, boundary))
		return -1;

	return obuf_splice(src, b, boundary, obuf_capacity(src) - boundary);
}

void *obuf_oob_fill(struct obuf *ob, size_t sz)
{
	void *b;

	if (obuf_check_size(ob, sz))
		return NULL;

	b = &ob->b[ob->n_written];
	ob->n_written += sz;

	return b;
}

int obuf_write(struct obuf *ob, const void *data, size_t sz)
{
	void *b;

	b = obuf_oob_fill(ob, sz);
	if (b == NULL)
		return -1;

	memcpy(b, data, sz);

	return 0;
}

int obuf_write_be8(struct obuf *ob, uint8_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_be8(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_be16(struct obuf *ob, uint16_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_be16(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_be32(struct obuf *ob, uint32_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_be32(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_be64(struct obuf *ob, uint64_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_be64(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_le8(struct obuf *ob, uint8_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_le8(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_le16(struct obuf *ob, uint16_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_le16(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_le32(struct obuf *ob, uint32_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_le32(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_le64(struct obuf *ob, uint64_t v)
{
	size_t sz = sizeof(v);

	if (obuf_check_size(ob, sz))
		return -1;

	write_at_le64(ob->b, v, ob->n_written);
	ob->n_written += sz;

	return 0;
}

int obuf_write_n8(struct obuf *ob, uint8_t v)
{
	return obuf_write(ob, &v, sizeof(v));
}

int obuf_write_n16(struct obuf *ob, uint16_t v)
{
	return obuf_write(ob, &v, sizeof(v));
}

int obuf_write_n32(struct obuf *ob, uint32_t v)
{
	return obuf_write(ob, &v, sizeof(v));
}

int obuf_write_n64(struct obuf *ob, uint64_t v)
{
	return obuf_write(ob, &v, sizeof(v));
}

const void *obuf_contents(const struct obuf *ob, size_t *sz)
{
	*sz = obuf_nr_written(ob);
	return ob->b;
}
