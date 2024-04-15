/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#include <commonlib/bsd/compression.h>
#include <commonlib/bsd/helpers.h>
#include <commonlib/bsd/sysincludes.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>

/*
 * RISC-V and older ARM architectures do not mandate support for misaligned access.
 * Our le16toh and friends functions assume misaligned access support. Writing the access
 * like this causes the compiler to generate instructions using misaligned access (or not)
 * depending on the architecture. So there is no performance penalty for platforms supporting
 * misaligned access.
 */
static uint16_t LZ4_readLE16(const void *src)
{
	return *((const uint8_t *)src + 1) << 8
	      | *(const uint8_t *)src;
}

static uint32_t LZ4_readLE32(const void *src)
{
	return *((const uint8_t *)src + 3) << 24
	     | *((const uint8_t *)src + 2) << 16
	     | *((const uint8_t *)src + 1) << 8
	     |  *(const uint8_t *)src;
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
#elif defined(__riscv)
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

#define FORCE_INLINE static __always_inline
#define likely(expr) __builtin_expect((expr) != 0, 1)
#define unlikely(expr) __builtin_expect((expr) != 0, 0)

/* Unaltered (just removed unrelated code) from github.com/Cyan4973/lz4/dev. */
#include "lz4.c.inc"	/* #include for inlining, do not link! */

#define LZ4F_MAGICNUMBER 0x184D2204

/* Bit field endianness is implementation-defined. Use masks instead.
 * https://stackoverflow.com/a/6044223 */
#define RESERVED0		0x03
#define HAS_CONTENT_CHECKSUM	0x04
#define HAS_CONTENT_SIZE	0x08
#define HAS_BLOCK_CHECKSUM	0x10
#define INDEPENDENT_BLOCKS	0x20
#define VERSION			0xC0
#define VERSION_SHIFT		6

#define RESERVED1_2		0x8F
#define MAX_BLOCK_SIZE		0x70

struct lz4_frame_header {
	uint32_t magic;
	uint8_t flags;
	uint8_t block_descriptor;
	/* + uint64_t content_size iff has_content_size is set */
	/* + uint8_t header_checksum */
} __packed;

#define BH_SIZE			0x7FFFFFFF
#define NOT_COMPRESSED		0x80000000

struct lz4_block_header {
	uint32_t raw;
	/* + size bytes of data */
	/* + uint32_t block_checksum iff has_block_checksum is set */
} __packed;

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
		if (LZ4_readLE32(&h->magic) != LZ4F_MAGICNUMBER
		    || (h->flags & VERSION) != (1 << VERSION_SHIFT))
			return 0;	/* unknown format */
		if ((h->flags & RESERVED0) || (h->block_descriptor & RESERVED1_2))
			return 0;	/* reserved must be zero */
		if (!(h->flags & INDEPENDENT_BLOCKS))
			return 0;	/* we don't support block dependency */
		has_block_checksum = h->flags & HAS_BLOCK_CHECKSUM;

		in += sizeof(*h);
		if (h->flags & HAS_CONTENT_SIZE)
			in += sizeof(uint64_t);
		in += sizeof(uint8_t);
	}

	while (1) {
		if ((size_t)(in - src) + sizeof(struct lz4_block_header) > srcn)
			break;          /* input overrun */

		struct lz4_block_header b = {
			.raw = LZ4_readLE32((const uint32_t *)in)
		};
		in += sizeof(struct lz4_block_header);

		if ((size_t)(in - src) + (b.raw & BH_SIZE) > srcn)
			break;			/* input overrun */

		if (!(b.raw & BH_SIZE)) {
			out_size = out - dst;
			break;			/* decompression successful */
		}

		if (b.raw & NOT_COMPRESSED) {
			size_t size = MIN((uintptr_t)(b.raw & BH_SIZE), (uintptr_t)dst
				+ dstn - (uintptr_t)out);
			memcpy(out, in, size);
			if (size < (b.raw & BH_SIZE))
				break;		/* output overrun */
			out += size;
		} else {
			/* constant folding essential, do not touch params! */
			int ret = LZ4_decompress_generic(in, out, (b.raw & BH_SIZE),
					dst + dstn - out, endOnInputSize,
					full, 0, noDict, out, NULL, 0);
			if (ret < 0)
				break;		/* decompression error */
			out += ret;
		}

		in += (b.raw & BH_SIZE);
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
