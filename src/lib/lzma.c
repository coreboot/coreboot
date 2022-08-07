/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * coreboot interface to memory-saving variant of LZMA decoder
 *
 * Copyright (C) 2006 Carl-Daniel Hailfinger
 *
 * Parts of this file are based on C/7zip/Compress/LZMA_C/LzmaTest.c from the
 * LZMA SDK 4.42, which is written and distributed to public domain by Igor
 * Pavlov.
 *
 */

#include <console/console.h>
#include <string.h>
#include <lib.h>

#include "lzmadecode.h"

size_t ulzman(const void *src, size_t srcn, void *dst, size_t dstn)
{
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	const int data_offset = LZMA_PROPERTIES_SIZE + 8;
	UInt32 outSize;
	SizeT inProcessed;
	SizeT outProcessed;
	int res;
	CLzmaDecoderState state;
	SizeT mallocneeds;
	static unsigned char scratchpad[15980];
	const unsigned char *cp;

	if (srcn < data_offset) {
		printk(BIOS_WARNING, "lzma: Input too small.\n");
		return 0;
	}

	memcpy(properties, src, LZMA_PROPERTIES_SIZE);
	/* The outSize in LZMA stream is a 64bit integer stored in little-endian
	 * (ref: lzma.cc@LZMACompress: put_64). To prevent accessing by
	 * unaligned memory address and to load in correct endianness, read each
	 * byte and re-construct. */
	cp = src + LZMA_PROPERTIES_SIZE;
	outSize = cp[3] << 24 | cp[2] << 16 | cp[1] << 8 | cp[0];
	if (outSize > dstn)
		outSize = dstn;
	if (LzmaDecodeProperties(&state.Properties, properties,
				 LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
		printk(BIOS_WARNING, "lzma: Incorrect stream properties.\n");
		return 0;
	}
	mallocneeds = (LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (mallocneeds > 15980) {
		printk(BIOS_WARNING, "lzma: Decoder scratchpad too small!\n");
		return 0;
	}
	state.Probs = (CProb *)scratchpad;
	res = LzmaDecode(&state, src + data_offset, srcn - data_offset,
			 &inProcessed, dst, outSize, &outProcessed);
	if (res != 0) {
		printk(BIOS_WARNING, "lzma: Decoding error = %d\n", res);
		return 0;
	}
	return outProcessed;
}
