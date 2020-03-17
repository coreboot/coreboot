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

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 27
 */

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};
static const struct reset_mapping rst_map_com2[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 3U << 30 },
};

/*
 * This layout matches the Linux kernel pinctrl map for TGL-LP at:
 * linux/drivers/pinctrl/intel/pinctrl-tigerlake.c
 */
static const struct pad_group tgl_community0_groups[] = {
	INTEL_GPP(GPP_B0, GPP_B0, GPP_B25),				/* GPP_B */
	INTEL_GPP(GPP_B0, GPP_T0, GPP_T15),				/* GPP_T */
	INTEL_GPP(GPP_B0, GPP_A0, GPP_A24),				/* GPP_A */
};

static const struct pad_group tgl_community1_groups[] = {
	INTEL_GPP(GPP_S0, GPP_S0, GPP_S7),				/* GPP_S */
	INTEL_GPP(GPP_S0, GPP_H0, GPP_H23),				/* GPP_H */
	INTEL_GPP(GPP_S0, GPP_D0, GPP_GSPI2_CLK_LOOPBK),		/* GPP_D */
	INTEL_GPP(GPP_S0, GPP_U0, GPP_GSPI6_CLK_LOOPBK),		/* GPP_U */
	INTEL_GPP(GPP_S0, CNV_BTEN, vI2S2_RXD),				/* GPP_VGPIO */
};

/* This community is not visible to the OS */
static const struct pad_group tgl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD_DRAM_RESETB),				/* GPD */
};

static const struct pad_group tgl_community4_groups[] = {
	INTEL_GPP(GPP_C0, GPP_C0, GPP_C23),				/* GPP_C */
	INTEL_GPP(GPP_C0, GPP_F0, GPP_F_CLK_LOOPBK),			/* GPP_F */
	INTEL_GPP(GPP_C0, GPP_L_BKLTEN, GPP_MLK_RSTB),			/* GPP_HVCMOS */
	INTEL_GPP(GPP_C0, GPP_E0, GPP_E_CLK_LOOPBK),			/* GPP_E */
	INTEL_GPP(GPP_C0, GPP_JTAG_TDO, GPP_DBG_PMODE),			/* GPP_JTAG */
};

static const struct pad_group tgl_community5_groups[] = {
	INTEL_GPP(GPP_R0, GPP_R0, GPP_R7),				/* GPP_R */
	INTEL_GPP(GPP_R0, GPP_SPI_IO_2, GPP_CLK_LOOPBK),		/* GPP_SPI */
};

static const struct pad_community tgl_communities[] = {
	[COMM_0] = { /* GPP B, T, A */
		.port = PID_GPIOCOM0,
		.first_pad = GPP_B0,
		.last_pad = GPP_A24,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_BTA",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community0_groups,
		.num_groups = ARRAY_SIZE(tgl_community0_groups),
	},
	[COMM_1] = { /* GPP S, D, H, U, VGPIO */
		.port = PID_GPIOCOM1,
		.first_pad = GPP_S0,
		.last_pad = vI2S2_RXD,
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_SDHU",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community1_groups,
		.num_groups = ARRAY_SIZE(tgl_community1_groups),
	},
	[COMM_2] = { /* GPD */
		.port = PID_GPIOCOM2,
		.first_pad = GPD0,
		.last_pad = GPD_DRAM_RESETB,
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
		.reset_map = rst_map_com2,
		.num_reset_vals = ARRAY_SIZE(rst_map_com2),
		.groups = tgl_community2_groups,
		.num_groups = ARRAY_SIZE(tgl_community2_groups),
	},
	[COMM_4] = { /* GPP F, C, HVCOS, E, JTAG */
		.port = PID_GPIOCOM4,
		.first_pad = GPP_C0,
		.last_pad = GPP_DBG_PMODE,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_FCE",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community4_groups,
		.num_groups = ARRAY_SIZE(tgl_community4_groups),
	},
	[COMM_5] = { /* GPP R, SPI */
		.port = PID_GPIOCOM5,
		.first_pad = GPP_R0,
		.last_pad = GPP_CLK_LOOPBK,
		.num_gpi_regs = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_CPU_VBPIO",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community5_groups,
		.num_groups = ARRAY_SIZE(tgl_community5_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(tgl_communities);
	return tgl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_T, GPP_T },
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_R, GPP_R },
		{ PMC_GPD, GPD },
		{ PMC_GPP_S, GPP_S },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_U, GPP_U },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_E, GPP_E },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
