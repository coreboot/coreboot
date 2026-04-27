/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_LPC_DEF_H
#define SOUTHBRIDGE_INTEL_LPC_DEF_H

/* LPC GPIO Base Address Register */
#define GPIOBASE		0x48

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define  CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define  CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define  MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define  KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define  GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define  GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define  FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define  LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define  COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define  COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[2:0] */

#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */

/* Only available on ICH10 and newer*/
#define LGMR			0x98 /* LPC Generic Memory Range */

#define BIOS_CNTL               0xdc
#define  BIOS_CNTL_BIOSWE       (1 << 0)
#define  BIOS_CNTL_BLE          (1 << 1)
#define  BIOS_CNTL_SMM_BWP      (1 << 5)

#endif /* SOUTHBRIDGE_INTEL_LPC_DEF_H */
