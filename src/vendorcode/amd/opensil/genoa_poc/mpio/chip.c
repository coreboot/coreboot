/* SPDX-License-Identifier: GPL-2.0-only */

#include <RcMgr/DfX/RcManager4-api.h>
#include <NBIO/NbioClass-api.h>
#include <Mpio/MpioClass-api.h>
#include <Mpio/Common/MpioStructs.h>
#include <device/device.h>
#include <device/pci_def.h>
#include "chip.h"
#include "../opensil.h"

struct chip_operations vendorcode_amd_opensil_chip_mpio_ops = {
	.name = "AMD GENOA MPIO",
};

static void nbio_config(void)
{
	NBIOCLASS_DATA_BLOCK *nbio_data = SilFindStructure(SilId_NbioClass, 0);
	NBIOCLASS_INPUT_BLK *input = &nbio_data->NbioInputBlk;
	input->CfgHdAudioEnable           = false;
	input->EsmEnableAllRootPorts      = false;
	input->EsmTargetSpeed             = 16;
	input->CfgRxMarginPersistenceMode = 1;
	input->CfgDxioFrequencyVetting    = false;
	input->CfgSkipPspMessage          = 1;
	input->CfgEarlyTrainTwoPcieLinks  = false;
	input->EarlyBmcLinkTraining       = true;
	input->EdpcEnable                 = 0;
	input->PcieAerReportMechanism     = 2;
	input->SevSnpSupport              = false;
}

static void mpio_global_config(MPIOCLASS_INPUT_BLK *mpio_data)
{
	mpio_data->CfgDxioClockGating                  = 1;
	mpio_data->PcieDxioTimingControlEnable         = 0;
	mpio_data->PCIELinkReceiverDetectionPolling    = 0;
	mpio_data->PCIELinkResetToTrainingTime         = 0;
	mpio_data->PCIELinkL0Polling                   = 0;
	mpio_data->PCIeExactMatchEnable                = 0;
	mpio_data->DxioPhyValid                        = 1;
	mpio_data->DxioPhyProgramming                  = 1;
	mpio_data->CfgSkipPspMessage                   = 1;
	mpio_data->DxioSaveRestoreModes                = 0xff;
	mpio_data->AmdAllowCompliance                  = 0;
	mpio_data->AmdAllowCompliance                  = 0xff;
	mpio_data->SrisEnableMode                      = 0xff;
	mpio_data->SrisSkipInterval                    = 0;
	mpio_data->SrisSkpIntervalSel                  = 1;
	mpio_data->SrisCfgType                         = 0;
	mpio_data->SrisAutoDetectMode                  = 0xff;
	mpio_data->SrisAutodetectFactor                = 0;
	mpio_data->SrisLowerSkpOsGenSup                = 0;
	mpio_data->SrisLowerSkpOsRcvSup                = 0;
	mpio_data->AmdCxlOnAllPorts                    = 1;
	mpio_data->CxlCorrectableErrorLogging          = 1;
	mpio_data->CxlUnCorrectableErrorLogging        = 1;
	  // This is also available in Nbio. How to handle duplicate entries?
	mpio_data->CfgAEREnable                        = 1;
	mpio_data->CfgMcCapEnable                      = 0;
	mpio_data->CfgRcvErrEnable                     = 0;
	mpio_data->EarlyBmcLinkTraining                = 1;
	mpio_data->SurpriseDownFeature                 = 1;
	mpio_data->LcMultAutoSpdChgOnLastRateEnable    = 0;
	mpio_data->AmdRxMarginEnabled                  = 1;
	mpio_data->CfgPcieCVTestWA                     = 1;
	mpio_data->CfgPcieAriSupport                   = 1;
	mpio_data->CfgNbioCTOtoSC                      = 0;
	mpio_data->CfgNbioCTOIgnoreError               = 1;
	mpio_data->CfgNbioSsid                         = 0;
	mpio_data->CfgIommuSsid                        = 0;
	mpio_data->CfgPspccpSsid                       = 0;
	mpio_data->CfgNtbccpSsid                       = 0;
	mpio_data->CfgNbifF0Ssid                       = 0;
	mpio_data->CfgNtbSsid                          = 0;
	mpio_data->AmdPcieSubsystemDeviceID            = 0x1453;
	mpio_data->AmdPcieSubsystemVendorID            = 0x1022;
	mpio_data->GppAtomicOps                        = 1;
	mpio_data->GfxAtomicOps                        = 1;
	mpio_data->AmdNbioReportEdbErrors              = 0;
	mpio_data->OpnSpare                            = 0;
	mpio_data->AmdPreSilCtrl0                      = 0;
	mpio_data->MPIOAncDataSupport                  = 1;
	mpio_data->AfterResetDelay                     = 0;
	mpio_data->CfgEarlyLink                        = 0;
	mpio_data->AmdCfgExposeUnusedPciePorts         = 1; // Show all ports
	mpio_data->CfgForcePcieGenSpeed                = 0;
	mpio_data->CfgSataPhyTuning                    = 0;
	mpio_data->PcieLinkComplianceModeAllPorts      = 0;
	mpio_data->AmdMCTPEnable                       = 0;
	mpio_data->SbrBrokenLaneAvoidanceSup           = 1;
	mpio_data->AutoFullMarginSup                   = 1;
	  // A getter and setter, both are needed for this PCD.
	mpio_data->AmdPciePresetMask8GtAllPort         = 0xffffffff;
	  // A getter and setter, both are needed for this PCD.
	mpio_data->AmdPciePresetMask16GtAllPort        = 0xffffffff;
	  // A getter and setter, both are needed for this PCD.
	mpio_data->AmdPciePresetMask32GtAllPort        = 0xffffffff;
	mpio_data->PcieLinkAspmAllPort                 = 0xff;

	mpio_data->SyncHeaderByPass                    = 1;
	mpio_data->CxlTempGen5AdvertAltPtcl            = 0;

	/* TODO handle this differently on multisocket */
	mpio_data->PcieTopologyData.PlatformData[0].Flags = DESCRIPTOR_TERMINATE_LIST;
	mpio_data->PcieTopologyData.PlatformData[0].PciePortList = mpio_data->PcieTopologyData.PortList;

}

