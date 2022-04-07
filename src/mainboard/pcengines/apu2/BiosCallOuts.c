/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <console/console.h>
#include <spd_bin.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <FchPlatform.h>

#include "gpio_ftns.h"
#include "imc.h"
#include "hudson.h"

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr);

const BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_READ_SPD,                 board_ReadSpd_from_cbfs },
	{AGESA_DO_RESET,                 agesa_Reset },
	{AGESA_READ_SPD_RECOVERY,        agesa_NoopUnsupported },
	{AGESA_RUNFUNC_ONAP,             agesa_RunFuncOnAp },
	{AGESA_GET_IDS_INIT_DATA,        agesa_EmptyIdsInitData },
	{AGESA_HOOKBEFORE_DQS_TRAINING,  agesa_NoopSuccess },
	{AGESA_HOOKBEFORE_EXIT_SELF_REF, agesa_NoopSuccess }
};
const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);

//{AGESA_GNB_GFX_GET_VBIOS_IMAGE,  agesa_NoopUnsupported }

/*
 * Hardware Monitor Fan Control
 * Hardware limitation:
 *  HWM will fail to read the input temperature via I2C if other
 *  software switches the I2C address.  AMD recommends using IMC
 *  to control fans, instead of HWM.
 */
static void oem_fan_control(FCH_DATA_BLOCK *FchParams)
{
	FchParams->Imc.ImcEnable = FALSE;
	FchParams->Hwm.HwMonitorEnable = FALSE;
	FchParams->Hwm.HwmFchtsiAutoPoll = FALSE;                /* 1 enable, 0 disable TSI Auto Polling */
}

void board_FCH_InitReset(struct sysinfo *cb_NA, FCH_RESET_DATA_BLOCK *FchParams)
{
	printk(BIOS_DEBUG, "Fch OEM config in INIT RESET ");
	//FchParams_reset->EcChannel0 = TRUE; /* logical devicd 3 */
	FchParams->LegacyFree = CONFIG(HUDSON_LEGACY_FREE);
	FchParams->FchReset.SataEnable = hudson_sata_enable();
	FchParams->FchReset.IdeEnable = hudson_ide_enable();
	FchParams->FchReset.Xhci0Enable = CONFIG(HUDSON_XHCI_ENABLE);
	FchParams->FchReset.Xhci1Enable = FALSE;
	printk(BIOS_DEBUG, "Done\n");
}

void board_FCH_InitEnv(struct sysinfo *cb_NA, FCH_DATA_BLOCK *FchParams)
{
	printk(BIOS_DEBUG, "Fch OEM config in INIT ENV ");

	FchParams->Azalia.AzaliaEnable = AzDisable;

	/* Fan Control */
	oem_fan_control(FchParams);

	/* XHCI configuration */
	FchParams->Usb.Xhci0Enable = CONFIG(HUDSON_XHCI_ENABLE);
	FchParams->Usb.Xhci1Enable = FALSE;

	/* EHCI configuration */
	FchParams->Usb.Ehci3Enable = !CONFIG(HUDSON_XHCI_ENABLE);

	if (CONFIG(BOARD_PCENGINES_APU2)) {
		// Disable EHCI 0 (port 0 to 3)
		FchParams->Usb.Ehci1Enable = FALSE;
	} else {
		// Enable EHCI 0 (port 0 to 3)
		FchParams->Usb.Ehci1Enable = TRUE;
	}

	// Enable EHCI 1 (port 4 to 7)
	// port 4 and 5 to EHCI header port 6 and 7 to PCIe slot.
	FchParams->Usb.Ehci2Enable = TRUE;

	/* sata configuration */
	// Disable DEVSLP0 and 1 to make sure GPIO55 and 59 are not used by DEVSLP
	FchParams->Sata.SataDevSlpPort0 = 0;
	FchParams->Sata.SataDevSlpPort1 = 0;

	FchParams->Sata.SataClass = CONFIG_HUDSON_SATA_MODE;
	switch ((SATA_CLASS)CONFIG_HUDSON_SATA_MODE) {
	case SataRaid:
	case SataAhci:
	case SataAhci7804:
	case SataLegacyIde:
		FchParams->Sata.SataIdeMode = FALSE;
		break;
	case SataIde2Ahci:
	case SataIde2Ahci7804:
	default: /* SataNativeIde */
		FchParams->Sata.SataIdeMode = TRUE;
		break;
	}
	printk(BIOS_DEBUG, "Done\n");
}

static AGESA_STATUS board_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_READ_SPD_PARAMS	*info = ConfigPtr;

	if (!ENV_RAMINIT)
		return AGESA_UNSUPPORTED;

	u8 index = get_spd_offset();

	if (info->MemChannelId > 0)
		return AGESA_UNSUPPORTED;
	if (info->SocketId != 0)
		return AGESA_UNSUPPORTED;
	if (info->DimmId != 0)
		return AGESA_UNSUPPORTED;

	/* Read index 0, first SPD_SIZE bytes of spd.bin file. */
	if (read_ddr3_spd_from_cbfs((u8*)info->Buffer, index) < 0)
		die("No SPD data\n");

	return AGESA_SUCCESS;
}
