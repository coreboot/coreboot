/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/gpio.h>
#include <soc/gpio_snr.h>
#include <soc/pcr_ids.h>

static const struct pad_group snr_community_west2_groups[] = {
	INTEL_GPP(GPIO_WEST2_0, GPIO_WEST2_0, GPIO_WEST2_23),
};

static const struct pad_group snr_community_west3_groups[] = {
	INTEL_GPP(GPIO_WEST3_0, GPIO_WEST3_0, GPIO_WEST3_23),
};

static const struct pad_group snr_community_west01_groups[] = {
	INTEL_GPP(GPIO_WEST01_0, GPIO_WEST01_0, GPIO_WEST01_22),
};

static const struct pad_group snr_community_west5_groups[] = {
	INTEL_GPP(GPIO_WEST5_0, GPIO_WEST5_0, GPIO_WEST5_18),
};

static const struct pad_group snr_community_westb_groups[] = {
	INTEL_GPP(GPIO_WESTB_0, GPIO_WESTB_0, GPIO_WESTB_11),
};

static const struct pad_group snr_community_westd_peci_groups[] = {
	INTEL_GPP(GPIO_WESTD_PECI_0, GPIO_WESTD_PECI_0, GPIO_WESTD_PECI_0),
};

static const struct pad_group snr_community_east2_groups[] = {
	INTEL_GPP(GPIO_EAST2_0, GPIO_EAST2_0, GPIO_EAST2_23),
};

static const struct pad_group snr_community_east3_groups[] = {
	INTEL_GPP(GPIO_EAST3_0, GPIO_EAST3_0, GPIO_EAST3_9),
};

static const struct pad_group snr_community_east0_groups[] = {
	INTEL_GPP(GPIO_EAST0_0, GPIO_EAST0_0, GPIO_EAST0_22),
};

static const struct pad_group snr_community_emmc_groups[] = {
	INTEL_GPP(GPIO_EMMC_0, GPIO_EMMC_0, GPIO_EMMC_10),
};

