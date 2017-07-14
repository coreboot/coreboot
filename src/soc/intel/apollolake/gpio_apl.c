/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};

static const struct pad_community apl_gpio_communities[] = {
	{
		.port = PID_GPIO_SW,
		.first_pad = SW_OFFSET,
		.last_pad = LPC_FRAMEB,
		.num_gpi_regs = NUM_SW_GPI_REGS,
		.gpi_status_offset = 0,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_SW",
		.acpi_path = "\\_SB.GPO3",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
	}, {
		.port = PID_GPIO_W,
		.first_pad = W_OFFSET,
		.last_pad = SUSPWRDNACK,
		.num_gpi_regs = NUM_W_GPI_REGS,
		.gpi_status_offset = NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_W",
		.acpi_path = "\\_SB.GPO2",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
	}, {
		.port = PID_GPIO_NW,
		.first_pad = NW_OFFSET,
		.last_pad = GPIO_123,
		.num_gpi_regs = NUM_NW_GPI_REGS,
		.gpi_status_offset = NUM_W_GPI_REGS + NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_NW",
		.acpi_path = "\\_SB.GPO1",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
	}, {
		.port = PID_GPIO_N,
		.first_pad = N_OFFSET,
		.last_pad = SVID0_CLK,
		.num_gpi_regs = NUM_N_GPI_REGS,
		.gpi_status_offset = NUM_NW_GPI_REGS + NUM_W_GPI_REGS
			+ NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_N",
		.acpi_path = "\\_SB.GPO0",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(apl_gpio_communities);
	return apl_gpio_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPE_SW_31_0, GPIO_GPE_SW_31_0 },
		{ PMC_GPE_SW_63_32, GPIO_GPE_SW_63_32 },
		{ PMC_GPE_NW_31_0, GPIO_GPE_NW_31_0 },
		{ PMC_GPE_NW_63_32, GPIO_GPE_NW_63_32 },
		{ PMC_GPE_NW_95_64, GPIO_GPE_NW_95_64 },
		{ PMC_GPE_N_31_0, GPIO_GPE_N_31_0 },
		{ PMC_GPE_N_63_32, GPIO_GPE_N_63_32 },
		{ PMC_GPE_W_31_0, GPIO_GPE_W_31_0 },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
