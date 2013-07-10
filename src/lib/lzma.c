/*
 * coreboot interface to memory-saving variant of LZMA decoder
 *
 * Copyright (C) 2006 Carl-Daniel Hailfinger
 * Released under the GNU GPL v2 or later
 *
 * Parts of this file are based on C/7zip/Compress/LZMA_C/LzmaTest.c from the LZMA
 * SDK 4.42, which is written and distributed to public domain by Igor Pavlov.
 *
 */

#include "lzmadecode.c"
#include <console/console.h>
#include <string.h>
#include <lib.h>

unsigned long ulzma(unsigned char * src, unsigned char * dst)
{
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	UInt32 outSize;
	SizeT inProcessed;
	SizeT outProcessed;
	int res;
	CLzmaDecoderState state;
	SizeT mallocneeds;
#if !defined(__PRE_RAM__)
	/* in ramstage, this can go in BSS */
	static
#endif
	/* in pre-ram, it must go on the stack */
	unsigned char scratchpad[15980];
	unsigned char *cp;

	memcpy(properties, src, LZMA_PROPERTIES_SIZE);
	/* The outSize in LZMA stream is a 64bit integer stored in little-endian
	 * (ref: lzma.cc@LZMACompress: put_64). To prevent accessing by
	 * unaligned memory address and to load in correct endianness, read each
	 * byte and re-construct. */
	cp = src + LZMA_PROPERTIES_SIZE;
	outSize = cp[3] << 24 | cp[2] << 16 | cp[1] << 8 | cp[0];
	if (LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
		printk(BIOS_WARNING, "lzma: Incorrect stream properties.\n");
		return 0;
	}
	mallocneeds = (LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (mallocneeds > 15980) {
		printk(BIOS_WARNING, "lzma: Decoder scratchpad too small!\n");
		return 0;
	}
	state.Probs = (CProb *)scratchpad;
	res = LzmaDecode(&state, src + LZMA_PROPERTIES_SIZE + 8, (SizeT)0xffffffff, &inProcessed,
		dst, outSize, &outProcessed);
	if (res != 0) {
		printk(BIOS_WARNING, "lzma: Decoding error = %d\n", res);
		return 0;
	}
	return outSize;
}
