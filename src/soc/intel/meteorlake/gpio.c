/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

#define DEFAULT_VW_BASE		0x10

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_RESET(RSMRST), .chipset = 0U << 30 },
	{ .logical = PAD_RESET(DEEP), .chipset = 1U << 30 },
	{ .logical = PAD_RESET(PLTRST), .chipset = 2U << 30 },
};
static const struct reset_mapping rst_map_com3[] = {
	{ .logical = PAD_RESET(PWROK), .chipset = 0U << 30 },
	{ .logical = PAD_RESET(DEEP), .chipset = 1U << 30 },
	{ .logical = PAD_RESET(PLTRST), .chipset = 2U << 30 },
	{ .logical = PAD_RESET(RSMRST), .chipset = 3U << 30 },
};

/*
 * The GPIO pinctrl driver for Meteor Lake on Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 */
static const struct pad_group mtl_community0_groups[] = {
	INTEL_GPP(GPP_PECI, GPP_PECI, GPP_VIDALERT_B),			/* GPP_CPU */
	INTEL_GPP_BASE(GPP_PECI, GPP_V0, GPP_V23, 0),			/* GPP_V */
	INTEL_GPP_BASE(GPP_PECI, GPP_C0, GPP_C23, 32),			/* GPP_C */
};

static const struct vw_entries mtl_community0_vw[] = {
	{GPP_C0, GPP_C23},
};

static const struct pad_group mtl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_A0, GPP_A0, GPP_ESPI_CLK_LPBK, 64),		/* GPP_A */
	INTEL_GPP_BASE(GPP_A0, GPP_E0, GPP_THC0_GSPI_CLK_LPBK, 96),	/* GPP_E */
};

static const struct vw_entries mtl_community1_vw[] = {
	{GPP_A0, GPP_A20},
	{GPP_E0, GPP_E23},
};

static const struct pad_group mtl_community3_groups[] = {
	INTEL_GPP_BASE(GPP_H0, GPP_H0, GPP_LPI3C0_CLK_LPBK, 128),	/* GPP_H */
	INTEL_GPP_BASE(GPP_H0, GPP_F0, GPP_GSPI0A_CLK_LOOPBK, 160),	/* GPP_F */
	INTEL_GPP(GPP_H0, GPP_SPI0_IO_2, GPP_SPI0_CLK_LOOPBK),		/* GPP_SPI0 */
	INTEL_GPP(GPP_H0, GPP_VGPIO3_USB0, GPP_VGPIO3_THC3),		/* GPP_VGPIO3 */
};

static const struct vw_entries mtl_community3_vw[] = {
	{GPP_H0, GPP_H23},
	{GPP_F0, GPP_F23},
};

static const struct pad_group mtl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_S0, GPP_S0, GPP_S7, 192),			/* GPP_S */
	INTEL_GPP(GPP_S0, GPP_JTAG_MBPB0, GPP_JTAG_TRST_B),		/* GPP_JTAG */
};

static const struct pad_group mtl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_B0, GPP_B0, GPP_ACI3C0_CLK_LPBK, 224),	/* GPP_B */
	INTEL_GPP_BASE(GPP_B0, GPP_D0, GPP_BOOTHALT_B, 256),		/* GPP_D */
	INTEL_GPP(GPP_B0, GPP_VGPIO0, GPP_VGPIO47),			/* GPP_VGPIO */
};

static const struct vw_entries mtl_community5_vw[] = {
	{GPP_B0, GPP_B23},
	{GPP_D0, GPP_D23},
};

static const struct pad_community mtl_communities[] = {
	[COMM_0] = { /* GPP CPU, V, C */
		.port = PID_GPIOCOM0,
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
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_CPUVC",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = mtl_community0_groups,
		.num_groups = ARRAY_SIZE(mtl_community0_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = mtl_community0_vw,
		.num_vw_entries = ARRAY_SIZE(mtl_community0_vw),
	},
	[COMM_1] = { /* GPP A, E */
		.port = PID_GPIOCOM1,
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
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_AE",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = mtl_community1_groups,
		.num_groups = ARRAY_SIZE(mtl_community1_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = mtl_community1_vw,
		.num_vw_entries = ARRAY_SIZE(mtl_community1_vw),
	},
	[COMM_3] = { /* GPP H, F, SPI0, VGPIO3 */
		.port = PID_GPIOCOM3,
		.first_pad = GPIO_COM3_START,
		.last_pad = GPIO_COM3_END,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.pad_cfg_lock_offset = PAD_CFG_LOCK_OFFSET,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_gpe_sts_reg_0 = GPI_GPE_STS_0,
		.gpi_gpe_en_reg_0 = GPI_GPE_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_HFSPI0VG3",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_com3,
		.num_reset_vals = ARRAY_SIZE(rst_map_com3),
		.groups = mtl_community3_groups,
		.num_groups = ARRAY_SIZE(mtl_community3_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = mtl_community3_vw,
		.num_vw_entries = ARRAY_SIZE(mtl_community3_vw),
	},
	[COMM_4] = { /* GPP S, JTAG */
		.port = PID_GPIOCOM4,
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
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_SJTAG",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = mtl_community4_groups,
		.num_groups = ARRAY_SIZE(mtl_community4_groups),
	},
	[COMM_5] = { /* GPP B, D, VGPIO */
		.port = PID_GPIOCOM5,
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
		.gpi_nmi_sts_reg_0 = GPI_NMI_STS_0,
		.gpi_nmi_en_reg_0 = GPI_NMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_BDVG",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = mtl_community5_groups,
		.num_groups = ARRAY_SIZE(mtl_community5_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = mtl_community5_vw,
		.num_vw_entries = ARRAY_SIZE(mtl_community5_vw),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(mtl_communities);
	return mtl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPP_V, GPP_V },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_E, GPP_E },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_S, GPP_S },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_D, GPP_D },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
