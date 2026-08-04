/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_LPC_DEF_H
#define SOUTHBRIDGE_INTEL_LPC_DEF_H

/* LPC GPIO Base Address Register */
#define GPIOBASE		0x48

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6a
#define PIRQH_ROUT		0x6b

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
/* LPC_GENx_DEC register fields */
#define LPC_LGIR_AMASK_MASK	(0xfc << 16)
#define LPC_LGIR_ADDR_MASK	0xfffc
#define LPC_LGIR_EN		(1 << 0)
#define LPC_LGIR_MAX_WINDOW_SIZE	256

/*
 * Encode a LPC_GENx_DEC value from base address and window size.
 * Size must be a power of two in [4, 256]; base must be aligned to size so
 * AMASK yields a contiguous window [base, base + size).
 */
#define LPC_IO_INVALID(base, size) ( \
	(__builtin_constant_p(base) && __builtin_constant_p(size)) && ( \
		((size) < 4) || ((size) > LPC_LGIR_MAX_WINDOW_SIZE) || \
		((size) & ((size) - 1)) || ((base) & ((size) - 1))))
#define LPC_IO_CHECK(base, size) \
	(sizeof(struct { \
		_Static_assert(!LPC_IO_INVALID(base, size), \
			"LPC_IO: size must be power of 2 in [4,256], base aligned to size"); \
		int dummy; \
	}) * 0)

#define LPC_IO(base, size) ( \
	LPC_IO_CHECK(base, size) + ( \
	((LPC_LGIR_AMASK_MASK) & (((size) - 1) << 16)) | \
	((base) & LPC_LGIR_ADDR_MASK) | LPC_LGIR_EN))


/* Only available on ICH10 and newer*/
#define LGMR			0x98 /* LPC Generic Memory Range */

#define BIOS_CNTL               0xdc
#define  BIOS_CNTL_BIOSWE       (1 << 0)
#define  BIOS_CNTL_BLE          (1 << 1)
#define  BIOS_CNTL_SMM_BWP      (1 << 5)

#endif /* SOUTHBRIDGE_INTEL_LPC_DEF_H */
