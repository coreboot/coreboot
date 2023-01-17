/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};

static const struct reset_mapping rst_map_com0[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 3U << 30 },
};

/*
 * The GPIO driver for Jasperlake on Windows/Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 *
 * This layout matches the Linux kernel pinctrl map for JSP at:
 * linux/drivers/pinctrl/intel/pinctrl-jasperlake.c
 */
static const struct pad_group jsl_community0_groups[] = {

	INTEL_GPP_BASE(GPP_F0, GPP_F0, GPP_F19, 320),		/* GPP_F */
	INTEL_GPP(GPP_F0, GPIO_SPI0_IO_2, GPIO_SPI0_CLK_LOOPBK),/* SPI0 */
	INTEL_GPP_BASE(GPP_F0, GPP_B0, GPIO_GSPI1_CLK_LOOPBK, 32),/* GPP_B */
	INTEL_GPP_BASE(GPP_F0, GPP_A0, GPIO_ESPI_CLK_LOOPBK, 64),/* GPP_A */
	INTEL_GPP_BASE(GPP_F0, GPP_S0, GPP_S7, 96),		/* GPP_S */
	INTEL_GPP_BASE(GPP_F0, GPP_R0, GPP_R7, 128),		/* GPP_R */
};

static const struct pad_group jsl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_H0, GPP_H0, GPP_H23, 160),		/* GPP_H */
	INTEL_GPP_BASE(GPP_H0, GPP_D0, GPIO_SPI1_CLK_LOOPBK, 192),/* GPP_D */
	INTEL_GPP_BASE(GPP_H0, VGPIO_0, VGPIO_39, 224),		/* VGPIO */
	INTEL_GPP_BASE(GPP_H0, GPP_C0, GPP_C23, 256),		/* GPP_C */
};

/* This community is not visible to the OS */
static const struct pad_group jsl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPIO_DRAM_RESETB),		/* GPD */
};

static const struct pad_group jsl_community4_groups[] = {
	INTEL_GPP(GPIO_L_BKLTEN, GPIO_L_BKLTEN, GPIO_MLK_RSTB),	/* Reserved */
	INTEL_GPP_BASE(GPIO_L_BKLTEN, GPP_E0, GPP_E23, 288),	/* GPP_E */
};

static const struct pad_group jsl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_G0, GPP_G0, GPP_G7, 0),		/* GPP_G */
};

static const struct pad_community jsl_communities[TOTAL_GPIO_COMM] = {
	/* GPP F, B, A, S, R */
	[COMM_0] = {
		.port = PID_GPIOCOM0,
		.first_pad = GPIO_COM0_START,
		.last_pad = GPIO_COM0_END,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_FBASR",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_com0,
		.num_reset_vals = ARRAY_SIZE(rst_map_com0),
		.groups = jsl_community0_groups,
		.num_groups = ARRAY_SIZE(jsl_community0_groups),
	},
	/* GPP H, D, VGPIO, C */
	[COMM_1] = {
		.port = PID_GPIOCOM1,
		.first_pad = GPIO_COM1_START,
		.last_pad = GPIO_COM1_END,
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
		.name = "GPP_HDC",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = jsl_community1_groups,
		.num_groups = ARRAY_SIZE(jsl_community1_groups),
	},
	/* GPD */
	[COMM_2] = {
		.port = PID_GPIOCOM2,
		.first_pad = GPIO_COM2_START,
		.last_pad = GPIO_COM2_END,
		.num_gpi_regs = NUM_GPIO_COM2_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPD",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = jsl_community2_groups,
		.num_groups = ARRAY_SIZE(jsl_community2_groups),
	},
	/* GPP E */
	[COMM_4] = {
		.port = PID_GPIOCOM4,
		.first_pad = GPIO_COM4_START,
		.last_pad = GPIO_COM4_END,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_E",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = jsl_community4_groups,
		.num_groups = ARRAY_SIZE(jsl_community4_groups),
	},
	/* GPP G */
	[COMM_5] = {
		.port = PID_GPIOCOM5,
		.first_pad = GPIO_COM5_START,
		.last_pad = GPIO_COM5_END,
		.num_gpi_regs = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_G",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = jsl_community5_groups,
		.num_groups = ARRAY_SIZE(jsl_community5_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(jsl_communities);
	return jsl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_G, GPP_G },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_R, GPP_R },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_S, GPP_S },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPD,   GPP_GPD },
		{ PMC_GPP_E, GPP_E }
	};

	*num = ARRAY_SIZE(routes);
	return routes;
}
