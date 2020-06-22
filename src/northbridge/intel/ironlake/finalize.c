/* SPDX-License-Identifier: GPL-2.0-only */

#include "ironlake.h"

void intel_ironlake_finalize_smm(void)
{
	MCHBAR32_OR(0x5500, 1 << 0);	/* PAVP */
	MCHBAR32_OR(0x5f00, 1 << 31);	/* SA PM */
	MCHBAR32_OR(0x6020, 1 << 0);	/* UMA GFX */
	MCHBAR32_OR(0x63fc, 1 << 0);	/* VTDTRK */
	MCHBAR32_OR(0x6800, 1 << 31);
	MCHBAR32_OR(0x7000, 1 << 31);
	MCHBAR32_OR(0x77fc, 1 << 0);

	/* Memory Controller Lockdown */
	MCHBAR8(0x50fc) = 0x8f;

	/* Read+write the following */
	MCHBAR32(0x6030) = MCHBAR32(0x6030);
	MCHBAR32(0x6034) = MCHBAR32(0x6034);
	MCHBAR32(0x6008) = MCHBAR32(0x6008);
}
