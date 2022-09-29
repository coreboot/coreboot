/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>

void mtcmos_set_scpd_ext_buck_iso(const struct power_domain_data *pd)
{
	clrbits32(&mtk_spm->ext_buck_iso, pd->ext_buck_iso_bits);
}
