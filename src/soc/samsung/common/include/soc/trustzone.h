/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_TRUSTZONE_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_TRUSTZONE_H

#include <stdint.h>

/* Distance between each Trust Zone PC register set */
#define TZPC_BASE_OFFSET		0x10000
/* TZPC : Register Offsets */
#define TZPC0_BASE		0x10100000
#define TZPC1_BASE		0x10110000
#define TZPC2_BASE		0x10120000
#define TZPC3_BASE		0x10130000
#define TZPC4_BASE		0x10140000
#define TZPC5_BASE		0x10150000
#define TZPC6_BASE		0x10160000
#define TZPC7_BASE		0x10170000
#define TZPC8_BASE		0x10180000
#define TZPC9_BASE		0x10190000
#define TZPC10_BASE		0x100E0000
#define TZPC11_BASE		0x100F0000

/*
 * TZPC Register Value :
 * R0SIZE: 0x0 : Size of secured ram
 */
#define R0SIZE			0x0

/*
 * TZPC Decode Protection Register Value :
 * DECPROTXSET: 0xFF : Set Decode region to non-secure
 */
#define DECPROTXSET		0xFF

struct exynos_tzpc {
	u32 r0size;
	u8 res1[0x7FC];
	u32 decprot0stat;
	u32 decprot0set;
	u32 decprot0clr;
	u32 decprot1stat;
	u32 decprot1set;
	u32 decprot1clr;
	u32 decprot2stat;
	u32 decprot2set;
	u32 decprot2clr;
	u32 decprot3stat;
	u32 decprot3set;
	u32 decprot3clr;
	u8 res2[0x7B0];
	u32 periphid0;
	u32 periphid1;
	u32 periphid2;
	u32 periphid3;
	u32 pcellid0;
	u32 pcellid1;
	u32 pcellid2;
	u32 pcellid3;
};
check_member(exynos_tzpc, pcellid3, 0xffc);

void trustzone_init(void);

#endif	/* SOC_SAMSUNG_COMMON_INCLUDE_SOC_TRUSTZONE_H */
