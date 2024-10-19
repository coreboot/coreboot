/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

/*
 * Reset mapping
 * 00 = RSMRST#
 * 01 = Host Deep Reset
 * 10 = PLTRST#
 * 11 = Reserved
 */
static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP,   .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};

static const struct pad_group emmitsburg_community0_groups[] = {
	INTEL_GPP(GPPC_A0, GPPC_A0, ESPI_CLK_LOOPBK),	/* GPPC A */
	INTEL_GPP(GPPC_A0, GPPC_B0, GPPC_B23),		/* GPPC B */
	INTEL_GPP(GPPC_A0, GPPC_S0, SPI_CLK_LOOPBK),	/* GPPC S */
};

static const struct pad_group emmitsburg_community1_groups[] = {
	INTEL_GPP(GPPC_C0, GPPC_C0, GPPC_C21),		/* GPPC C */
	INTEL_GPP(GPPC_C0, GPP_D0, GPP_D23),		/* GPP D */
};

static const struct pad_group emmitsburg_community3_groups[] = {
	INTEL_GPP(GPP_E0, GPP_E0, GPP_E23),		/* GPP E */
};

static const struct pad_group emmitsburg_community4_groups[] = {
	INTEL_GPP(GPPC_H0, GPPC_H0, GPPC_H19),		/* GPPC H */
	INTEL_GPP(GPPC_H0, GPP_J0, GPP_J17),		/* GPP J */
};

static const struct pad_group emmitsburg_community5_groups[] = {
	INTEL_GPP(GPP_I0, GPP_I0, GPP_I23),		/* GPP I */
	INTEL_GPP(GPP_I0, GPP_L0, GPP_L17),		/* GPP L */
	INTEL_GPP(GPP_I0, GPP_M0, GPP_M17),		/* GPP M */
	INTEL_GPP(GPP_I0, GPP_N0, GPP_N17),		/* GPP N */
};

static const struct pad_group emmitsburg_community2_groups[] = {
	INTEL_GPP(GPP_O0, GPP_O0, GPP_O16),       /* GPP O */
};

static const struct pad_community emmitsburg_gpio_communities[] = {
	[COMM_0] = { /* GPIO Community 0: GPPC A, B, S */
		.port               = PID_GPIOCOM0,
		.first_pad          = GPPC_A0,
		.last_pad           = SPI_CLK_LOOPBK,
		.num_gpi_regs       = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM0",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community0_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community0_groups),
	},
	[COMM_1] = { /* GPIO Community 1: GPPC C, GPP D */
		.port               = PID_GPIOCOM1,
		.first_pad          = GPPC_C0,
		.last_pad           = GPP_D23,
		.num_gpi_regs       = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM1",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community1_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community1_groups),
	},
	[COMM_3] = { /* GPIO Community 3: GPP E */
		.port               = PID_GPIOCOM3,
		.first_pad          = GPP_E0,
		.last_pad           = GPP_E23,
		.num_gpi_regs       = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM3",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community3_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community3_groups),
	},
	[COMM_4] = { /* GPIO Community 4: GPPC H, J */
		.port               = PID_GPIOCOM4,
		.first_pad          = GPPC_H0,
		.last_pad           = GPP_J17,
		.num_gpi_regs       = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM4",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community4_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community4_groups),
	},
	[COMM_5] = { /* GPIO Community 5: GPP I, L, M, N */
		.port               = PID_GPIOCOM5,
		.first_pad          = GPP_I0,
		.last_pad           = GPP_N17,
		.num_gpi_regs       = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM5",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community5_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community5_groups),
	},
	[COMM_2] = { /* GPIO Community 2: GPP O */
		.port               = PID_GPIOCOM2,
		.first_pad          = GPP_O0,
		.last_pad           = GPP_O16,
		.num_gpi_regs       = NUM_GPIO_COM2_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM2",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = emmitsburg_community2_groups,
		.num_groups         = ARRAY_SIZE(emmitsburg_community2_groups),
	},
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(emmitsburg_gpio_communities);
	return emmitsburg_gpio_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ GPPC_A, GPPC_A },
		{ GPPC_B, GPPC_B },
		{ GPPC_S, GPPC_S },
		{ GPPC_C, GPPC_C },
		{ GPP_D, GPP_D },
		{ GPP_E, GPP_E },
		{ GPPC_H, GPPC_H },
		{ GPPC_J, GPPC_J },
	};

	*num = ARRAY_SIZE(routes);
	return routes;
}
