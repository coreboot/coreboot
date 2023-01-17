/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 0U << 30},
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30},
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30},
};

static const struct reset_mapping rst_map_com2[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30},
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30},
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30},
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 3U << 30},
};

static const struct pad_group skl_community_com0_groups[] = {
	INTEL_GPP(GPP_A0, GPP_A0, GPP_A23),	/* GPP A */
	INTEL_GPP(GPP_A0, GPP_B0, GPP_B23),	/* GPP B */
};

static const struct pad_group skl_community_com1_groups[] = {
	INTEL_GPP(GPP_C0, GPP_C0, GPP_C23),	/* GPP C */
#if CONFIG(SKYLAKE_SOC_PCH_H)
	INTEL_GPP(GPP_C0, GPP_D0, GPP_D23),	/* GPP D */
	INTEL_GPP(GPP_C0, GPP_E0, GPP_E12),	/* GPP E */
	INTEL_GPP(GPP_C0, GPP_F0, GPP_F23),	/* GPP F */
	INTEL_GPP(GPP_C0, GPP_G0, GPP_G23),	/* GPP G */
	INTEL_GPP(GPP_C0, GPP_H0, GPP_H23),	/* GPP H */
#else
	INTEL_GPP(GPP_C0, GPP_D0, GPP_D23),	/* GPP D */
	INTEL_GPP(GPP_C0, GPP_E0, GPP_E23),	/* GPP E */
#endif
};

static const struct pad_group skl_community_com3_groups[] = {
#if CONFIG(SKYLAKE_SOC_PCH_H)
	INTEL_GPP(GPP_I0, GPP_I0, GPP_I10),	/* GPP I */
#else
	INTEL_GPP(GPP_F0, GPP_F0, GPP_F23),	/* GPP F */
	INTEL_GPP(GPP_F0, GPP_G0, GPP_G7),	/* GPP G */
#endif
};

static const struct pad_group skl_community_com2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD11),		/* GPP GDP */
};

static const struct pad_community skl_gpio_communities[] = {
	{
		.port = PID_GPIOCOM0,
		.first_pad = GPP_A0,
		.last_pad = GPP_B23,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_COM0",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = skl_community_com0_groups,
		.num_groups = ARRAY_SIZE(skl_community_com0_groups),
	}, {
		.port = PID_GPIOCOM1,
		.first_pad = GPP_C0,
#if CONFIG(SKYLAKE_SOC_PCH_H)
		.last_pad = GPP_H23,
#else
		.last_pad = GPP_E23,
#endif
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_COM1",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = skl_community_com1_groups,
		.num_groups = ARRAY_SIZE(skl_community_com1_groups),
	}, {
		.port = PID_GPIOCOM3,
#if CONFIG(SKYLAKE_SOC_PCH_H)
		.first_pad = GPP_I0,
		.last_pad = GPP_I10,
#else
		.first_pad = GPP_F0,
		.last_pad = GPP_G7,
#endif
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_COM3",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = skl_community_com3_groups,
		.num_groups = ARRAY_SIZE(skl_community_com3_groups),
	}, {
		.port = PID_GPIOCOM2,
		.first_pad = GPD0,
		.last_pad = GPD11,
		.num_gpi_regs = NUM_GPIO_COM2_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPIO_COM2",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_com2,
		.num_reset_vals = ARRAY_SIZE(rst_map_com2),
		.groups = skl_community_com2_groups,
		.num_groups = ARRAY_SIZE(skl_community_com2_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(skl_gpio_communities);
	return skl_gpio_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ GPP_A, GPP_A},
		{ GPP_B, GPP_B},
		{ GPP_C, GPP_C},
		{ GPP_D, GPP_D},
		{ GPP_E, GPP_E},
		{ GPP_F, GPP_F},
		{ GPP_G, GPP_G},
#if CONFIG(SKYLAKE_SOC_PCH_H)
		{ GPP_H, GPP_H},
		{ GPP_I, GPP_I},
#endif
		{ GPD, GPD},
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}

uint32_t soc_gpio_pad_config_fixup(const struct pad_config *cfg,
					int dw_reg, uint32_t reg_val)
{
	if (CONFIG(SKYLAKE_SOC_PCH_H))
		return reg_val;
	/*
	 * For U/Y series, clear PAD_CFG1_TOL_1V8 in GPP_F4
	 * ~ GPP_F11.
	 */
	if (cfg->pad >= GPP_F4 && cfg->pad <= GPP_F11 && dw_reg == 1)
		reg_val = reg_val & ~(PAD_CFG1_TOL_1V8);
	return reg_val;

}
