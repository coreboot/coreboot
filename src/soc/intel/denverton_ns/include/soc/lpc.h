/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DENVERTON_NS_LPC_H_
#define _DENVERTON_NS_LPC_H_

/* PCI Configuration Space (D31:F0): LPC */
#define PCH_LPC_DEV PCI_DEV(0, LPC_DEV, LPC_FUNC)

#define SERIRQ_CNTL 0x64
#define LPC_IO_DEC 0x80  /* IO Decode Ranges Register */
#define FDD_LPC_SHIFT 12 /* LPC_IO_DEC[12] */
#define FDD_DEC_MASK 1
#define FDD_DEC_3F8 0   /* 3F0h - 3F5Fh, 3F7h (Primary) */
#define FDD_DEC_2F8 1   /* 370h - 375h, 377h (Secondary) */
#define LPT_LPC_SHIFT 8 /* LPC_IO_DEC[9:8] */
#define LPT_DEC_MASK 3
#define LPT_DEC_378 0    /* 378h - 37Fh and 778h - 77Fh */
#define LPT_DEC_278 1    /* 278h - 27Fh and 678h - 67Fh */
#define LPT_DEC_3BC 2    /* 3BCh - 3BEh and 7BCh - 7BEh */
#define COMB_LPC_SHIFT 4 /* LPC_IO_DEC[6:4] */
#define COMA_LPC_SHIFT 0 /* LPC_IO_DEC[2:0] */
#define COM_DEC_MASK 7
#define COM_DEC_3F8 0	 /* 3F8h - 3FFh (COM1) */
#define COM_DEC_2F8 1	 /* 2F8h - 2FFh (COM2) */
#define COM_DEC_220 2	 /* 220h - 227h */
#define COM_DEC_228 3	 /* 228h - 22Fh */
#define COM_DEC_238 4	 /* 238h - 23Fh */
#define COM_DEC_2E8 5	 /* 2E8h - 2EFh (COM4) */
#define COM_DEC_338 6	 /* 338h - 33Fh */
#define COM_DEC_3E8 7	 /* 3E8h - 3EFh (COM3) */
#define LPC_EN 0x82	   /* LPC IF Enables Register */
#define CNF2_LPC_EN (1 << 13) /* 0x4e/0x4f */
#define CNF1_LPC_EN (1 << 12) /* 0x2e/0x2f */
#define MC_LPC_EN (1 << 11)   /* 0x62/0x66 */
#define KBC_LPC_EN (1 << 10)  /* 0x60/0x64 */
#define GAMEH_LPC_EN (1 << 9) /* 0x208/0x20f */
#define GAMEL_LPC_EN (1 << 8) /* 0x200/0x207 */
#define FDD_LPC_EN (1 << 3)   /* LPC_IO_DEC[12] */
#define LPT_LPC_EN (1 << 2)   /* LPC_IO_DEC[9:8] */
#define COMB_LPC_EN (1 << 1)  /* LPC_IO_DEC[6:4] */
#define COMA_LPC_EN (1 << 0)  /* LPC_IO_DEC[2:0] */
#define LPC_GEN1_DEC 0x84     /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC 0x88     /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC 0x8c     /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC 0x90     /* LPC IF Generic Decode Range 4 */

#endif /* _DENVERTON_NS_LPC_H_ */
