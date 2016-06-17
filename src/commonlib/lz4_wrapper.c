/*
 * Copyright 2015-2016 Google Inc.
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

#include <commonlib/compression.h>
#include <commonlib/endian.h>
#include <commonlib/helpers.h>
#include <stdint.h>
#include <string.h>

/* LZ4 comes with its own supposedly portable memory access functions, but they
 * seem to be very inefficient in practice (at least on ARM64). Since coreboot
 * knows about endinaness and allows some basic assumptions (such as unaligned
 * access support), we can easily write the ones we need ourselves. */
static uint16_t LZ4_readLE16(const void *src)
{
	return read_le16(src);
}
static void LZ4_copy8(void *dst, const void *src)
{
/* ARM32 needs to be a special snowflake to prevent GCC from coalescing the
 * access into LDRD/STRD (which don't support unaligned accesses). */
#ifdef __arm__	/* ARMv < 6 doesn't support unaligned accesses at all. */
	#if defined(__COREBOOT_ARM_ARCH__) && __COREBOOT_ARM_ARCH__ < 6
		int i;
		for (i = 0; i < 8; i++)
			((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
	#else
		uint32_t x0, x1;
		__asm__ ("ldr %[x0], [%[src]]"
			: [x0]"=r"(x0)
			: [src]"r"(src), "m"(*(const uint32_t *)src));
		__asm__ ("ldr %[x1], [%[src], #4]"
			: [x1]"=r"(x1)
			: [src]"r"(src), "m"(*(const uint32_t *)(src + 4)));
		__asm__ ("str %[x0], [%[dst]]"
			: "=m"(*(uint32_t *)dst)
			: [x0]"r"(x0), [dst]"r"(dst));
		__asm__ ("str %[x1], [%[dst], #4]"
			: "=m"(*(uint32_t *)(dst + 4))
			: [x1]"r"(x1), [dst]"r"(dst));
	#endif
#elif defined(__riscv__)
	/* RISC-V implementations may trap on any unaligned access. */
	int i;
	for (i = 0; i < 8; i++)
		((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
#else
	*(uint64_t *)dst = *(const uint64_t *)src;
#endif
}

typedef  uint8_t BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef  int32_t S32;
typedef uint64_t U64;

#define FORCE_INLINE static inline __attribute__((always_inline))
#define likely(expr) __builtin_expect((expr) != 0, 1)
#define unlikely(expr) __builtin_expect((expr) != 0, 0)

/* Unaltered (just removed unrelated code) from github.com/Cyan4973/lz4/dev. */
#include "lz4.c.inc"	/* #include for inlining, do not link! */

#define LZ4F_MAGICNUMBER 0x184D2204

struct lz4_frame_header {
	uint32_t magic;
	union {
		uint8_t flags;
		struct {
			uint8_t reserved0		: 2;
			uint8_t has_content_checksum	: 1;
			uint8_t has_content_size	: 1;
			uint8_t has_block_checksum	: 1;
			uint8_t independent_blocks	: 1;
			uint8_t version			: 2;
		};
	};
	union {
		uint8_t block_descriptor;
		struct {
			uint8_t reserved1		: 4;
			uint8_t max_block_size		: 3;
			uint8_t reserved2		: 1;
		};
	};
	/* + uint64_t content_size iff has_content_size is set */
	/* + uint8_t header_checksum */
} __attribute__((packed));

struct lz4_block_header {
	union {
		uint32_t raw;
		struct {
			uint32_t size		: 31;
			uint32_t not_compressed	: 1;
		};
	};
	/* + size bytes of data */
	/* + uint32_t block_checksum iff has_block_checksum is set */
} __attribute__((packed));

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	const void *in = src;
	void *out = dst;
	size_t out_size = 0;
	int has_block_checksum;

	{ /* With in-place decompression the header may become invalid later. */
		const struct lz4_frame_header *h = in;

		if (srcn < sizeof(*h) + sizeof(uint64_t) + sizeof(uint8_t))
			return 0;	/* input overrun */

		/* We assume there's always only a single, standard frame. */
		if (read_le32(&h->magic) != LZ4F_MAGICNUMBER || h->version != 1)
			return 0;	/* unknown format */
		if (h->reserved0 || h->reserved1 || h->reserved2)
			return 0;	/* reserved must be zero */
		if (!h->independent_blocks)
			return 0;	/* we don't support block dependency */
		has_block_checksum = h->has_block_checksum;

		in += sizeof(*h);
		if (h->has_content_size)
			in += sizeof(uint64_t);
		in += sizeof(uint8_t);
	}

	while (1) {
		struct lz4_block_header b = { { .raw = read_le32(in) } };
		in += sizeof(struct lz4_block_header);

		if ((size_t)(in - src) + b.size > srcn)
			break;			/* input overrun */

		if (!b.size) {
			out_size = out - dst;
			break;			/* decompression successful */
		}

		if (b.not_compressed) {
			size_t size = MIN((uintptr_t)b.size, (uintptr_t)dst + dstn - (uintptr_t)out);
			memcpy(out, in, size);
			if (size < b.size)
				break;		/* output overrun */
			else
				out += size;
		} else {
			/* constant folding essential, do not touch params! */
			int ret = LZ4_decompress_generic(in, out, b.size,
					dst + dstn - out, endOnInputSize,
					full, 0, noDict, out, NULL, 0);
			if (ret < 0)
				break;		/* decompression error */
			else
				out += ret;
		}

		in += b.size;
		if (has_block_checksum)
			in += sizeof(uint32_t);
	}

	return out_size;
}

size_t ulz4f(const void *src, void *dst)
{
	/* LZ4 uses signed size parameters, so can't just use ((u32)-1) here. */
	return ulz4fn(src, 1*GiB, dst, 1*GiB);
}
