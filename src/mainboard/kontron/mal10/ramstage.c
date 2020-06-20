/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <carrier/gpio.h>
#include <stddef.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig)
{
	carrier_gpio_configure();

	/*
	 * CPU Power Management Configuration correspond to the BIOS Setup menu settings
	 * in the AMI UEFI v112.
	 * TODO: move these FSP options to device tree
	 */
	silconfig->EnableCx = 1;            /* Enable CPU power states */
	silconfig->C1e = 1;                 /* enable Enhanced C-states */
	/*
	 * Attention! Do not change PkgCStateLimit! This causes spikes in the power
	 * consumption of the SoC when the system comes out of power saving mode, and
	 * voltage sagging at the output of DC-DC converters on the COMe module. In the
	 * AMI BIOS Setup shows this parameter, but does not allow changing it.
	 */
	silconfig->PkgCStateLimit = 0;      /* Max Pkg Cstate : PkgC0C1 */
	silconfig->MaxCoreCState = 3;       /* Max Core C-State : C6 */
	silconfig->CStateAutoDemotion = 0;  /* Disable C1 and C3 Auto-demotion */
	silconfig->CStateUnDemotion = 0;    /* Disable C1 and C3 Un-demotion */
	silconfig->PkgCStateDemotion = 1;   /* enable package Cstate demotion */
	silconfig->PkgCStateUnDemotion = 1; /* enable package Cstate undemotion */
	silconfig->PmSupport = 1;           /* GT PM Support */
	silconfig->EnableRenderStandby = 1; /* enable render standby */
	silconfig->LPSS_S0ixEnable = 1;     /* LPSS IOSF PMCTL S0ix Enable */
	silconfig->InitS3Cpu = 1;           /* initialize CPU during S3 resume */

	/* Override High Precision Timer options */
	silconfig->HpetBdfValid = 1;
	silconfig->HpetBusNumber = 0xFA;
	silconfig->HpetDeviceNumber = 0x0F;
	silconfig->HpetFunctionNumber = 0;

	/* Override APIC options */
	silconfig->IoApicId = 1;
	silconfig->IoApicBdfValid = 1;
	silconfig->IoApicBusNumber = 0xFA;
	silconfig->IoApicDeviceNumber = 0x1F;
	silconfig->IoApicFunctionNumber = 0;
}
