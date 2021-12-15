/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/agesawrapper.h>
#include <device/device.h>
#include <soc/pci_devs.h>

static int is_sata_config(void)
{
	return !((SataNativeIde == CONFIG_STONEYRIDGE_SATA_MODE)
			|| (SataLegacyIde == CONFIG_STONEYRIDGE_SATA_MODE));
}

static inline int sb_sata_enable(void)
{
	/* True if IDE or AHCI. */
	return (SataNativeIde == CONFIG_STONEYRIDGE_SATA_MODE) ||
		(SataAhci == CONFIG_STONEYRIDGE_SATA_MODE);
}

static inline int sb_ide_enable(void)
{
	/* True if IDE or LEGACY IDE. */
	return (SataNativeIde == CONFIG_STONEYRIDGE_SATA_MODE) ||
		(SataLegacyIde == CONFIG_STONEYRIDGE_SATA_MODE);
}

void SetFchResetParams(FCH_RESET_INTERFACE *params)
{
	const struct device *dev = pcidev_path_on_root(SATA_DEVFN);
	params->Xhci0Enable = CONFIG(STONEYRIDGE_XHCI_ENABLE);
	if (dev && dev->enabled) {
		params->SataEnable = sb_sata_enable();
		params->IdeEnable = sb_ide_enable();
	} else {
		params->SataEnable = FALSE;
		params->IdeEnable = FALSE;
	}
}

void SetFchEnvParams(FCH_INTERFACE *params)
{
	const struct device *dev = pcidev_path_on_root(SATA_DEVFN);
	params->AzaliaController = AzEnable;
	params->SataClass = CONFIG_STONEYRIDGE_SATA_MODE;
	if (dev && dev->enabled) {
		params->SataEnable = is_sata_config();
		params->IdeEnable = !params->SataEnable;
		params->SataIdeMode = (CONFIG_STONEYRIDGE_SATA_MODE ==
					SataLegacyIde);
	} else {
		params->SataEnable = FALSE;
		params->IdeEnable = FALSE;
		params->SataIdeMode = FALSE;
	}
}

void SetFchMidParams(FCH_INTERFACE *params)
{
	SetFchEnvParams(params);
}
