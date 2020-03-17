/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/ramstage.h>
#include <fsp/api.h>
#include <FspsUpd.h>
#include <console/console.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig)
{
	printk(BIOS_DEBUG, "MAINBOARD: %s/%s called\n", __FILE__, __func__);

	silconfig->C1e = 0x1;			// 0x0
	silconfig->PkgCStateLimit = 0xFE;	// 0x2
	silconfig->CStateAutoDemotion = 0x3;	// 0x0
	silconfig->CStateUnDemotion = 0x3;	// 0x0
	silconfig->PkgCStateDemotion = 0x1;	// 0x0
	silconfig->PkgCStateUnDemotion = 0x1;	// 0x0
	silconfig->Pme = 0x1;				// 0x0
	silconfig->HdAudioIoBufferOwnership = 0x3;	// 0x0
	silconfig->DspEndpointDmic = 0x0;		// 0x1
	silconfig->DspEndpointBluetooth = 0x0;		// 0x1
	silconfig->DspEndpointI2sSkp = 0x1;		// 0x0
	silconfig->DspEndpointI2sHp = 0x1;		// 0x0
	silconfig->HDAudioPwrGate = 0x1;		// 0x0
	silconfig->HDAudioClkGate = 0x1;		// 0x0
	silconfig->DspFeatureMask = 0x2A;		// 0x0
	silconfig->HpetBdfValid = 0x1;			// 0x0
	silconfig->HpetDeviceNumber = 0xF;		// 0x1f
	silconfig->IoApicBdfValid = 0x1;		// 0x0
	silconfig->IoApicDeviceNumber = 0x1F;		// 0xf
	silconfig->LPSS_S0ixEnable = 0x1;		// 0x0
	silconfig->eMMCHostMaxSpeed = 0x2;		// 0x0
	silconfig->Usb30Mode = 0x1;			// 0x0
	silconfig->HdAudioDspUaaCompliance = 0x1;	// 0x0
	silconfig->InitS3Cpu = 0x1;			// 0x0

	silconfig->PcieRpLtrMaxNonSnoopLatency[0] = 0x1003;	// 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[0] = 0x1003;	// 0x0

	silconfig->PcieRpHotPlug[1] = 0x0;			// 0x1
	silconfig->PcieRpPmSci[1] = 0x1;			// 0x0
	silconfig->PcieRpTransmitterHalfSwing[1] = 0x0;		// 0x1
	silconfig->PcieRpClkReqNumber[1] = 0x3;			// 0x5
	silconfig->PcieRpLtrMaxNonSnoopLatency[1] = 0x1003;	// 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[1] = 0x1003;	// 0x0

	silconfig->PcieRpHotPlug[2] = 0x0;			// 0x1
	silconfig->PcieRpPmSci[2] = 0x1;			// 0x0
	silconfig->PcieRpTransmitterHalfSwing[2] = 0x0;		// 0x1
	silconfig->PcieRpLtrMaxNonSnoopLatency[2] = 0x1003;	// 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[2] = 0x1003;	// 0x0

	silconfig->PcieRpHotPlug[3] = 0x0;		// 0x1
	silconfig->PcieRpPmSci[3] = 0x1;		// 0x0
	silconfig->PcieRpTransmitterHalfSwing[3] = 0x0; // 0x1
	silconfig->PcieRpLtrMaxNonSnoopLatency[3] = 0x1003; // 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[3] = 0x1003;	// 0x0

	silconfig->PcieRpHotPlug[4] = 0x0;		// 0x1
	silconfig->PcieRpPmSci[4] = 0x1;		// 0x0
	silconfig->PcieRpTransmitterHalfSwing[4] = 0x0; // 0x1
	silconfig->PcieRpLtrMaxNonSnoopLatency[4] = 0x1003;	// 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[4] = 0x1003;	// 0x0

	silconfig->PcieRpHotPlug[5] = 0x0;		// 0x1
	silconfig->PcieRpPmSci[5] = 0x1;		// 0x0
	silconfig->PcieRpTransmitterHalfSwing[5] = 0x0; // 0x1
	silconfig->PcieRpLtrMaxNonSnoopLatency[5] = 0x1003;	// 0x0
	silconfig->PcieRpLtrMaxSnoopLatency[5] = 0x1003;	// 0x0
}
