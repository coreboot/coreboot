/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/bsd/compiler.h>
#include <common/nvme_seq.h>
#include <intelblocks/gpio.h>

#if ENV_RAMSTAGE
const __weak struct pad_config *variant_nvme_power_sequence_pads(size_t *num)
{
	*num = 0;
	return NULL;
}

const __weak struct pad_config *variant_nvme_power_sequence_post_pads(size_t *num)
{
	*num = 0;
	return NULL;
}

__weak void variant_nvme_power_sequence_configure(void)
{
	size_t num;
	const struct pad_config *pads = variant_nvme_power_sequence_pads(&num);

	if (pads && num)
		gpio_configure_pads(pads, num);
}

__weak void variant_nvme_power_sequence_post_gpio_configure(void)
{
	size_t num;
	const struct pad_config *pads = variant_nvme_power_sequence_post_pads(&num);

	if (pads && num)
		gpio_configure_pads(pads, num);
}
#endif

/*
 * Star Labs NVMe/M.2 slot power sequence glue.
 *
 * Boards/variants provide the actual pad configs (PWREN, PERST#, CLKREQ#).
 *
 * This file only owns the ramstage ordering so the staged overrides happen
 * before PCIe enumeration/link training.
 */

static void starlabs_nvme_power_sequence_step2(void *unused)
{
	/* Stage 2: power on + enable CLKREQ#, keep PERST# asserted. */
	variant_nvme_power_sequence_configure();
}

static void starlabs_nvme_power_sequence_step3(void *unused)
{
	/* Stage 3: deassert PERST# after GPIO configuration, before device init. */
	variant_nvme_power_sequence_post_gpio_configure();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, starlabs_nvme_power_sequence_step2, NULL);
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, starlabs_nvme_power_sequence_step3, NULL);
