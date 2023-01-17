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

static const struct pad_group apl_community_n_groups[] = {
	INTEL_GPP(N_OFFSET, N_OFFSET, GPIO_31),	/* NORTH 0 */
	INTEL_GPP(N_OFFSET, GPIO_32, TRST_B),	/* NORTH 1 */
	INTEL_GPP(N_OFFSET, TMS, SVID0_CLK),	/* NORTH 2 */
};

static const struct pad_group apl_community_w_groups[] = {
	INTEL_GPP(W_OFFSET, W_OFFSET, OSC_CLK_OUT_1),/* WEST 0 */
	INTEL_GPP(W_OFFSET, OSC_CLK_OUT_2, SUSPWRDNACK),/* WEST 1 */
};

static const struct pad_group apl_community_sw_groups[] = {
	INTEL_GPP(SW_OFFSET, SW_OFFSET, SMB_ALERTB),	/* SOUTHWEST 0 */
	INTEL_GPP(SW_OFFSET, SMB_CLK, LPC_FRAMEB),	/* SOUTHWEST 1 */
};

static const struct pad_group apl_community_nw_groups[] = {
	INTEL_GPP(NW_OFFSET, NW_OFFSET, PROCHOT_B),	/* NORTHWEST 0 */
	INTEL_GPP(NW_OFFSET, PMIC_I2C_SCL, GPIO_106),/* NORTHWEST 1 */
	INTEL_GPP(NW_OFFSET, GPIO_109, GPIO_123),	/* NORTHWEST 2 */
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
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_SW",
		.acpi_path = "\\_SB.GPO3",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = apl_community_sw_groups,
		.num_groups = ARRAY_SIZE(apl_community_sw_groups),
	}, {
		.port = PID_GPIO_W,
		.first_pad = W_OFFSET,
		.last_pad = SUSPWRDNACK,
		.num_gpi_regs = NUM_W_GPI_REGS,
		.gpi_status_offset = NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_W",
		.acpi_path = "\\_SB.GPO2",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = apl_community_w_groups,
		.num_groups = ARRAY_SIZE(apl_community_w_groups),
	}, {
		.port = PID_GPIO_NW,
		.first_pad = NW_OFFSET,
		.last_pad = GPIO_123,
		.num_gpi_regs = NUM_NW_GPI_REGS,
		.gpi_status_offset = NUM_W_GPI_REGS + NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_NW",
		.acpi_path = "\\_SB.GPO1",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = apl_community_nw_groups,
		.num_groups = ARRAY_SIZE(apl_community_nw_groups),
	}, {
		.port = PID_GPIO_N,
		.first_pad = N_OFFSET,
		.last_pad = SVID0_CLK,
		.num_gpi_regs = NUM_N_GPI_REGS,
		.gpi_status_offset = NUM_NW_GPI_REGS + NUM_W_GPI_REGS
			+ NUM_SW_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_GPE_N",
		.acpi_path = "\\_SB.GPO0",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = apl_community_n_groups,
		.num_groups = ARRAY_SIZE(apl_community_n_groups),
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
