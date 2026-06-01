/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg mem_config = {
	.type = MEM_TYPE_DDR4,
	.UserBd = BOARD_TYPE_DESKTOP_2DPC,
	.ddr_config = {
		.dq_pins_interleaved = true,
	},
};

static const struct mem_spd spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus = {
		[0] = {
			.addr_dimm[0] = 0x50,
			.addr_dimm[1] = 0x51,
		},
		[1] = {
			.addr_dimm[0] = 0x52,
			.addr_dimm[1] = 0x53,
		},
	},
};

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Set DMI (Intel's fancy name for CPU-PCH link) to PCIe Gen4 speed.
	 * It's undocumented in FSP headers (for some reason), but MSI sets it
	 * to the same value.
	 * Most likely it also affects all PCH RootPorts, which support Gen4.
	 * DMI ASPM is working, so it doesn't cause any additional power draw.
	 */
	mupd->FspmConfig.DmiMaxLinkSpeed = 4;

	/* Don't let FSP mess with GPIOs, it causes issues such as:
	 * - Broken suspend (system never suspends and reboots after a minute)
	 * - PCIe Gen4 SSDs not being detected (Gen3 were fine... somehow)
	 */
	mupd->FspmConfig.GpioOverride = 0;

	/* Experimental: Enable XMP.
	 * This will be moved to be a CFR option in the future,
	 * but I'm keeping this so I don't have to keep it in my local tree.
	 * Consider this an emotional support API call :)
	 *
	 * Keep in mind that if you enable this, your system might fail to
	 * initialize the memory (and/or be unstable).
	 * It would be pretty catastrophic since there's no recovery mechanism
	 * at the moment, would require re-compiling/re-flashing so I'm not
	 * enabling this by default at this time.
	 *
	 * Selects XMP Profile 1, refer to FSP headers for more options:
	 * RaptorLakeFspBinPkg/Client/RaptorLakeS/Include/FspmUpd.h
	 */
	//mupd->FspmConfig.SpdProfileSelected = 2;

	memcfg_init(mupd, &mem_config, &spd_info, false);
}
