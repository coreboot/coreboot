/* SPDX-License-Identifier: GPL-2.0-only */

#include <FCH/Common/FchCommonCfg.h>
#include <FCH/Common/FchCore/FchSata/FchSata.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <amdblocks/reset.h>
#include <bootstate.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/soc_chip.h>
#include <static.h>
#include <stdio.h>
#include <xSIM-api.h>
#include "opensil_console.h"
#include "../opensil.h"

void SIL_STATUS_report(const char *function, const int status)
{
	const int log_level = status == SilPass ? BIOS_DEBUG : BIOS_ERR;
	const char *error_string = "Unkown error";

	const struct error_string_entry {
		SIL_STATUS status;
		const char *string;
	} errors[] = {
		{SilPass, "SilPass"},
		{SilUnsupportedHardware, "SilUnsupportedHardware"},
		{SilUnsupported, "SilUnsupported"},
		{SilInvalidParameter, "SilInvalidParameter"},
		{SilAborted, "SilAborted"},
		{SilOutOfResources, "SilOutOfResources"},
		{SilNotFound, "SilNotFound"},
		{SilOutOfBounds, "SilOutOfBounds"},
		{SilDeviceError, "SilDeviceError"},
		{SilResetRequestColdImm, "SilResetRequestColdImm"},
		{SilResetRequestColdDef, "SilResetRequestColdDef"},
		{SilResetRequestWarmImm, "SilResetRequestWarmImm"},
		{SilResetRequestWarmDef, "SilResetRequestWarmDef"},
	};

	int i;
	for (i = 0; i < ARRAY_SIZE(errors); i++) {
		if (errors[i].status == status)
			error_string = errors[i].string;
	}
	printk(log_level, "%s returned %d (%s)\n", function, status, error_string);
}

static void setup_rc_manager_default(void)
{
	DFX_RCMGR_INPUT_BLK *rc_mgr_input_block = SilFindStructure(SilId_RcManager,  0);
	/* Let openSIL distribute the resources to the different PCI roots */
	rc_mgr_input_block->SetRcBasedOnNv = false;

	/* Currently 1P is the only supported configuration */
	rc_mgr_input_block->SocketNumber = 1;
	rc_mgr_input_block->RbsPerSocket = 4; /* PCI root bridges per socket */
	rc_mgr_input_block->McptEnable = true;
	rc_mgr_input_block->PciExpressBaseAddress = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	rc_mgr_input_block->BottomMmioReservedForPrimaryRb = 4ull * GiB - 32 * MiB;
	rc_mgr_input_block->MmioSizePerRbForNonPciDevice = 16 * MiB;
	/* MmioAbove4GLimit will be adjusted down in openSIL */
	rc_mgr_input_block->MmioAbove4GLimit = POWER_OF_2(cpu_phys_address_size());
	rc_mgr_input_block->Above4GMmioSizePerRbForNonPciDevice = 0;
}

#define NUM_XHCI_CONTROLLERS 2
static void configure_usb(void)
{
	const struct soc_amd_genoa_poc_config *soc_config = config_of_soc();
	const struct soc_usb_config *usb = &soc_config->usb;

	FCHUSB_INPUT_BLK *fch_usb_data = SilFindStructure(SilId_FchUsb, 0);
	fch_usb_data->Xhci0Enable = usb->xhci0_enable;
	fch_usb_data->Xhci1Enable = usb->xhci1_enable;
	fch_usb_data->Xhci2Enable = false; /* there's no XHCI2 on this SoC */
	for (int i = 0; i < NUM_XHCI_CONTROLLERS; i++) {
		memcpy(&fch_usb_data->XhciOCpinSelect[i].Usb20OcPin, &usb->usb2_oc_pins[i],
		       sizeof(fch_usb_data->XhciOCpinSelect[i].Usb20OcPin));
		memcpy(&fch_usb_data->XhciOCpinSelect[i].Usb31OcPin, &usb->usb3_oc_pins[i],
		       sizeof(fch_usb_data->XhciOCpinSelect[i].Usb31OcPin));
	}
	fch_usb_data->XhciOcPolarityCfgLow = usb->polarity_cfg_low;
	fch_usb_data->Usb3PortForceGen1 = usb->usb3_force_gen1.raw;

	/* Instead of overwriting the whole OemUsbConfigurationTable, only copy the relevant
	   fields to the pre-populated data structure */
	fch_usb_data->OemUsbConfigurationTable.Usb31PhyEnable = usb->usb31_phy_enable;
	if (usb->usb31_phy_enable)
		memcpy(&fch_usb_data->OemUsbConfigurationTable.Usb31PhyPort, usb->usb31_phy,
		       sizeof(fch_usb_data->OemUsbConfigurationTable.Usb31PhyPort));
	fch_usb_data->OemUsbConfigurationTable.Usb31PhyEnable = usb->s1_usb31_phy_enable;
	if (usb->s1_usb31_phy_enable)
		memcpy(&fch_usb_data->OemUsbConfigurationTable.S1Usb31PhyPort, usb->s1_usb31_phy,
		       sizeof(fch_usb_data->OemUsbConfigurationTable.S1Usb31PhyPort));
}

