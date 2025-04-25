/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_RESET(PWROK), .chipset = 0U << 30 },
	{ .logical = PAD_RESET(DEEP), .chipset = 1U << 30 },
	{ .logical = PAD_RESET(PLTRST), .chipset = 2U << 30 },
	{ .logical = PAD_RESET(GLBRST), .chipset = 3U << 30 },
};

/*
 * The GPIO pinctrl driver for Panther Lake on Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 */
static const struct pad_group ptl_community0_groups[] = {
	INTEL_GPP_BASE(GPP_V00, GPP_V00, GPP_RST_B, 0),			/* GPP_V */
	INTEL_GPP_BASE(GPP_V00, GPP_C00, GPP_C23, 32),			/* GPP_C */
};

static const struct pad_group ptl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_F00, GPP_F00, GPP_GSPI0_CLK_LOOPBK, 0),	/* GPP_F */
	INTEL_GPP_BASE(GPP_F00, GPP_E00, GPP_THC0_GSPI_CLK_LPBK, 32),	/* GPP_E */
};

#if CONFIG(SOC_INTEL_WILDCATLAKE)
static const struct pad_group ptl_community3_groups[] = {
	INTEL_GPP_BASE(GPP_H00, GPP_H00, GPP_SHD3C_CLK_LPBK, 0),	/* GPP_H */
	INTEL_GPP_BASE(GPP_H00, GPP_A00, GPP_SPI0_CLK_LOOPBK, 32),	/* GPP_A */
	INTEL_GPP(GPP_H00, GPP_VGPIO3_USB0, GPP_VGPIO3_THC3),		/* GPP_VGPIO_3*/
};

static const struct pad_group ptl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_S00, GPP_S00, GPP_S07, 0),			/* GPP_S */
	INTEL_GPP(GPP_S00, GPP_EPD_ON, GPP_DDSP_HPDALV),		/* GPP_CPUJTAG */
};
#else
static const struct pad_group ptl_community3_groups[] = {
	INTEL_GPP(GPP_EPD_ON, GPP_EPD_ON, GPP_DDSP_HPDALV),		/* GPP_CPUJTAG */
	INTEL_GPP_BASE(GPP_EPD_ON, GPP_H00, GPP_ISHI3C1_CLK_LPBK, 32),	/* GPP_H */
	INTEL_GPP_BASE(GPP_EPD_ON, GPP_A00, GPP_SPI0_CLK_LOOPBK, 64),	/* GPP_A */
	INTEL_GPP(GPP_EPD_ON, GPP_VGPIO3_USB0, GPP_VGPIO3_THC3),	/* GPP_VGPIO_3*/
};

static const struct pad_group ptl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_S00, GPP_S00, GPP_S07, 0),			/* GPP_S */
};
#endif

static const struct pad_group ptl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_B00, GPP_B00, GPP_ISHI3C0_CLK_LPBK, 0),	/* GPP_B */
	INTEL_GPP_BASE(GPP_B00, GPP_D00, GPP_D25, 32),			/* GPP_D */
	INTEL_GPP(GPP_B00, GPP_VGPIO0, GPP_VGPIO47),			/* GPP_VGPIO */
};

static const struct pad_community ptl_communities[] = {
	[COMM_0] = { /* GPP V,C */
		.port = PID_GPIOCOM0,
		.first_pad = COM0_GRP_PAD_START,
		.last_pad = COM0_GRP_PAD_END,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_REG_0,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_V_C",
		.acpi_path = "\\_SB.PCI0.GPI0",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = ptl_community0_groups,
		.num_groups = ARRAY_SIZE(ptl_community0_groups),
	},
	[COMM_1] = { /* GPP: F, E */
		.port = PID_GPIOCOM1,
		.first_pad = COM1_GRP_PAD_START,
		.last_pad = COM1_GRP_PAD_END,
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_REG_0,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_F_E",
		.acpi_path = "\\_SB.PCI0.GPI1",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = ptl_community1_groups,
		.num_groups = ARRAY_SIZE(ptl_community1_groups),
	},
	[COMM_3] = { /* GPP: CPUJTAG, H, A, VGPIO3 for PTL
			GPP: H, A, VGPIO3 for WCL */
		.port = PID_GPIOCOM3,
		.first_pad = COM3_GRP_PAD_START,
		.last_pad = COM3_GRP_PAD_END,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_REG_0,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
#if CONFIG(SOC_INTEL_WILDCATLAKE)
		.name = "GPP_H_A_VGPIO3",
#else
		.name = "GPP_CPUJTAG_H_A_VGPIO3",
#endif
		.acpi_path = "\\_SB.PCI0.GPI3",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = ptl_community3_groups,
		.num_groups = ARRAY_SIZE(ptl_community3_groups),
	},
	[COMM_4] = { /* GPP: S for PTL
			GPP: S, CPUJTAG for WCL */
		.port = PID_GPIOCOM4,
		.first_pad = COM4_GRP_PAD_START,
		.last_pad = COM4_GRP_PAD_END,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_REG_0,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
#if CONFIG(SOC_INTEL_WILDCATLAKE)
		.name = "GPP_S_CPUJTAG",
#else
		.name = "GPP_S",
#endif
		.acpi_path = "\\_SB.PCI0.GPI4",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = ptl_community4_groups,
		.num_groups = ARRAY_SIZE(ptl_community4_groups),
	},
	[COMM_5] = { /* GPP: B, D, VGPIO */
		.port = PID_GPIOCOM5,
		.first_pad = COM5_GRP_PAD_START,
		.last_pad = COM5_GRP_PAD_END,
		.num_gpi_regs = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_REG_0,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_B_D_VGPIO",
		.acpi_path = "\\_SB.PCI0.GPI5",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = ptl_community5_groups,
		.num_groups = ARRAY_SIZE(ptl_community5_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(ptl_communities);
	return ptl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_V, GPP_V },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_E, GPP_E },
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_VGPIO, GPP_VGPIO },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_S, GPP_S },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
