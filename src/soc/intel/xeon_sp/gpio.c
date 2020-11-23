/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

/*
 * Reset mapping for Lewisburg PCH. See page 428, Intel Doc #336067-007US
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

static const struct pad_group lewisburg_community0_groups[] = {
	INTEL_GPP(GPP_A0, GPP_A0, GPP_A23), /* GPP A */
	INTEL_GPP(GPP_A0, GPP_B0, GPP_B23), /* GPP B */
	INTEL_GPP(GPP_A0, GPP_F0, GPP_F23), /* GPP F */
};

static const struct pad_group lewisburg_community1_groups[] = {
	INTEL_GPP(GPP_C0, GPP_C0, GPP_C23), /* GPP C */
	INTEL_GPP(GPP_C0, GPP_D0, GPP_D23), /* GPP D */
	INTEL_GPP(GPP_C0, GPP_E0, GPP_E12), /* GPP E */
};

static const struct pad_group lewisburg_community3_groups[] = {
	INTEL_GPP(GPP_I0, GPP_I0, GPP_I10), /* GPP I */
};

static const struct pad_group lewisburg_community4_groups[] = {
	INTEL_GPP(GPP_J0, GPP_J0, GPP_J23), /* GPP F */
	INTEL_GPP(GPP_J0, GPP_K0, GPP_K10), /* GPP K */
};

static const struct pad_group lewisburg_community5_groups[] = {
	INTEL_GPP(GPP_G0, GPP_G0, GPP_G23), /* GPP G */
	INTEL_GPP(GPP_G0, GPP_H0, GPP_H23), /* GPP H */
	INTEL_GPP(GPP_G0, GPP_L0, GPP_L19), /* GPP L */
};

static const struct pad_group lewisburg_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD11),       /* GPP GDP */
};

static const struct pad_community lewisburg_gpio_communities[] = {
	[COMM_0] = { /* GPIO Community 0: GPP A, B, F */
		.port               = PID_GPIOCOM0,
		.first_pad          = GPP_A0,
		.last_pad           = GPP_F23,
		.num_gpi_regs       = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0  = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0   = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM0",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = lewisburg_community0_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community0_groups),
	},
	[COMM_1] = { /* GPIO Community 1: GPP C, D, E */
		.port               = PID_GPIOCOM1,
		.first_pad          = GPP_C0,
		.last_pad           = GPP_E12,
		.num_gpi_regs       = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0  = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0   = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM1",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = lewisburg_community1_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community1_groups),
	},
	[COMM_3] = { /* GPIO Community 3: GPP I */
		.port               = PID_GPIOCOM3,
		.first_pad          = GPP_I0,
		.last_pad           = GPP_I10,
		.num_gpi_regs       = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base       = PAD_CFG_BASE,
		.host_own_reg_0     = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0  = GPI_INT_STS_0,
		.gpi_int_en_reg_0   = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0  = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0   = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0  = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0   = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name               = "GPIO_COM3",
		.acpi_path          = "\\_SB.PCI0.GPIO",
		.reset_map          = rst_map,
		.num_reset_vals     = ARRAY_SIZE(rst_map),
		.groups             = lewisburg_community3_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community3_groups),
	},
	[COMM_4] = { /* GPIO Community 4: GPP F, G */
		.port               = PID_GPIOCOM4,
		.first_pad          = GPP_J0,
		.last_pad           = GPP_K10,
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
		.groups             = lewisburg_community4_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community4_groups),
	},
	[COMM_5] = { /* GPIO Community 5: GPP G, H, L */
		.port               = PID_GPIOCOM5,
		.first_pad          = GPP_G0,
		.last_pad           = GPP_L19,
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
		.groups             = lewisburg_community5_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community5_groups),
	},
	[COMM_2] = { /* GPIO Community 2: GPD */
		.port               = PID_GPIOCOM2,
		.first_pad          = GPD0,
		.last_pad           = GPD11,
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
		.groups             = lewisburg_community2_groups,
		.num_groups         = ARRAY_SIZE(lewisburg_community2_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(lewisburg_gpio_communities);
	return lewisburg_gpio_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ GPP_A, GPP_A },
		{ GPP_B, GPP_B },
		{ GPP_F, GPP_F },
		{ GPP_C, GPP_C },
		{ GPP_D, GPP_D },
		{ GPP_E, GPP_E },
		{ GPP_I, GPP_I },
		{ GPP_J, GPP_J },
		{ GPP_K, GPP_K },
		{ GPD,   GPD   },
	};

	*num = ARRAY_SIZE(routes);
	return routes;
}
