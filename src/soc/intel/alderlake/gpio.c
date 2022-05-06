/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

#define DEFAULT_VW_BASE		0x10

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 630094, Chapter number: 27
 * Document number: 630603, Chapter number: 16
 */

static const struct reset_mapping rst_map_gpp[] = {
	{ .logical = PAD_RESET(PWROK), .chipset = 0U << 30 },
	{ .logical = PAD_RESET(DEEP), .chipset = 1U << 30 },
	{ .logical = PAD_RESET(PLTRST), .chipset = 2U << 30 },
};
static const struct reset_mapping rst_map_gpd[] = {
	{ .logical = PAD_RESET(PWROK), .chipset = 0U << 30 },
	{ .logical = PAD_RESET(DEEP), .chipset = 1U << 30 },
	{ .logical = PAD_RESET(PLTRST), .chipset = 2U << 30 },
	{ .logical = PAD_RESET(RSMRST), .chipset = 3U << 30 },
};

/*
 * The GPIO pinctrl driver for Alder Lake on Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 */
static const struct pad_group adl_community0_groups[] = {
	INTEL_GPP_BASE(GPP_B0, GPP_B0, GPP_B25, 0),			/* GPP_B */
	INTEL_GPP_BASE(GPP_B0, GPP_T0, GPP_T15, 32),			/* GPP_T */
	INTEL_GPP_BASE(GPP_B0, GPP_A0, GPP_ESPI_CLK_LOOPBK, 64),	/* GPP_A */
};

static const struct vw_entries adl_community0_vw[] = {
	{GPP_A0, GPP_A23},
	{GPP_B0, GPP_B23},
};

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
static const struct pad_group adl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_S0, GPP_S0, GPP_S7, 96),			/* GPP_S */
	INTEL_GPP_BASE(GPP_S0, GPP_I0, GPP_I19, 128),			/* GPP_I */
	INTEL_GPP_BASE(GPP_S0, GPP_H0, GPP_H23, 160),			/* GPP_H */
	INTEL_GPP_BASE(GPP_S0, GPP_D0, GPP_GSPI2_CLK_LOOPBK, 192),	/* GPP_D */
	INTEL_GPP(GPP_S0, GPP_VGPIO_0, GPP_VGPIO_THC1),			/* vGPIO */
};
#else
static const struct pad_group adl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_S0, GPP_S0, GPP_S7, 96),			/* GPP_S */
	INTEL_GPP_BASE(GPP_S0, GPP_H0, GPP_H23, 128),			/* GPP_H */
	INTEL_GPP_BASE(GPP_S0, GPP_D0, GPP_GSPI2_CLK_LOOPBK, 160),	/* GPP_D */
	INTEL_GPP(GPP_S0, GPP_CPU_RSVD_1, GPP_CPU_RSVD_24),		/* GPP_CPU_RSVD */
	INTEL_GPP(GPP_S0, GPP_VGPIO_0, GPP_VGPIO_37),			/* vGPIO */
};
#endif

static const struct vw_entries adl_community1_vw[] = {
	{GPP_D0, GPP_D19},
	{GPP_H0, GPP_H23},
};

/* This community is not visible to the OS */
static const struct pad_group adl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD_DRAM_RESETB),				/* GPD */
};

/* This community is not visible to the OS */
static const struct pad_group adl_community3_groups[] = {
	INTEL_GPP(GPP_CPU_RSVD_25, GPP_CPU_RSVD_25, GPP_vGPIO_PCIE_83),	/* vGPIO_PCIE */
};

static const struct pad_group adl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_C0, GPP_C0, GPP_C23, 256),			/* GPP_C */
	INTEL_GPP_BASE(GPP_C0, GPP_F0, GPP_F_CLK_LOOPBK, 288),		/* GPP_F */
	INTEL_GPP(GPP_C0, GPP_L_BKLTEN, GPP_MLK_RSTB),			/* GPP_HVMOS */
	INTEL_GPP_BASE(GPP_C0, GPP_E0, GPP_E_CLK_LOOPBK, 320),		/* GPP_E */
};

static const struct vw_entries adl_community4_vw[] = {
	{GPP_F0, GPP_F23},
	{GPP_C0, GPP_C23},
	{GPP_E0, GPP_E23},
};

static const struct pad_group adl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_R0, GPP_R0, GPP_R7, 352),			/* GPP_R */
	INTEL_GPP(GPP_R0, GPP_SPI0_IO_2, GPP_SPI0_CLK),			/* GPP_SPI0 */
};

static const struct pad_community adl_communities[] = {
	[COMM_0] = { /* GPP B, T, A */
		.port = PID_GPIOCOM0,
		.cpu_port = PID_CPU_GPIOCOM0,
		.first_pad = GPIO_COM0_START,
		.last_pad = GPIO_COM0_END,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_BTA",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community0_groups,
		.num_groups = ARRAY_SIZE(adl_community0_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community0_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community0_vw),
	},
	[COMM_1] = { /* GPP S, D, H for ADL-P/M
			GPP S, I, D, H for ADL-N */
		.port = PID_GPIOCOM1,
		.cpu_port = PID_CPU_GPIOCOM1,
		.first_pad = GPIO_COM1_START,
		.last_pad = GPIO_COM1_END,
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
		.name = "GPP_SIHD",
#else
		.name = "GPP_SDH",
#endif
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community1_groups,
		.num_groups = ARRAY_SIZE(adl_community1_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community1_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community1_vw),
	},
	[COMM_2] = { /* GPD */
		.port = PID_GPIOCOM2,
		.first_pad = GPIO_COM2_START,
		.last_pad = GPIO_COM2_END,
		.num_gpi_regs = NUM_GPIO_COM2_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPD",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpd,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpd),
		.groups = adl_community2_groups,
		.num_groups = ARRAY_SIZE(adl_community2_groups),
	},
	[COMM_3] = { /* vGPIO */
		.port = PID_GPIOCOM3,
		.cpu_port = PID_CPU_GPIOCOM3,
		.first_pad = GPIO_COM3_START,
		.last_pad = GPIO_COM3_END,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_VGPIO",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community3_groups,
		.num_groups = ARRAY_SIZE(adl_community3_groups),
	},
	[COMM_4] = { /* GPP F, C, HVMOS, E */
		.port = PID_GPIOCOM4,
		.cpu_port = PID_CPU_GPIOCOM4,
		.first_pad = GPIO_COM4_START,
		.last_pad = GPIO_COM4_END,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_FCE",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community4_groups,
		.num_groups = ARRAY_SIZE(adl_community4_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community4_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community4_vw),
	},
	[COMM_5] = { /* GPP R, SPI0 */
		.port = PID_GPIOCOM5,
		.cpu_port = PID_CPU_GPIOCOM5,
		.first_pad = GPIO_COM5_START,
		.last_pad = GPIO_COM5_END,
		.num_gpi_regs = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_RSPI0",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community5_groups,
		.num_groups = ARRAY_SIZE(adl_community5_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(adl_communities);
	return adl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_B,     GPP_B },
		{ PMC_GPP_T,     GPP_T },
		{ PMC_GPP_A,     GPP_A },
		{ PMC_GPP_S,     GPP_S },
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
		{ PMC_GPP_I,     GPP_I },
#endif
		{ PMC_GPP_H,     GPP_H },
		{ PMC_GPP_D,     GPP_D },
		{ PMC_GPD,       GPD },
		{ PMC_GPP_C,     GPP_C },
		{ PMC_GPP_F,     GPP_F },
		{ PMC_GPP_E,     GPP_E },
		{ PMC_GPP_R,     GPP_R },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
};
