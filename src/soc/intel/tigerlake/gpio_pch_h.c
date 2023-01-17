/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 619207
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
 * The GPIO pinctrl driver for Tiger Lake on Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 *
 * This layout matches the Linux kernel pinctrl map for TGL-H at:
 * linux/drivers/pinctrl/intel/pinctrl-tigerlake.c
 */
static const struct pad_group tgl_community0_groups[] = {
	INTEL_GPP_BASE(GPIO_COM0_START, GPP_SPI0_IO_2, GPP_ESPI_CLK_LOOPBK, 0),	/* GPP_A */
	INTEL_GPP_BASE(GPIO_COM0_START, GPP_R0, GPP_R19, 32),			/* GPP_R */
	INTEL_GPP_BASE(GPIO_COM0_START, GPP_B0, GPP_GSPI1_CLK_LOOPBK, 64),	/* GPP_B */
	INTEL_GPP_BASE(GPIO_COM0_START, ESPI_USB_OCB_0, USB_CPU_OCB_3, 96),	/* vGPIO_0 */
};

static const struct pad_group tgl_community1_groups[] = {
	INTEL_GPP_BASE(GPIO_COM1_START, GPP_D0, GPP_GSPI3_CLK_LOOPBK, 128),	/* GPP_D */
	INTEL_GPP_BASE(GPIO_COM1_START, GPP_C0, GPP_C23, 160),			/* GPP_C */
	INTEL_GPP_BASE(GPIO_COM1_START, GPP_S0, GPP_S7, 192),			/* GPP_S */
	INTEL_GPP_BASE(GPIO_COM1_START, GPP_G0, GPP_GSPI2_CLK_LOOPBK, 224),	/* GPP_G */
	INTEL_GPP_BASE(GPIO_COM1_START, CNV_BTEN, vI2S2_RXD, 256),		/* vGPIO */
};

/* This community is not visible to the OS */
static const struct pad_group tgl_community2_groups[] = {
	INTEL_GPP(GPIO_COM2_START, GPD0, GPD12),				/* GPD */
};

static const struct pad_group tgl_community3_groups[] = {
	INTEL_GPP_BASE(GPIO_COM3_START, GPP_E0, GPP_E12, 288),			/* GPP_E */
	INTEL_GPP_BASE(GPIO_COM3_START, GPP_F0, GPP_F23, 320),			/* GPP_F */
};

static const struct pad_group tgl_community4_groups[] = {
	INTEL_GPP_BASE(GPIO_COM4_START, GPP_H0, GPP_H23, 352),			/* GPP_H */
	INTEL_GPP_BASE(GPIO_COM4_START, GPP_J0, GPP_J9, 384),			/* GPP_J */
	INTEL_GPP_BASE(GPIO_COM4_START, GPP_K0, GPP_MLK_RSTB, 416),		/* GPP_K */
};

static const struct pad_group tgl_community5_groups[] = {
	INTEL_GPP_BASE(GPIO_COM5_START, GPP_I0, GPP_I14, 448),			/* GPP_I */
	INTEL_GPP(GPIO_COM5_START, GPP_JTAG_TDO, GPP_JTAG_CPU_TRSTB),		/* JTAG */
};

static const struct pad_community tgl_communities[] = {
	[COMM_0] = { /* GPP A, R, B, vGPIO_0 */
		.port = PID_GPIOCOM0,
		.first_pad = GPIO_COM0_START,
		.last_pad = GPIO_COM0_END,
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
		.name = "GPP_ARB",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community0_groups,
		.num_groups = ARRAY_SIZE(tgl_community0_groups),
	},
	[COMM_1] = { /* GPP D, C, S, G, vGPIO */
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
		.name = "GPP_DCSG",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community1_groups,
		.num_groups = ARRAY_SIZE(tgl_community1_groups),
	},
	[COMM_2] = { /* GPD */
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
		.reset_map = rst_map_com2,
		.num_reset_vals = ARRAY_SIZE(rst_map_com2),
		.groups = tgl_community2_groups,
		.num_groups = ARRAY_SIZE(tgl_community2_groups),
	},
	[COMM_3] = { /* GPP E, F */
		.port = PID_GPIOCOM3,
		.first_pad = GPIO_COM3_START,
		.last_pad = GPIO_COM3_END,
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
		.name = "GPP_EF",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community3_groups,
		.num_groups = ARRAY_SIZE(tgl_community3_groups),
	},
	[COMM_4] = { /* GPP H, J, K */
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
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_HJK",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community4_groups,
		.num_groups = ARRAY_SIZE(tgl_community4_groups),
	},
	[COMM_5] = { /* GPP I, JTAG */
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
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_I",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community5_groups,
		.num_groups = ARRAY_SIZE(tgl_community5_groups),
	},
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(tgl_communities);
	return tgl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPD, GPD },
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_E, GPP_E },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_G, GPP_G },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_I, GPP_I },
		{ PMC_GPP_J, GPP_J },
		{ PMC_GPP_K, GPP_K },
		{ PMC_GPP_R, GPP_R },
		{ PMC_GPP_S, GPP_S },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
