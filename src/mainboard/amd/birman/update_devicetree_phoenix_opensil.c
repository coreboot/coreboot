/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/amd/phoenix/chip.h>
#include <soc/soc_util.h>
#include <static.h>
#include <drivers/amd/opensil/mpio/chip.h>
#include "display_card_type.h"
#include "update_devicetree.h"

static void mainboard_update_mpio(void)
{
	struct device *mxm_bridge = DEV_PTR(gpp_bridge_1_1);
	struct device *ssd1_bridge = DEV_PTR(gpp_bridge_1_2);
	struct device *wwan_bridge = DEV_PTR(gpp_bridge_2_2);
	struct device *wlan_bridge = DEV_PTR(gpp_bridge_2_3);
	struct drivers_amd_opensil_mpio_config *mxm_bridge_cfg = config_of(mxm_bridge);
	struct drivers_amd_opensil_mpio_config *ssd1_bridge_cfg = config_of(ssd1_bridge);
	struct drivers_amd_opensil_mpio_config *wwan_bridge_cfg = config_of(wwan_bridge);
	struct drivers_amd_opensil_mpio_config *wlan_bridge_cfg = config_of(wlan_bridge);

	/* Phoenix 2 has less PCIe lanes than Phoenix */
	if (get_soc_type() == SOC_PHOENIX2) {
		mxm_bridge_cfg->end_lane = 3;
		ssd1_bridge_cfg->end_lane = 9;
	}
	if (!CONFIG(ENABLE_EVAL_CARD)) {
		mxm_bridge->enabled = false;
	}
	if (CONFIG(DISABLE_DT_M2)) {
		ssd1_bridge->enabled = false;
	}
	/* When the WLAN card uses 2 lanes, the WWAN card can't be used */
	if (CONFIG(WLAN01)) {
		wwan_bridge->enabled = false;
		wlan_bridge_cfg->end_lane = 14;
	}
	/* When the WWAN card uses 2 lanes, the WLAN card can't be used */
	if (CONFIG(WWAN01)) {
		wlan_bridge->enabled = false;
		wwan_bridge_cfg->end_lane = 15;
	}
}

static void mainboard_update_ddi(void)
{
	struct soc_amd_phoenix_config *cfg = config_of_soc();
	cfg->ddi[1].connector_type =  get_ddi1_type();
}

void mainboard_update_devicetree_opensil(void)
{
	mainboard_update_mpio();
	mainboard_update_ddi();
}
