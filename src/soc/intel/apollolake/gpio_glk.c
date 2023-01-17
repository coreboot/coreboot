/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};

static const struct pad_group glk_community_audio_groups[] = {
	INTEL_GPP(AUDIO_OFFSET, AUDIO_OFFSET, GPIO_175),	/* AUDIO 0 */
};

static const struct pad_group glk_community_nw_groups[] = {
	INTEL_GPP(NW_OFFSET, NW_OFFSET, GPIO_31),	/* NORTHWEST 0 */
	INTEL_GPP(NW_OFFSET, GPIO_32, GPIO_63),		/* NORTHWEST 1 */
	INTEL_GPP(NW_OFFSET, GPIO_64, GPIO_214),	/* NORTHWEST 2 */
};

static const struct pad_group glk_community_scc_groups[] = {
	INTEL_GPP(SCC_OFFSET, SCC_OFFSET, GPIO_206),	/* SCC 0 */
	INTEL_GPP(SCC_OFFSET, GPIO_207, GPIO_209),	/* SCC 1 */
};

static const struct pad_group glk_community_n_groups[] = {
	INTEL_GPP(N_OFFSET, N_OFFSET, GPIO_107),	/* NORTH 0 */
	INTEL_GPP(N_OFFSET, GPIO_108, GPIO_139),	/* NORTH 1 */
	INTEL_GPP(N_OFFSET, GPIO_140, GPIO_155),	/* NORTH 2 */
};

static const struct pad_community glk_gpio_communities[] = {
	{
		.port = PID_GPIO_NW,
		.first_pad = NW_OFFSET,
		.last_pad = GPIO_214,
		.num_gpi_regs = NUM_NW_GPI_REGS,
		.gpi_status_offset = 0,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_NORTHWEST",
		.acpi_path = "\\_SB.GPO0",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = glk_community_nw_groups,
		.num_groups = ARRAY_SIZE(glk_community_nw_groups),
	}, {
		.port = PID_GPIO_N,
		.first_pad = N_OFFSET,
		.last_pad = GPIO_155,
		.num_gpi_regs = NUM_N_GPI_REGS,
		.gpi_status_offset = NUM_NW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_NORTH",
		.acpi_path = "\\_SB.GPO1",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = glk_community_n_groups,
		.num_groups = ARRAY_SIZE(glk_community_n_groups),
	}, {
		.port = PID_GPIO_AUDIO,
		.first_pad = AUDIO_OFFSET,
		.last_pad = GPIO_175,
		.num_gpi_regs = NUM_AUDIO_GPI_REGS,
		.gpi_status_offset = NUM_NW_GPI_REGS + NUM_N_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_AUDIO",
		.acpi_path = "\\_SB.GPO2",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = glk_community_audio_groups,
		.num_groups = ARRAY_SIZE(glk_community_audio_groups),
	}, {
		.port = PID_GPIO_SCC,
		.first_pad = SCC_OFFSET,
		.last_pad = GPIO_209,
		.num_gpi_regs = NUM_SCC_GPI_REGS,
		.gpi_status_offset = NUM_NW_GPI_REGS + NUM_N_GPI_REGS +
			NUM_AUDIO_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_SCC",
		.acpi_path = "\\_SB.GPO3",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = glk_community_scc_groups,
		.num_groups = ARRAY_SIZE(glk_community_scc_groups),
	},
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(glk_gpio_communities);
	return glk_gpio_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPE_SCC_31_0, GPIO_GPE_SCC_31_0 },
		{ PMC_GPE_SCC_63_32, GPIO_GPE_SCC_63_32 },
		{ PMC_GPE_NW_31_0, GPIO_GPE_NW_31_0 },
		{ PMC_GPE_NW_63_32, GPIO_GPE_NW_63_32 },
		{ PMC_GPE_NW_95_64, GPIO_GPE_NW_95_64 },
		/*
		 * PMC_GPE_NW_127_96 maps to GPIO group 3, which is reserved and
		 * cannot be set in GPE0_DWx. Hence, it is skipped here.
		 */
		{ PMC_GPE_N_31_0, GPIO_GPE_N_31_0 },
		{ PMC_GPE_N_63_32, GPIO_GPE_N_63_32 },
		{ PMC_GPE_N_95_64, GPIO_GPE_N_95_64 },
		{ PMC_GPE_AUDIO_31_0, GPIO_GPE_AUDIO_31_0 },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
