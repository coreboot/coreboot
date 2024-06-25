/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

#include "romstage.h"

static void mainboard_hsio_config_params(FSPM_UPD *mupd)
{
	static BL_HSIO_INFORMATION high_speed_io_config;

	/**
	 * HSIO lanes are shared by PCH PCIe root ports, SATA ports and USB port, just leave it as default.
	 */
	for (uint8_t lane = 0; lane < BL_MAX_FIA_LANES; lane++) {
		high_speed_io_config.FiaLaneConfig[lane] = BL_FIA_LANE_OVERRIDE_DISABLED;
		high_speed_io_config.FiaLaneLinkWidth[lane] =
			BL_FIA_LANE_PCIE_ROOT_PORT_LINK_WIDTH_SET_BY_BICTRL;
	}

	mupd->FspmConfig.PcdFiaLaneConfigPtr = (uint32_t)&high_speed_io_config;
}

static void mainboard_pcie_init(FSPM_UPD *mupd)
{
	/**
	 * The following UPD value are related to port bifurcation and hot plug:
	 *     1. `mupd->FspmConfig.PcdIIOPciePortBifurcation,
	 *     2. `mupd->FspmConfig.PcdIIoPciePort1[ABCD]HPCapable`,
	 *     3. `mupd->FspmConfig.PcdIIoPciePort1[ABCD]HPSurprise`,
	 *     4. `mupd->FspmConfig.PcdBifurcationPcie[02]` and
	 *     5. `mupd->FspmConfig.PcdPcieHotPlugEnable`.
	 *
	 * For example, if the `mupd->FspmConfig.PcdIIOPciePortBifurcation` is 0 (x4x4x4x4), then
	 * set `mupd->FspmConfig.PcdIIoPciePort1[ABCD]HPCapable` and `mupd->FspmConfig.PcdIIoPciePort1[ABCD]HPSurprise`
	 * to enable hot plug capable and surprise on each Root Port.
	 *
	 * For PCH PCIe Root Port, item 4 and 5 are used.
	 */
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mainboard_hsio_config_params(mupd);

	if (CONFIG(PCIEXP_HOTPLUG))
		mainboard_pcie_init(mupd);
}
