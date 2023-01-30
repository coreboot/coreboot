/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>

/*
 * CPX-SP and SKX-SP use LBG PCH, while SPR-SP uses EBG PCH.
 * These definitions come from LBG datasheet (Intel Doc #336067-007US)
 * and Emmitsburg datasheet (Intel Doc #606161).
 */

#define HPTC_OFFSET			0x60
#define HPTC_ADDR_ENABLE_BIT		(1 << 7)
#define P2SB_SIZE			(16 * MiB)
#define P2SBC				0xe0
#define SBILOCK				(1 << 31)
#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP)
#define PCH_P2SB_EPMASK0		0x220
#else
#define PCH_P2SB_EPMASK0		0xb0
#endif
