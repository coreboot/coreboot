/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/option/cfr_frontend.h>
#include <intelblocks/pcie_rp.h>
#include <common/cfr.h>

void __weak cfr_card_reader_update(struct sm_object *new_obj)
{
	(void)new_obj;
}

void __weak cfr_touchscreen_update(struct sm_object *new_obj)
{
	(void)new_obj;
}

static const struct cfr_default_override starlabs_cfr_overrides[] = {
	CFR_OVERRIDE_ENUM("pciexp_aspm", ASPM_L0S_L1),
	CFR_OVERRIDE_END
};

void starlabs_cfr_register_overrides(void)
{
	if (!CONFIG(DRIVERS_OPTION_CFR))
		return;
	cfr_register_overrides(starlabs_cfr_overrides);
}
