/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include "i945.h"
#include "raminit.h"

int fixup_i945gm_errata(void)
{
	u32 reg32;

	/* Mobile Intel 945 Express only */
	reg32 = MCHBAR32(FSBPMC3);
	reg32 &= ~((1 << 13) | (1 << 29));
	MCHBAR32(FSBPMC3) = reg32;

	return 0;
}
