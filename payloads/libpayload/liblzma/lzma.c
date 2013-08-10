/*
 * coreboot interface to memory-saving variant of LZMA decoder
 *
 * Copyright (C) 2006 Carl-Daniel Hailfinger
 * Released under the BSD license
 *
 * Parts of this file are based on C/7zip/Compress/LZMA_C/LzmaTest.c from the LZMA
 * SDK 4.42, which is written and distributed to public domain by Igor Pavlov.
 *
 */

#include <lzma.h>
#include <stdio.h>
#include <string.h>
#include "lzmadecode.c"

unsigned long ulzman(const unsigned char *src, unsigned long srcn,
		     unsigned char *dst, unsigned long dstn)
{
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	const int data_offset = LZMA_PROPERTIES_SIZE + 8;
	UInt32 outSize;
	SizeT inProcessed;
	SizeT outProcessed;
	int res;
	CLzmaDecoderState state;
	SizeT mallocneeds;
	unsigned char scratchpad[15980];

	memcpy(properties, src, LZMA_PROPERTIES_SIZE);
	memcpy(&outSize, src + LZMA_PROPERTIES_SIZE, sizeof(outSize));
	if (outSize > dstn)
		outSize = dstn;
	if (LzmaDecodeProperties(&state.Properties, properties,
				 LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
		printf("lzma: Incorrect stream properties.\n");
		return 0;
	}
	mallocneeds = (LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (mallocneeds > 15980) {
		printf("lzma: Decoder scratchpad too small!\n");
		return 0;
	}
	state.Probs = (CProb *)scratchpad;
	res = LzmaDecode(&state, src + data_offset, srcn - data_offset,
			 &inProcessed, dst, outSize, &outProcessed);
	if (res != 0) {
		printf("lzma: Decoding error = %d\n", res);
		return 0;
	}
	return outSize;
}

unsigned long ulzma(const unsigned char *src, unsigned char *dst)
{
	return ulzman(src, (unsigned long)(-1), dst, (unsigned long)(-1));
}