static void setup_bmc_lanes(uint8_t lane, uint8_t socket)
{
	DFX_RCMGR_INPUT_BLK *rc_mgr_input_block = SilFindStructure(SilId_RcManager,  0);
	rc_mgr_input_block->BmcSocket = socket;
	rc_mgr_input_block->EarlyBmcLinkLaneNum = lane;

	NBIOCLASS_DATA_BLOCK *nbio_data = SilFindStructure(SilId_NbioClass, 0);
	NBIOCLASS_INPUT_BLK *nbio_input = &nbio_data->NbioInputBlk;
	nbio_input->EarlyBmcLinkSocket         = socket;
	nbio_input->EarlyBmcLinkLaneNum        = lane;
	nbio_input->EarlyBmcLinkDie            = 0;

	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);
	mpio_data->EarlyBmcLinkSocket                  = socket;
	mpio_data->EarlyBmcLinkLaneNum                 = lane;
	mpio_data->EarlyBmcLinkDie                     = 0;
}

static void per_device_config(MPIOCLASS_INPUT_BLK *mpio_data, struct device *dev)
{
	static uint32_t slot_num;
	const uint32_t domain = dev_get_domain_id(dev);
	const uint32_t devfn = dev->path.pci.devfn;
	const struct vendorcode_amd_opensil_chip_mpio_config *const config = dev->chip_info;
	printk(BIOS_DEBUG, "Setting MPIO port for domain 0x%x, PCI %d:%d\n",
	       domain, PCI_SLOT(devfn), PCI_FUNC(devfn));

	if (config->type == IFTYPE_UNUSED) {
		if (is_dev_enabled(dev)) {
			printk(BIOS_WARNING, "Unused MPIO chip, disabling PCI device.\n");
			dev->enabled = false;
		} else {
			printk(BIOS_DEBUG, "Unused MPIO chip, skipping.\n");
		}
		return;
	}

	if (config->bmc) {
		setup_bmc_lanes(config->start_lane, 0); // TODO support multiple sockets
		return;
	}

	static int mpio_port = 0;
	MPIO_PORT_DESCRIPTOR port = { .Flags = DESCRIPTOR_TERMINATE_LIST };
	if (config->type == IFTYPE_PCIE) {
		const MPIO_ENGINE_DATA engine_data =
			MPIO_ENGINE_DATA_INITIALIZER(MpioPcieEngine,
						     config->start_lane, config->end_lane,
						     config->hotplug == HotplugDisabled ? 0 : 1,
						     config->gpio_group);
		port.EngineData = engine_data;
		const MPIO_PORT_DATA port_data =
			MPIO_PORT_DATA_INITIALIZER_PCIE(is_dev_enabled(dev) ?
								MpioPortEnabled : MpioPortDisabled,
							PCI_SLOT(devfn),
							PCI_FUNC(devfn),
							config->hotplug,
							config->speed,
							0, // No backup PCIe speed
							config->aspm,
							config->aspm_l1_1,
							config->aspm_l1_2,
							config->clock_pm);
		port.Port = port_data;
	} else if (config->type == IFTYPE_SATA) {
		const MPIO_ENGINE_DATA engine_data =
			MPIO_ENGINE_DATA_INITIALIZER(MpioSATAEngine,
						     config->start_lane, config->end_lane,
						     0, // meaningless field
						     config->gpio_group);
		port.EngineData = engine_data;
		const MPIO_PORT_DATA port_data = { .PortPresent = 1 };
		port.Port = port_data;

	}
	port.Port.AlwaysExpose = 1;
	port.Port.SlotNum = ++slot_num;
	mpio_data->PcieTopologyData.PortList[mpio_port] = port;
	/* Update TERMINATE list */
	if (mpio_port > 0)
		mpio_data->PcieTopologyData.PortList[mpio_port - 1].Flags = 0;
	mpio_port++;
}

void configure_mpio(void)
{
	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);
	mpio_global_config(mpio_data);
	nbio_config();

	/* Find all devices with this chip that are directly below the chip */
	for (struct device *dev = &dev_root; dev; dev = dev->next)
		if (dev->chip_ops == &vendorcode_amd_opensil_chip_mpio_ops &&
		    dev->chip_info != dev->upstream->dev->chip_info)
			per_device_config(mpio_data, dev);
}