#define NUM_SATA_CONTROLLERS 4
static void configure_sata(void)
{
	FCHSATA_INPUT_BLK *fch_sata_data = SilFindStructure(SilId_FchSata, 0);
	FCH_SATA2 *fch_sata_defaults = GetFchSataData();
	for (int i = 0; i < NUM_SATA_CONTROLLERS; i++) {
		fch_sata_data[i] = fch_sata_defaults[i];
		fch_sata_data[i].SataSetMaxGen2 = false;
		fch_sata_data[i].SataMsiEnable = true;
		fch_sata_data[i].SataEspPort = 0xFF;
		fch_sata_data[i].SataRasSupport = true;
		fch_sata_data[i].SataDevSlpPort1Num = 1;
		fch_sata_data[i].SataMsiEnable = true;
		fch_sata_data[i].SataControllerAutoShutdown = true;
		fch_sata_data[i].SataRxPolarity = 0xFF;
	}
}

void setup_opensil(void)
{
	const SIL_STATUS debug_ret = SilDebugSetup(HostDebugService);
	SIL_STATUS_report("SilDebugSetup", debug_ret);
	const size_t mem_req = xSimQueryMemoryRequirements();
	void *buf = cbmem_add(CBMEM_ID_AMD_OPENSIL, mem_req);
	assert(buf);
	/* We run all openSIL timepoints in the same stage so using TP1 as argument is fine. */
	const SIL_STATUS assign_mem_ret = xSimAssignMemoryTp1(buf, mem_req);
	SIL_STATUS_report("xSimAssignMemory", assign_mem_ret);

	setup_rc_manager_default();
	configure_usb();
	configure_sata();
	configure_mpio();
}

static void opensil_entry(SIL_TIMEPOINT timepoint)
{
	SIL_STATUS ret;
	SIL_TIMEPOINT tp = (uintptr_t)timepoint;

	switch (tp) {
	case SIL_TP1:
		ret = InitializeSiTp1();
		break;
	case SIL_TP2:
		ret = InitializeSiTp2();
		break;
	case SIL_TP3:
		ret = InitializeSiTp3();
		break;
	default:
		printk(BIOS_ERR, "Unknown openSIL timepoint\n");
		return;
	}
	char opensil_function[16];
	snprintf(opensil_function, sizeof(opensil_function), "InitializeSiTp%d", tp);
	SIL_STATUS_report(opensil_function, ret);
	if (ret == SilResetRequestColdImm || ret == SilResetRequestColdDef) {
		printk(BIOS_INFO, "openSIL requested a cold reset");
		do_cold_reset();
	} else if (ret == SilResetRequestWarmImm || ret == SilResetRequestWarmDef) {
		printk(BIOS_INFO, "openSIL requested a warm reset");
		do_warm_reset();
	}
}

void opensil_xSIM_timepoint_1(void)
{
	opensil_entry(SIL_TP1);
}

void opensil_xSIM_timepoint_2(void)
{
	opensil_entry(SIL_TP2);
}

void opensil_xSIM_timepoint_3(void)
{
	opensil_entry(SIL_TP3);
}

/* TODO: also call timepoints 2 and 3 from coreboot. Are they NOOP? */