static const struct pad_community snr_gpio_communities[] = {
	{
		.name = "GPIO_WEST2",
		.acpi_path = "\\_SB.GPO0",
		.num_gpi_regs = GPIO_WEST2_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WEST2_0,
		.last_pad = GPIO_WEST2_23,
		.pad_own_reg_0 = GPIO_WEST2_PAD_OWN,
		.host_own_reg_0 = GPIO_WEST2_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WEST2_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WEST2_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WEST2_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WEST2_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WEST2_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WEST2_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WEST2_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WEST2_NMI_EN,
		.pad_cfg_base = GPIO_WEST2_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WEST2_PADCFGLOCK,
		.gpi_status_offset = 0,
		.port = PID_GPIOCOM1,
		.groups = snr_community_west2_groups,
		.num_groups = ARRAY_SIZE(snr_community_west2_groups),
	},
	{
		.name = "GPIO_WEST3",
		.acpi_path = "\\_SB.GPO1",
		.num_gpi_regs = GPIO_WEST3_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WEST3_0,
		.last_pad = GPIO_WEST3_23,
		.pad_own_reg_0 = GPIO_WEST3_PAD_OWN,
		.host_own_reg_0 = GPIO_WEST3_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WEST3_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WEST3_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WEST3_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WEST3_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WEST3_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WEST3_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WEST3_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WEST3_NMI_EN,
		.pad_cfg_base = GPIO_WEST3_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WEST3_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS,
		.port = PID_GPIOCOM1,
		.groups = snr_community_west3_groups,
		.num_groups = ARRAY_SIZE(snr_community_west3_groups),
	},
	{
		.name = "GPIO_WEST01",
		.acpi_path = "\\_SB.GPO2",
		.num_gpi_regs = GPIO_WEST01_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WEST01_0,
		.last_pad = GPIO_WEST01_22,
		.pad_own_reg_0 = GPIO_WEST01_PAD_OWN,
		.host_own_reg_0 = GPIO_WEST01_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WEST01_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WEST01_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WEST01_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WEST01_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WEST01_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WEST01_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WEST01_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WEST01_NMI_EN,
		.pad_cfg_base = GPIO_WEST01_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WEST01_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS,
		.port = PID_GPIOCOM1,
		.groups = snr_community_west01_groups,
		.num_groups = ARRAY_SIZE(snr_community_west01_groups),
	},
	{
		.name = "GPIO_WEST5",
		.acpi_path = "\\_SB.GPO3",
		.num_gpi_regs = GPIO_WEST5_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WEST5_0,
		.last_pad = GPIO_WEST5_18,
		.pad_own_reg_0 = GPIO_WEST5_PAD_OWN,
		.host_own_reg_0 = GPIO_WEST5_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WEST5_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WEST5_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WEST5_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WEST5_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WEST5_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WEST5_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WEST5_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WEST5_NMI_EN,
		.pad_cfg_base = GPIO_WEST5_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WEST5_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS,
		.port = PID_GPIOCOM1,
		.groups = snr_community_west5_groups,
		.num_groups = ARRAY_SIZE(snr_community_west5_groups),
	},
	{
		.name = "GPIO_WESTB",
		.acpi_path = "\\_SB.GPO4",
		.num_gpi_regs = GPIO_WESTB_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WESTB_0,
		.last_pad = GPIO_WESTB_11,
		.pad_own_reg_0 = GPIO_WESTB_PAD_OWN,
		.host_own_reg_0 = GPIO_WESTB_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WESTB_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WESTB_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WESTB_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WESTB_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WESTB_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WESTB_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WESTB_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WESTB_NMI_EN,
		.pad_cfg_base = GPIO_WESTB_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WESTB_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS,
		.port = PID_GPIOCOM1,
		.groups = snr_community_westb_groups,
		.num_groups = ARRAY_SIZE(snr_community_westb_groups),
	},
	{
		.name = "GPIO_WESTD_PECI",
		.acpi_path = "\\_SB.GPO5",
		.num_gpi_regs = GPIO_WESTD_PECI_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_WESTD_PECI_0,
		.last_pad = GPIO_WESTD_PECI_0,
		.pad_own_reg_0 = GPIO_WESTD_PECI_PAD_OWN,
		.host_own_reg_0 = GPIO_WESTD_PECI_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_WESTD_PECI_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_WESTD_PECI_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_WESTD_PECI_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_WESTD_PECI_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_WESTD_PECI_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_WESTD_PECI_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_WESTD_PECI_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_WESTD_PECI_NMI_EN,
		.pad_cfg_base = GPIO_WESTD_PECI_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_WESTD_PECI_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS +
				     GPIO_WESTB_GPI_STATUS_REGS,
		.port = PID_GPIOCOM1,
		.groups = snr_community_westd_peci_groups,
		.num_groups = ARRAY_SIZE(snr_community_westd_peci_groups),
	},
	{
		.name = "GPIO_EAST2",
		.acpi_path = "\\_SB.GPO11",
		.num_gpi_regs = GPIO_EAST2_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_EAST2_0,
		.last_pad = GPIO_EAST2_23,
		.pad_own_reg_0 = GPIO_EAST2_PAD_OWN,
		.host_own_reg_0 = GPIO_EAST2_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_EAST2_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_EAST2_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_EAST2_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_EAST2_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_EAST2_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_EAST2_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_EAST2_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_EAST2_NMI_EN,
		.pad_cfg_base = GPIO_EAST2_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_EAST2_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS +
				     GPIO_WESTB_GPI_STATUS_REGS +
				     GPIO_WESTD_PECI_GPI_STATUS_REGS,
		.port = PID_GPIOCOM0,
		.groups = snr_community_east2_groups,
		.num_groups = ARRAY_SIZE(snr_community_east2_groups),
	},
	{
		.name = "GPIO_EAST3",
		.acpi_path = "\\_SB.GPO12",
		.num_gpi_regs = GPIO_EAST3_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_EAST3_0,
		.last_pad = GPIO_EAST3_9,
		.pad_own_reg_0 = GPIO_EAST3_PAD_OWN,
		.host_own_reg_0 = GPIO_EAST3_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_EAST3_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_EAST3_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_EAST3_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_EAST3_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_EAST3_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_EAST3_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_EAST3_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_EAST3_NMI_EN,
		.pad_cfg_base = GPIO_EAST3_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_EAST3_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS +
				     GPIO_WESTB_GPI_STATUS_REGS +
				     GPIO_WESTD_PECI_GPI_STATUS_REGS +
				     GPIO_EAST2_GPI_STATUS_REGS,
		.port = PID_GPIOCOM0,
		.groups = snr_community_east3_groups,
		.num_groups = ARRAY_SIZE(snr_community_east3_groups),
	},
	{
		.name = "GPIO_EAST0",
		.acpi_path = "\\_SB.GPO13",
		.num_gpi_regs = GPIO_EAST0_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_EAST0_0,
		.last_pad = GPIO_EAST0_22,
		.pad_own_reg_0 = GPIO_EAST0_PAD_OWN,
		.host_own_reg_0 = GPIO_EAST0_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_EAST0_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_EAST0_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_EAST0_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_EAST0_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_EAST0_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_EAST0_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_EAST0_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_EAST0_NMI_EN,
		.pad_cfg_base = GPIO_EAST0_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_EAST0_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS +
				     GPIO_WESTB_GPI_STATUS_REGS +
				     GPIO_WESTD_PECI_GPI_STATUS_REGS +
				     GPIO_EAST2_GPI_STATUS_REGS + GPIO_EAST3_GPI_STATUS_REGS,
		.port = PID_GPIOCOM0,
		.groups = snr_community_east0_groups,
		.num_groups = ARRAY_SIZE(snr_community_east0_groups),
	},
	{
		.name = "GPIO_EMMC",
		.acpi_path = "\\_SB.GPO14",
		.num_gpi_regs = GPIO_EMMC_GPI_STATUS_REGS,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.first_pad = GPIO_EMMC_0,
		.last_pad = GPIO_EMMC_10,
		.pad_own_reg_0 = GPIO_EMMC_PAD_OWN,
		.host_own_reg_0 = GPIO_EMMC_HOSTSW_OWN,
		.gpi_int_sts_reg_0 = GPIO_EMMC_GPI_IS,
		.gpi_int_en_reg_0 = GPIO_EMMC_GPI_IE,
		.gpi_smi_sts_reg_0 = GPIO_EMMC_SMI_STS,
		.gpi_smi_en_reg_0 = GPIO_EMMC_SMI_EN,
		.gpi_gpe_sts_reg_0 = GPIO_EMMC_GPI_GPE_STS,
		.gpi_gpe_en_reg_0 = GPIO_EMMC_GPI_GPE_EN,
		.gpi_nmi_sts_reg_0 = GPIO_EMMC_NMI_STS,
		.gpi_nmi_en_reg_0 = GPIO_EMMC_NMI_EN,
		.pad_cfg_base = GPIO_EMMC_PADCFG_OFFSET,
		.pad_cfg_lock_offset = GPIO_EMMC_PADCFGLOCK,
		.gpi_status_offset = GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +
				     GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS +
				     GPIO_WESTB_GPI_STATUS_REGS +
				     GPIO_WESTD_PECI_GPI_STATUS_REGS +
				     GPIO_EAST2_GPI_STATUS_REGS + GPIO_EAST3_GPI_STATUS_REGS +
				     GPIO_EAST0_GPI_STATUS_REGS,
		.port = PID_GPIOCOM0,
		.groups = snr_community_emmc_groups,
		.num_groups = ARRAY_SIZE(snr_community_emmc_groups),
	}
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(snr_gpio_communities);
	return snr_gpio_communities;
}
