/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_CONFIGURE_PADS(a) gpio_configure_pads(a, ARRAY_SIZE(a))

static const struct pad_config pre_mem_x1slot_pads[] = {
	/* GPP_A08:     X1_PCIE_SLOT_PWR_EN */
	PAD_CFG_GPO(GPP_A08, 0, PLTRST),
};

/*
 * WWAN: power sequence requires three stages:
 * step 1: 3.3V power, FCP# (Full Card Power), RST#, and PERST# off
 * step 2: deassert FCP#
 * step 3: deassert RST# first, and then PERST#.
 * NOTE: Since PERST# is gated by platform reset, PERST# deassertion will happen
 * at much later time and time between RST# and PERSET# is guaranteed.
 */
static const struct pad_config pre_mem_wwan_pwr_seq1_pads[] = {
	/* GPP_H16:     WWAN_PWREN */
	PAD_CFG_GPO(GPP_H16, 1, PLTRST),
	/* GPP_A09:  M.2_WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_A09, 0, PLTRST),
	/* GPP_B20:     M.2_WWAN_RST_N */
	PAD_CFG_GPO(GPP_B20, 0, PLTRST),
	/* GPP_D03:     M.2_WWAN_PERST_GPIO_N */
	PAD_CFG_GPO(GPP_D03, 0, PLTRST),
};

static const struct pad_config pre_mem_wwan_pwr_seq2_pads[] = {
	/* GPP_A09:     M.2_WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_A09, 1, PLTRST),
};

/* gen4 NVME: at the top M.2 slot */
static const struct pad_config pre_mem_gen4_ssd_pwr_pads[] = {
	/* GPP_B10:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_B10, 0, PLTRST),
};

/* gen5 NVME: at the bottom M.2 slot */
static const struct pad_config pre_mem_gen5_ssd_pwr_pads[] = {
	/* GPP_B16:     GEN5_SSD_PWREN */
	PAD_CFG_GPO(GPP_B16, 0, PLTRST),
};

/* camera1: WFC  */
static const struct pad_config pre_mem_wfc_camera_pwr_pads[] = {
	/* GPP_C05:     CRD1_PWREN */
	PAD_CFG_GPO(GPP_C05, 0, PLTRST),
};

/* camera2: UFC */
static const struct pad_config pre_mem_ufc_camera_pwr_pads[] = {
	/* GPP_C08:     CRD2_PWREN */
	PAD_CFG_GPO(GPP_C08, 0, PLTRST),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_PCIE)) ||
		fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_USB))) {
		GPIO_CONFIGURE_PADS(pre_mem_wwan_pwr_seq1_pads);
	}

	if (fw_config_probe(FW_CONFIG(WFC, WFC_MIPI))) {
		GPIO_CONFIGURE_PADS(pre_mem_wfc_camera_pwr_pads);
	}

	if (fw_config_probe(FW_CONFIG(UFC, UFC_MIPI))) {
		GPIO_CONFIGURE_PADS(pre_mem_ufc_camera_pwr_pads);
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN5))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		/* TODO */
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pwr_pads);
		/* TODO for UFS */
	}

	if (fw_config_probe(FW_CONFIG(SD, SD_GENSYS)) ||
		fw_config_probe(FW_CONFIG(SD, SD_BAYHUB))) {
		GPIO_CONFIGURE_PADS(pre_mem_x1slot_pads);
	}
	/*
	 * NOTE: We place WWAN sequence 2 here. According to the WWAN FIBOCOM
	 * FM350-GL datasheet, the minimum time requirement (Tpr: time between 3.3V
	 * and FCP#) is '0'. Therefore, it will be fine even though there is no
	 * GPIO configured for other PADs via fw_config to have the time delay
	 * introduced in between sequence 1 and 2. Also, FCP# was not the last PAD
	 * configured in sequence 1. Although the Tpr is '0' in the datasheet, three
	 * stages are preserved at this time to guarantee the sequence shown in the
	 * datasheet timing diagram.
	 */
	if (fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_PCIE)) ||
		fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_USB))) {
		GPIO_CONFIGURE_PADS(pre_mem_wwan_pwr_seq2_pads);
	}
}
