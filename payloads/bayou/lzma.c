/*

Coreboot interface to memory-saving variant of LZMA decoder

(C)opyright 2006 Carl-Daniel Hailfinger
Released under the GNU GPL v2 or later

Parts of this file are based on C/7zip/Compress/LZMA_C/LzmaTest.c from the LZMA
SDK 4.42, which is written and distributed to public domain by Igor Pavlov.

*/

#include <libpayload.h>
#include "lzmadecode.c"

unsigned long ulzma(u8 *src, u8 *dst)
{
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	UInt32 outSize;
	SizeT inProcessed;
	SizeT outProcessed;
	int res;
	CLzmaDecoderState state;
	SizeT mallocneeds;
	unsigned char scratchpad[15980];

	memcpy(properties, src, LZMA_PROPERTIES_SIZE);
	outSize = *(UInt32 *)(src + LZMA_PROPERTIES_SIZE);
	if (LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
		printf("Incorrect stream properties\n");
	}
	mallocneeds = (LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
	if (mallocneeds > 15980) {
		printf("Decoder scratchpad too small!\n");
	}
	state.Probs = (CProb *)scratchpad;
	res = LzmaDecode(&state, src + LZMA_PROPERTIES_SIZE + 8, (SizeT)0xffffffff, &inProcessed,
		dst, outSize, &outProcessed);
	if (res != 0) {
		printf("Decoding error = %d\n", res);
	}
	return outSize;
}
