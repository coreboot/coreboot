/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <amdblocks/BiosCallOuts.h>
#include <console/console.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/dimm_spd.h>
#include <amdblocks/car.h>

#include "chip.h"

void __weak platform_FchParams_reset(FCH_RESET_DATA_BLOCK *FchParams_reset) {}

AGESA_STATUS agesa_fch_initreset(uint32_t Func, uintptr_t FchData,
							void *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;

	if (StdHeader->Func == AMD_INIT_RESET) {
		FCH_RESET_DATA_BLOCK *FchParams_reset;
		FchParams_reset = (FCH_RESET_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");

		/* Get platform specific configuration changes */
		platform_FchParams_reset(FchParams_reset);

		printk(BIOS_DEBUG, "Done\n");
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_fch_initenv(uint32_t Func, uintptr_t FchData,
							void *ConfigPtr)
{
	AMD_CONFIG_PARAMS *StdHeader = ConfigPtr;
	const struct device *dev = pcidev_path_on_root(SATA_DEVFN);

	if (StdHeader->Func == AMD_INIT_ENV) {
		FCH_DATA_BLOCK *FchParams_env = (FCH_DATA_BLOCK *)FchData;
		printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

		/* XHCI configuration */
		if (CONFIG(STONEYRIDGE_XHCI_ENABLE))
			FchParams_env->Usb.Xhci0Enable = TRUE;
		else
			FchParams_env->Usb.Xhci0Enable = FALSE;
		FchParams_env->Usb.Xhci1Enable = FALSE;

		/* SATA configuration */
		FchParams_env->Sata.SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
		if (dev && dev->enabled) {
			switch ((SATA_CLASS)CONFIG_STONEYRIDGE_SATA_MODE) {
			case SataRaid:
			case SataAhci:
			case SataAhci7804:
			case SataLegacyIde:
				FchParams_env->Sata.SataIdeMode = FALSE;
				break;
			case SataIde2Ahci:
			case SataIde2Ahci7804:
			default: /* SataNativeIde */
				FchParams_env->Sata.SataIdeMode = TRUE;
				break;
			}
		} else
			FchParams_env->Sata.SataIdeMode = FALSE;

		/* Platform updates */
		platform_FchParams_env(FchParams_env);

		printk(BIOS_DEBUG, "Done\n");
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_ReadSpd(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	uint8_t spd_address;
	int err;
	DEVTREE_CONST struct device *dev;
	DEVTREE_CONST struct soc_amd_stoneyridge_config *conf;
	AGESA_READ_SPD_PARAMS *info = ConfigPtr;

	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	dev = pcidev_path_on_root(DCT_DEVFN);
	if (dev == NULL)
		return AGESA_ERROR;

	conf = dev->chip_info;
	if (conf == NULL)
		return AGESA_ERROR;

	if (info->SocketId >= ARRAY_SIZE(conf->spd_addr_lookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(conf->spd_addr_lookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(conf->spd_addr_lookup[0][0]))
		return AGESA_ERROR;

	spd_address = conf->spd_addr_lookup
		[info->SocketId][info->MemChannelId][info->DimmId];
	if (spd_address == 0)
		return AGESA_ERROR;

	err = mainboard_read_spd(spd_address, (void *)info->Buffer,
				CONFIG_DIMM_SPD_SIZE);

	/* Read the SPD if the mainboard didn't fill the buffer */
	if (err || (*info->Buffer == 0))
		err = sb_read_spd(spd_address, (void *)info->Buffer,
				CONFIG_DIMM_SPD_SIZE);

	if (err)
		return AGESA_ERROR;

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_HaltThisAp(uint32_t Func, uintptr_t Data, void *ConfigPtr)
{
	AGESA_HALT_THIS_AP_PARAMS *info = ConfigPtr;
	uint32_t flags = 0;

	if (info->PrimaryCore == TRUE)
		return AGESA_UNSUPPORTED; /* force normal path */
	if (info->ExecWbinvd == TRUE)
		flags |= 1;
	if (info->CacheEn == TRUE)
		flags |= 2;

	ap_teardown_car(flags); /* does not return */

	/* Should never reach here */
	return AGESA_UNSUPPORTED;
}

/* Allow mainboards to fill the SPD buffer */
__weak int mainboard_read_spd(uint8_t spdAddress, char *buf,
						size_t len)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
	return -1; /* SPD not read */
}
