/* SPDX-License-Identifier: BSD-4-Clause-UC */

#ifndef _BITS_H
#define _BITS_H

#define CONST_POPCOUNT2(x) ((((x) >> 0) & 1) + (((x) >> 1) & 1))
#define CONST_POPCOUNT4(x) (CONST_POPCOUNT2(x) + CONST_POPCOUNT2((x)>>2))
#define CONST_POPCOUNT8(x) (CONST_POPCOUNT4(x) + CONST_POPCOUNT4((x)>>4))
#define CONST_POPCOUNT16(x) (CONST_POPCOUNT8(x) + CONST_POPCOUNT8((x)>>8))
#define CONST_POPCOUNT32(x) (CONST_POPCOUNT16(x) + CONST_POPCOUNT16((x)>>16))
#define CONST_POPCOUNT64(x) (CONST_POPCOUNT32(x) + CONST_POPCOUNT32((x)>>32))
#define CONST_POPCOUNT(x) CONST_POPCOUNT64(x)

#define CONST_CTZ2(x) CONST_POPCOUNT2(((x) & -(x))-1)
#define CONST_CTZ4(x) CONST_POPCOUNT4(((x) & -(x))-1)
#define CONST_CTZ8(x) CONST_POPCOUNT8(((x) & -(x))-1)
#define CONST_CTZ16(x) CONST_POPCOUNT16(((x) & -(x))-1)
#define CONST_CTZ32(x) CONST_POPCOUNT32(((x) & -(x))-1)
#define CONST_CTZ64(x) CONST_POPCOUNT64(((x) & -(x))-1)
#define CONST_CTZ(x) CONST_CTZ64(x)

#define STR(x) XSTR(x)
#define XSTR(x) #x

#if __riscv_xlen == 64
#define SLL32 sllw
#define STORE sd
#define LOAD ld
#define LWU lwu
#define LOG_REGBYTES 3
#else
#define SLL32 sll
#define STORE sw
#define LOAD lw
#define LWU lw
#define LOG_REGBYTES 2
#endif

#define REGBYTES (1 << LOG_REGBYTES)

#endif
