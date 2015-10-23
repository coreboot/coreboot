#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"
#include "C/LzmaDec.h"
#include "C/LzmaEnc.h"

#define L (uint64_t)

static inline uint64_t get_64(const void *p)
{
	const unsigned char *data = (const unsigned char *)p;
	return (L data[0]) | (L data[1] << 8) | (L data[2] << 16) |
		(L data[3] << 24) | (L data [4] << 32) | (L data[5] << 40) |
		(L data[6] << 48) | (L data[7] << 56);
}

static void put_64(void *p, uint64_t value)
{
	unsigned char *data = (unsigned char *)p;
	data[0] = value & 0xff;
	data[1] = (value >> 8) & 0xff;
	data[2] = (value >> 16) & 0xff;
	data[3] = (value >> 24) & 0xff;
	data[4] = (value >> 32) & 0xff;
	data[5] = (value >> 40) & 0xff;
	data[6] = (value >> 48) & 0xff;
	data[7] = (value >> 56) & 0xff;
}

/* Memory Allocation API */

static void *SzAlloc(unused void *u, size_t size)
{
	return malloc(size);
}

static void SzFree(unused void *u, void *address)
{
	free(address);
}

static struct ISzAlloc LZMAalloc = { SzAlloc, SzFree };

/* Streaming API */

struct vector_t {
	char *p;
	size_t pos;
	size_t size;
};

static struct vector_t instream, outstream;

static SRes Read(unused void *u, void *buf, size_t *size)
{
	if ((instream.size - instream.pos) < *size)
		*size = instream.size - instream.pos;
	memcpy(buf, instream.p + instream.pos, *size);
	instream.pos += *size;
	return SZ_OK;
}

static size_t Write(unused void *u, const void *buf, size_t size)
{
	if(outstream.size - outstream.pos < size)
		size = outstream.size - outstream.pos;
	memcpy(outstream.p + outstream.pos, buf, size);
	outstream.pos += size;
	return size;
}

static struct ISeqInStream is = { Read };
static struct ISeqOutStream os = { Write };

/**
 * Compress a buffer with lzma
 * Don't copy the result back if it is too large.
 * @param in a pointer to the buffer
 * @param in_len the length in bytes
 * @param out a pointer to a buffer of at least size in_len
 * @param out_len a pointer to the compressed length of in
 */

int do_lzma_compress(char *in, int in_len, char *out, int *out_len)
{
	if (in_len == 0) {
		ERROR("LZMA: Input length is zero.\n");
		return -1;
	}

	struct CLzmaEncProps props;
	LzmaEncProps_Init(&props);
	props.dictSize = in_len;
	props.pb = 0; /* PosStateBits, default: 2, range: 0..4 */
	props.lp = 0; /* LiteralPosStateBits, default: 0, range: 0..4 */
	props.lc = 1; /* LiteralContextBits, default: 3, range: 0..8 */
	props.fb = 273; /* NumFastBytes */
	props.mc = 0; /* MatchFinderCycles, default: 0 */
	props.algo = 1; /* AlgorithmNo, apparently, 0 and 1 are valid values. 0 = fast mode */
	props.numThreads = 1;

	switch (props.algo) {
	case 0:	// quick: HC4
		props.btMode = 0;
		props.level = 1;
		break;
	case 1:	// full: BT4
	default:
		props.level = 9;
		props.btMode = 1;
		props.numHashBytes = 4;
		break;
	}

	CLzmaEncHandle p = LzmaEnc_Create(&LZMAalloc);

	int res = LzmaEnc_SetProps(p, &props);
	if (res != SZ_OK) {
		ERROR("LZMA: LzmaEnc_SetProps failed.\n");
		return -1;
	}

	unsigned char propsEncoded[LZMA_PROPS_SIZE + 8];
	size_t propsSize = sizeof propsEncoded;
	res = LzmaEnc_WriteProperties(p, propsEncoded, &propsSize);
	if (res != SZ_OK) {
		ERROR("LZMA: LzmaEnc_WriteProperties failed.\n");
		return -1;
	}

	instream.p = in;
	instream.pos = 0;
	instream.size = in_len;

	outstream.p = out;
	outstream.pos = 0;
	outstream.size = in_len;

	put_64(propsEncoded + LZMA_PROPS_SIZE, in_len);
	Write(&os, propsEncoded, LZMA_PROPS_SIZE+8);

	res = LzmaEnc_Encode(p, &os, &is, 0, &LZMAalloc, &LZMAalloc);
	LzmaEnc_Destroy(p, &LZMAalloc, &LZMAalloc);
	if (res != SZ_OK) {
		ERROR("LZMA: LzmaEnc_Encode failed %d.\n", res);
		return -1;
	}

	*out_len = outstream.pos;
	return 0;
}

int do_lzma_uncompress(char *dst, int dst_len, char *src, int src_len,
			size_t *actual_size)
{
	if (src_len <= LZMA_PROPS_SIZE + 8) {
		ERROR("LZMA: Input length is too small.\n");
		return -1;
	}

	uint64_t out_sizemax = get_64(&src[LZMA_PROPS_SIZE]);

	if (out_sizemax > (size_t) dst_len) {
		ERROR("Not copying %d bytes to %d-byte buffer!\n",
			(unsigned int)out_sizemax, dst_len);
		return -1;
	}

	enum ELzmaStatus status;

	size_t destlen = out_sizemax;
	size_t srclen = src_len - (LZMA_PROPS_SIZE + 8);

	int res = LzmaDecode((uint8_t *) dst, &destlen,
			     (uint8_t *) &src[LZMA_PROPS_SIZE + 8], &srclen,
			     (uint8_t *) &src[0], LZMA_PROPS_SIZE,
			     LZMA_FINISH_END,
			     &status,
			     &LZMAalloc);

	if (res != SZ_OK) {
		ERROR("Error while decompressing.\n");
		return -1;
	}

	if (actual_size != NULL)
		*actual_size = destlen;

	return 0;
}
