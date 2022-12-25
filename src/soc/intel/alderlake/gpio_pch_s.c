/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

#define DEFAULT_VW_BASE		0x10

/*
 * This file is created based on Intel Alder Lake Processor PCH-S Datasheet
 */

static const struct reset_mapping rst_map_gpp[] = {
	{ .logical = PAD_RESET(RSMRST), .chipset = 0U << 30 },
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
	INTEL_GPP_BASE(GPP_I0, GPP_I0, GPP_GSPI1_CLK_LOOPBK, 0),	/* GPP_I */
	INTEL_GPP_BASE(GPP_I0, GPP_R0, GPP_GSPI2_CLK_LOOPBK, 32),	/* GPP_R */
	INTEL_GPP_BASE(GPP_I0, GPP_J0, GPP_J11, 64),			/* GPP_J */
	INTEL_GPP_BASE(GPP_I0, VGPIO_0, VGPIO_37, 96),			/* vGPIO */
	INTEL_GPP_BASE(GPP_I0, VGPIO_USB_0, VGPIO_USB_11, 128),		/* vGPIO_0 */
};

static const struct vw_entries adl_community0_vw[] = {
	{GPP_I0, GPP_I14},
	{GPP_R0, GPP_R19},
	{GPP_J0, GPP_J9},
};

static const struct pad_group adl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_B0, GPP_B0, GPP_B23, 160),		/* GPP_B */
	INTEL_GPP_BASE(GPP_B0, GPP_G0, GPP_G7, 192),		/* GPP_G */
	INTEL_GPP_BASE(GPP_B0, GPP_H0, GPP_H23, 224),		/* GPP_H */
};

static const struct vw_entries adl_community1_vw[] = {
	{GPP_B0, GPP_B23},
	{GPP_G0, GPP_G7},
	{GPP_H0, GPP_H23},
};

/* This community is not visible to the OS */
static const struct pad_group adl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD_DRAM_RESETB),			/* GPD */
};

static const struct pad_group adl_community3_groups[] = {
	INTEL_GPP(GPP_SPI0_IO_2, GPP_SPI0_IO_2, GPP_SPI0_CLK_LOOPBK),		/* SPI0 */
	INTEL_GPP_BASE(GPP_SPI0_IO_2, GPP_A0, GPP_ESPI_CLK_LOOPBK, 256),	/* GPP_A */
	INTEL_GPP_BASE(GPP_SPI0_IO_2, GPP_C0, GPP_C23, 288),			/* GPP_C */
	INTEL_GPP(GPP_SPI0_IO_2, VGPIO_PCIE_0, VGPIO_PCIE_83),			/* vGPIO_3 */
};

static const struct vw_entries adl_community3_vw[] = {
	{GPP_A0, GPP_A14},
	{GPP_C0, GPP_C23},
};

static const struct pad_group adl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_S0, GPP_S0, GPP_S7, 320),				/* GPP_S */
	INTEL_GPP_BASE(GPP_S0, GPP_E0, GPP_SPI1_THC0_CLK_LOOPBK, 352),		/* GPP_E */
	INTEL_GPP_BASE(GPP_S0, GPP_K0, GPP_MLK_RSTB, 384),			/* GPP_K */
	INTEL_GPP_BASE(GPP_S0, GPP_F0, GPP_F23, 416),				/* GPP_F */
};

static const struct vw_entries adl_community4_vw[] = {
	{GPP_E0, GPP_E21},
	{GPP_K0, GPP_K11},
	{GPP_F0, GPP_F23},
};

static const struct pad_group adl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_D0, GPP_D0, GPP_GSPI3_THC1_CLK_LOOPBK, 448),		/* GPP_D */
	INTEL_GPP(GPP_D0, GPP_JTAG_TDO, GPP_CPU_TRSTB),				/* JTAG */
	INTEL_GPP(GPP_D0, GPP_HDACPU_SDI, GPP_C10_WAKE),			/* CPU */
};

static const struct pad_community adl_communities[] = {
	[COMM_0] = { /* GPP I, R, J, vGPIO. vGPIO_0 */
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
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_IRJ",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community0_groups,
		.num_groups = ARRAY_SIZE(adl_community0_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community0_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community0_vw),
	},
	[COMM_1] = { /* GPP B, G, H */
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
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_BGH",
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
	[COMM_3] = { /* SPI0, GPP A, C */
		.port = PID_GPIOCOM3,
		.cpu_port = PID_CPU_GPIOCOM3,
		.first_pad = GPIO_COM3_START,
		.last_pad = GPIO_COM3_END,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_AC",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community3_groups,
		.num_groups = ARRAY_SIZE(adl_community3_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community3_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community3_vw),
	},
	[COMM_4] = { /* GPP S, E, K, F */
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
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_SEKF",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_gpp,
		.num_reset_vals = ARRAY_SIZE(rst_map_gpp),
		.groups = adl_community4_groups,
		.num_groups = ARRAY_SIZE(adl_community4_groups),
		.vw_base = DEFAULT_VW_BASE,
		.vw_entries = adl_community4_vw,
		.num_vw_entries = ARRAY_SIZE(adl_community4_vw),
	},
	[COMM_5] = { /* GPP D, JTAG, CPU */
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
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_D",
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
		{  PMC_GPP_I,    GPP_I },
		{  PMC_GPP_R,    GPP_R },
		{  PMC_GPP_J,    GPP_J },
		{  PMC_GPD,      GPD   },
		{  PMC_GPP_D,    GPP_D },
		{  PMC_GPP_S,    GPP_S },
		{  PMC_GPP_E,    GPP_E },
		{  PMC_GPP_K,    GPP_K },
		{  PMC_GPP_F,    GPP_F },
		{  PMC_GPP_A,    GPP_A },
		{  PMC_GPP_C,    GPP_C },
		{  PMC_GPP_B,    GPP_B },
		{  PMC_GPP_G,    GPP_G },
		{  PMC_GPP_H,    GPP_H },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
};
