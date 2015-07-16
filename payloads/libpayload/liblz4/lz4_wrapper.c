/*
 * Copyright 2015 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <endian.h>
#include <libpayload.h>
#include <lz4.h>

/* LZ4 comes with its own supposedly portable memory access functions, but they
 * seem to be very inefficient in practice (at least on ARM64). Since libpayload
 * knows about endinaness and allows some basic assumptions (such as unaligned
 * access support), we can easily write the ones we need ourselves. */
static u16 LZ4_readLE16(const void *src) { return le16toh(*(u16 *)src); }
static void LZ4_copy4(void *dst, const void *src) { *(u32 *)dst = *(u32 *)src; }
static void LZ4_copy8(void *dst, const void *src) { *(u64 *)dst = *(u64 *)src; }

typedef  uint8_t BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef  int32_t S32;
typedef uint64_t U64;

#define FORCE_INLINE static inline __attribute__((always_inline))
#define likely(expr) __builtin_expect((expr) != 0, 1)
#define unlikely(expr) __builtin_expect((expr) != 0, 0)

/* Unaltered (except removing unrelated code) from github.com/Cyan4973/lz4. */
#include "lz4.c"	/* #include for inlining, do not link! */

#define LZ4F_MAGICNUMBER 0x184D2204

struct lz4_frame_header {
	u32 magic;
	union {
		u8 flags;
		struct {
			u8 reserved0		: 2;
			u8 has_content_checksum	: 1;
			u8 has_content_size	: 1;
			u8 has_block_checksum	: 1;
			u8 independent_blocks	: 1;
			u8 version		: 2;
		};
	};
	union {
		u8 block_descriptor;
		struct {
			u8 reserved1		: 4;
			u8 max_block_size	: 3;
			u8 reserved2		: 1;
		};
	};
	/* + u64 content_size iff has_content_size is set */
	/* + u8 header_checksum */
} __attribute__((packed));

struct lz4_block_header {
	union {
		u32 raw;
		struct {
			u32 size		: 31;
			u32 not_compressed	: 1;
		};
	};
	/* + size bytes of data */
	/* + u32 block_checksum iff has_block_checksum is set */
} __attribute__((packed));

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	const void *in = src;
	void *out = dst;
	int has_block_checksum;

	{ /* With in-place decompression the header may become invalid later. */
		const struct lz4_frame_header *h = in;

		if (srcn < sizeof(*h) + sizeof(u64) + sizeof(u8))
			return 0;	/* input overrun */

		/* We assume there's always only a single, standard frame. */
		if (le32toh(h->magic) != LZ4F_MAGICNUMBER || h->version != 1)
			return 0;	/* unknown format */
		if (h->reserved0 || h->reserved1 || h->reserved2)
			return 0;	/* reserved must be zero */
		if (!h->independent_blocks)
			return 0;	/* we don't support block dependency */
		has_block_checksum = h->has_block_checksum;

		in += sizeof(*h);
		if (h->has_content_size)
			in += sizeof(u64);
		in += sizeof(u8);
	}

	while (1) {
		struct lz4_block_header b = { .raw = le32toh(*(u32 *)in) };
		in += sizeof(struct lz4_block_header);

		if (in - src + b.size > srcn)
			return 0;		/* input overrun */

		if (!b.size)
			return out - dst;	/* decompression successful */

		if (b.not_compressed) {
			size_t size = MIN((u32)b.size, dst + dstn - out);
			memcpy(out, in, size);
			if (size < b.size)
				return 0;	/* output overrun */
			else
				out += size;
		} else {
			/* constant folding essential, do not touch params! */
			int ret = LZ4_decompress_generic(in, out, b.size,
					dst + dstn - out, endOnInputSize,
					full, 0, noDict, out, NULL, 0);
			if (ret < 0)
				return 0;	/* decompression error */
			else
				out += ret;
		}

		in += b.size;
		if (has_block_checksum)
			in += sizeof(u32);
	}
}

size_t ulz4f(const void *src, void *dst)
{
	/* LZ4 uses signed size parameters, so can't just use ((u32)-1) here. */
	return ulz4fn(src, 1*GiB, dst, 1*GiB);
}
