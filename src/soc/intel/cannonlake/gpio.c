/*
 * This file is part of the coreboot project.
 *
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

#include <intelblocks/gpio.h>
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
 * The GPIO driver for Cannonlake on Windows/Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 *
 * This layout matches the Linux kernel pinctrl map for CNL-LP at:
 * linux/drivers/pinctrl/intel/pinctrl-cannonlake.c
 */
static const struct pad_group cnl_community0_groups[] = {
	INTEL_GPP_BASE(GPP_A0, GPP_A0, GPIO_RSVD_0, 0),		/* GPP_A */
	INTEL_GPP_BASE(GPP_A0, GPP_B0, GPIO_RSVD_2, 32),	/* GPP_B */
	INTEL_GPP_BASE(GPP_A0, GPP_G0, GPP_G7, 64),		/* GPP_G */
	INTEL_GPP(GPP_A0, GPIO_RSVD_3, GPIO_RSVD_11),		/* SPI */
};

static const struct pad_group cnl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_D0, GPP_D0, GPIO_RSVD_12, 96),	/* GPP_D */
	INTEL_GPP_BASE(GPP_D0, GPP_F0, GPP_F23, 128),		/* GPP_F */
	INTEL_GPP_BASE(GPP_D0, GPP_H0, GPP_H23, 160),		/* GPP_H */
	INTEL_GPP_BASE(GPP_D0, CNV_BTEN, vSD3_CD_B, 192),	/* VGPIO */
};

/* This community is not visible to the OS */
static const struct pad_group cnl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD11),				/* GPD */
};

/* This community is not visible to the OS */
static const struct pad_group cnl_community3_groups[] = {
	INTEL_GPP(HDA_BCLK, HDA_BCLK, SSP1_TXD),		/* AZA */
	INTEL_GPP(HDA_BCLK, GPIO_RSVD_28, GPIO_RSVD_38),	/* CPU */
};

static const struct pad_group cnl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_C0, GPP_C0, GPP_C23, 256),		/* GPP_C */
	INTEL_GPP_BASE(GPP_C0, GPP_E0, GPP_E23, 288),		/* GPP_E */
	INTEL_GPP(GPP_C0, GPIO_RSVD_13, GPIO_RSVD_21),		/* JTAG */
	INTEL_GPP(GPP_C0, GPIO_RSVD_22, GPIO_RSVD_27),		/* HVMOS */
};

static const struct pad_community cnl_communities[TOTAL_GPIO_COMM] = {
	/* GPP A, B, G, SPI */
	[COMM_0] = {
		.port = PID_GPIOCOM0,
		.first_pad = GPP_A0,
		.last_pad = GPIO_RSVD_11,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_ABG",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_com0,
		.num_reset_vals = ARRAY_SIZE(rst_map_com0),
		.groups = cnl_community0_groups,
		.num_groups = ARRAY_SIZE(cnl_community0_groups),
	},
	/* GPP D, F, H, VGPIO */
	[COMM_1] = {
		.port = PID_GPIOCOM1,
		.first_pad = GPP_D0,
		.last_pad = vSD3_CD_B,
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_DFH",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = cnl_community1_groups,
		.num_groups = ARRAY_SIZE(cnl_community1_groups),
	},
	/* GPD */
	[COMM_2] = {
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
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPD",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = cnl_community2_groups,
		.num_groups = ARRAY_SIZE(cnl_community2_groups),
	},
	/* AZA, CPU */
	[COMM_3] = {
		.port = PID_GPIOCOM3,
		.first_pad = HDA_BCLK,
		.last_pad = GPIO_RSVD_38,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GP_AC",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = cnl_community3_groups,
		.num_groups = ARRAY_SIZE(cnl_community3_groups),
	},
	/* GPP C, E, JTAG, HVMOS */
	[COMM_4] = {
		.port = PID_GPIOCOM4,
		.first_pad = GPP_C0,
		.last_pad = GPIO_RSVD_27,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_CEJ",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = cnl_community4_groups,
		.num_groups = ARRAY_SIZE(cnl_community4_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(cnl_communities);
	return cnl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_E, GPP_E },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_G, GPP_G },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPD, GPD },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
