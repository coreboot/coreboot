/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/i2c.h>
#include <soc/iomap.h>
#include <types.h>

static const struct soc_i3c_ctrlr_info i3c_ctrlr[I3C_CTRLR_COUNT] = {
	{ APU_I3C0_BASE, "I3C0" },
	{ APU_I3C1_BASE, "I3C1" },
	{ APU_I3C2_BASE, "I3C2" },
	{ APU_I3C3_BASE, "I3C3" }
};

const struct soc_i3c_ctrlr_info *soc_get_i3c_ctrlr_info(size_t *num_ctrlrs)
{
	*num_ctrlrs = ARRAY_SIZE(i3c_ctrlr);
	return i3c_ctrlr;
}
