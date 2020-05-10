/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTTYPES_H
#define INTTYPES_H

#include <stdint.h>

/* int8_t and uint8_t */
#define PRId8   "d"
#define PRIi8   "i"
#define PRIu8   "u"
#define PRIo8   "o"
#define PRIx8   "x"
#define PRIX8   "X"

/* int16_t and uint16_t */
#define PRId16  "d"
#define PRIi16  "i"
#define PRIu16  "u"
#define PRIo16  "o"
#define PRIx16  "x"
#define PRIX16  "X"

/* int32_t and uint32_t */
#define PRId32  "d"
#define PRIi32  "i"
#define PRIu32  "u"
#define PRIo32  "o"
#define PRIx32  "x"
#define PRIX32  "X"

/* int64_t and uint64_t */
#define PRId64  "lld"
#define PRIi64  "lli"
#define PRIu64  "llu"
#define PRIo64  "llo"
#define PRIx64  "llx"
#define PRIX64  "llX"

/* intptr_t and uintptr_t */
#define PRIdPTR "ld"
#define PRIiPTR "li"
#define PRIuPTR "lu"
#define PRIoPTR "lo"
#define PRIxPTR "lx"
#define PRIXPTR "lX"

/* intmax_t and uintmax_t */
#define PRIdMAX "jd"
#define PRIiMAX "ji"
#define PRIuMAX "ju"
#define PRIoMAX "jo"
#define PRIxMAX "jx"
#define PRIXMAX "jX"

#endif /* INTTYPES_H */
