/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/x86/name.h>
#include <delay.h>
#include <device/device.h>
#include <intelblocks/systemagent.h>
#include <smbios.h>
#include <soc/ramstage.h>
#include <soc/pcr_ids.h>
#include <soc/tcss.h>
#include <string.h>

#include "board_beep.h"

const char *smbios_mainboard_product_name(void)
{
	char processor_name[49];

	fill_processor_name(processor_name);

	if (strstr(processor_name, "i3-1215U") != NULL)
		return "VP6630";
	else if (strstr(processor_name, "i5-1235U") != NULL)
		return "VP6650";
	else if (strstr(processor_name, "i7-1255U") != NULL)
		return "VP6670";
	else
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	int i;

	memset(params->PcieRpEnableCpm, 0, sizeof(params->PcieRpEnableCpm));
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	memset(params->CpuPcieRpEnableCpm, 0, sizeof(params->CpuPcieRpEnableCpm));
	memset(params->CpuPcieClockGating, 0, sizeof(params->CpuPcieClockGating));
	memset(params->CpuPciePowerGating, 0, sizeof(params->CpuPciePowerGating));
	memset(params->CpuPcieRpPmSci, 0, sizeof(params->CpuPcieRpPmSci));

	/* Max payload 256B */
	memset(params->PcieRpMaxPayload, 1, sizeof(params->PcieRpMaxPayload));

	/* CLKREQs connected only to RP5 and RP6 */
	params->PcieRpEnableCpm[4] = 1;
	params->PcieRpEnableCpm[5] = 1;

	/* Type-C PD */
	params->PmcPdEnable = 1;
	params->PchSerialIoI2cSdaPinMux[1] = 0x1947c606; /* GPP_H6 */
	params->PchSerialIoI2cSclPinMux[1] = 0x1947a607; /* GPP_H7 */
	params->PortResetMessageEnable[7] = 1;

	/* IOM USB config */
	params->PchUsbOverCurrentEnable = 0;

	params->EnableTcssCovTypeA[0] = 1;
	params->EnableTcssCovTypeA[1] = 1;
	params->EnableTcssCovTypeA[3] = 1;

	params->MappingPchXhciUsbA[0] = 1;
	params->MappingPchXhciUsbA[1] = 2;
	params->MappingPchXhciUsbA[3] = 4;

	params->CnviRfResetPinMux = 0;
	params->CnviClkreqPinMux = 0;

	/*
	 * Workaround: poll for IOM ready before SiliconInit for 2 seconds. ME
	 * seems to be too sluggish with its firmware initialization and IOM
	 * is not ready during TCSS Init in SiliconInit, when serial console
	 * debugging is disabled in coreboot. Entering FSP SiliconInit without
	 * IOM ready, will cause the XHCI controller in the CPU to be disabled
	 * and all USB 3.x ports on the platform non-functional.
	 */
	for (i = 0; i < 200; i++) {
		if (REGBAR32(PID_IOM, IOM_TYPEC_STATUS_1) & IOM_READY)
			return;

		mdelay(10);
	}

	printk(BIOS_ERR, "TCSS IOM not ready, USB3.0 ports will not be functional\n");
}

static void mainboard_final(void *chip_info)
{
	if (CONFIG(BEEP_ON_BOOT))
		do_beep(1500, 100);
}

struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};
