/*  LzmaEnc.h -- LZMA Encoder
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __LZMA_ENC_H
#define __LZMA_ENC_H

#include "Types.h"

#define LZMA_PROPS_SIZE 5

struct CLzmaEncProps
{
  int level;       /*  0 <= level <= 9 */
  uint32_t dictSize; /* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version
                      (1 << 12) <= dictSize <= (1 << 30) for 64-bit version
                       default = (1 << 24) */
  int lc;          /* 0 <= lc <= 8, default = 3 */
  int lp;          /* 0 <= lp <= 4, default = 0 */
  int pb;          /* 0 <= pb <= 4, default = 2 */
  int algo;        /* 0 - fast, 1 - normal, default = 1 */
  int fb;          /* 5 <= fb <= 273, default = 32 */
  int btMode;      /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
  int numHashBytes; /* 2, 3 or 4, default = 4 */
  uint32_t mc;        /* 1 <= mc <= (1 << 30), default = 32 */
  unsigned writeEndMark;  /* 0 - do not write EOPM, 1 - write EOPM, default = 0 */
  int numThreads;  /* 1 or 2, default = 2 */
};

void LzmaEncProps_Init(struct CLzmaEncProps *p);
void LzmaEncProps_Normalize(struct CLzmaEncProps *p);
uint32_t LzmaEncProps_GetDictSize(const struct CLzmaEncProps *props2);


/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc_* functions can return the following exit codes:
Returns:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect paramater in props
  SZ_ERROR_WRITE  - Write callback error.
  SZ_ERROR_PROGRESS - some break from progress callback
  SZ_ERROR_THREAD - errors in multithreading functions (only for Mt version)
*/

typedef void * CLzmaEncHandle;

CLzmaEncHandle LzmaEnc_Create(struct ISzAlloc *alloc);
void LzmaEnc_Destroy(CLzmaEncHandle p, struct ISzAlloc *alloc, struct ISzAlloc *allocBig);
SRes LzmaEnc_SetProps(CLzmaEncHandle p, const struct CLzmaEncProps *props);
SRes LzmaEnc_WriteProperties(CLzmaEncHandle p, uint8_t *properties, size_t *size);
SRes LzmaEnc_Encode(CLzmaEncHandle p, struct ISeqOutStream *outStream, struct ISeqInStream *inStream,
    struct ICompressProgress *progress, struct ISzAlloc *alloc, struct ISzAlloc *allocBig);
SRes LzmaEnc_MemEncode(CLzmaEncHandle p, uint8_t *dest, size_t *destLen, const uint8_t *src, size_t srcLen,
    int writeEndMark, struct ICompressProgress *progress, struct ISzAlloc *alloc, struct ISzAlloc *allocBig);

/* ---------- One Call Interface ---------- */

/* LzmaEncode
Return code:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

SRes LzmaEncode(uint8_t *dest, size_t *destLen, const uint8_t *src, size_t srcLen,
    const struct CLzmaEncProps *props, uint8_t *propsEncoded, size_t *propsSize, int writeEndMark,
    struct ICompressProgress *progress, struct ISzAlloc *alloc, struct ISzAlloc *allocBig);

#endif
