/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 * Copyright (C) 2018 Online SAS
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
#include <soc/pcr.h>
#include <soc/pm.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
	/* (applicable only for GPD group) */
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 3U << 30 },
};

static const struct pad_group dnv_community_nc_groups[] = {
	INTEL_GPP(NORTH_ALL_GBE0_SDP0, NORTH_ALL_GBE0_SDP0, NORTH_ALL_PCIE_CLKREQ3_N),
	INTEL_GPP(NORTH_ALL_GBE0_SDP0, NORTH_ALL_PCIE_CLKREQ4_N, NORTH_ALL_MEMHOT_N),
};

static const struct pad_group dnv_community_sc_dfx_groups[] = {
	INTEL_GPP(SOUTH_DFX_DFX_PORT_CLK0, SOUTH_DFX_DFX_PORT_CLK0, SOUTH_DFX_DFX_PORT15),
};

static const struct pad_group dnv_community_sc0_groups[] = {
	INTEL_GPP(SOUTH_GROUP0_SMB3_CLTT_DATA, SOUTH_GROUP0_SMB3_CLTT_DATA, SOUTH_GROUP0_SATA0_LED_N),
	INTEL_GPP(SOUTH_GROUP0_SMB3_CLTT_DATA, SOUTH_GROUP0_SATA1_LED_N, SOUTH_GROUP0_DFX_SPARE4),
};

static const struct pad_group dnv_community_sc1_groups[] = {
	INTEL_GPP(SOUTH_GROUP1_SUSPWRDNACK, SOUTH_GROUP1_SUSPWRDNACK, SOUTH_GROUP1_EMMC_STROBE),
	INTEL_GPP(SOUTH_GROUP1_SUSPWRDNACK, SOUTH_GROUP1_EMMC_CLK, SOUTH_GROUP1_GPIO_3),
};

static const struct pad_community dnv_gpio_communities[] = {
	{
		.port = PID_GPIOCOM1,
		.first_pad = SOUTH_GROUP1_SUSPWRDNACK,
		.last_pad = SOUTH_GROUP1_GPIO_3,
		.num_gpi_regs = NUM_SC1_GPI_REGS,
		.gpi_status_offset = NUM_NC_GPI_REGS + NUM_SC_DFX_GPI_REGS +
				     NUM_SC0_GPI_REGS,
		.pad_cfg_base = R_PCH_PCR_GPIO_SC1_PADCFG_OFFSET,
		.host_own_reg_0 = R_PCH_PCR_GPIO_SC1_PAD_OWN,
		.gpi_smi_sts_reg_0 = R_PCH_PCR_GPIO_SC1_GPI_GPE_STS,
		.gpi_smi_en_reg_0 = R_PCH_PCR_GPIO_SC1_GPI_GPE_EN,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_SC1",
		.acpi_path = "\\_SB.GPO3",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = dnv_community_sc1_groups,
		.num_groups = ARRAY_SIZE(dnv_community_sc1_groups),
	}, {
		.port = PID_GPIOCOM1,
		.first_pad = SOUTH_GROUP0_SMB3_CLTT_DATA,
		.last_pad = SOUTH_GROUP0_DFX_SPARE4,
		.num_gpi_regs = NUM_SC0_GPI_REGS,
		.gpi_status_offset = NUM_NC_GPI_REGS + NUM_SC_DFX_GPI_REGS,
		.pad_cfg_base = R_PCH_PCR_GPIO_SC0_PADCFG_OFFSET,
		.host_own_reg_0 = R_PCH_PCR_GPIO_SC0_PAD_OWN,
		.gpi_smi_sts_reg_0 = R_PCH_PCR_GPIO_SC0_GPI_GPE_STS,
		.gpi_smi_en_reg_0 = R_PCH_PCR_GPIO_SC0_GPI_GPE_EN,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_SC0",
		.acpi_path = "\\_SB.GPO2",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = dnv_community_sc0_groups,
		.num_groups = ARRAY_SIZE(dnv_community_sc0_groups),
	}, {
		.port = PID_GPIOCOM1,
		.first_pad = SOUTH_DFX_DFX_PORT_CLK0,
		.last_pad = SOUTH_DFX_DFX_PORT15,
		.num_gpi_regs = NUM_SC_DFX_GPI_REGS,
		.gpi_status_offset = NUM_NC_GPI_REGS,
		.pad_cfg_base = R_PCH_PCR_GPIO_SC_DFX_PADCFG_OFFSET,
		.host_own_reg_0 = R_PCH_PCR_GPIO_SC_DFX_HOSTSW_OWN,
		.gpi_smi_sts_reg_0 = R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_STS,
		.gpi_smi_en_reg_0 = R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_EN,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_SC_DFX",
		.acpi_path = "\\_SB.GPO1",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = dnv_community_sc_dfx_groups,
		.num_groups = ARRAY_SIZE(dnv_community_sc_dfx_groups),
	}, {
		.port = PID_GPIOCOM0,
		.first_pad = NORTH_ALL_GBE0_SDP0,
		.last_pad = NORTH_ALL_MEMHOT_N,
		.num_gpi_regs = NUM_NC_GPI_REGS,
		.gpi_status_offset = 0,
		.pad_cfg_base = R_PCH_PCR_GPIO_NC_PADCFG_OFFSET,
		.host_own_reg_0 = R_PCH_PCR_GPIO_NC_PAD_OWN,
		.gpi_smi_sts_reg_0 = R_PCH_PCR_GPIO_NC_GPI_GPE_STS,
		.gpi_smi_en_reg_0 = R_PCH_PCR_GPIO_NC_GPI_GPE_EN,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_NC",
		.acpi_path = "\\_SB.GPO0",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = dnv_community_nc_groups,
		.num_groups = ARRAY_SIZE(dnv_community_nc_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(dnv_gpio_communities);
	return dnv_gpio_communities;
}
