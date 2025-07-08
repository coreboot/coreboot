/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <opensil_config.h>
#include <CCX/Common/CcxApic.h>
#include <Cxl/CxlClass-api.h>
#include <Mpio/Common/MpioStructs.h>
#include <Mpio/MpioClass-api.h>
#include <Nbio/NbioClass-api.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <vendorcode/amd/opensil/opensil.h>
#include <xSIM-api.h>
#include <static.h>

#include "chip.h"

static void mpio_params_config(void)
{
	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);
	struct device *gnb = DEV_PTR(gnb_0);
	struct device *iommu = DEV_PTR(iommu_0);
	struct device *psp = DEV_PTR(asp);
	struct device *nbif = DEV_PTR(nbif_0);

	mpio_data->CfgNbioSsid   = gnb->subsystem_vendor |
				  ((uint32_t)gnb->subsystem_device << 16);
	mpio_data->CfgIommuSsid  = iommu->subsystem_vendor |
				  ((uint32_t)iommu->subsystem_device << 16);
	mpio_data->CfgPspccpSsid = psp->subsystem_vendor |
				  ((uint32_t)psp->subsystem_device << 16);
	mpio_data->CfgNbifF0Ssid = nbif->subsystem_vendor |
				  ((uint32_t)nbif->subsystem_device << 16);
	mpio_data->CfgNtbSsid    = 0; // Not implemented in OpenSIL
	mpio_data->CfgNtbccpSsid = 0; // Not implemented in OpenSIL

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
	mpio_data->AmdAllowCompliance                  = 0xf;
	mpio_data->SrisEnableMode                      = 0xff;
	mpio_data->SrisSkipInterval                    = 0;
	mpio_data->SrisSkpIntervalSel                  = 1;
	mpio_data->SrisCfgType                         = 0;
	mpio_data->SrisAutoDetectMode                  = 0xf;
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
	mpio_data->CfgPcieCVTestWA                     = 0;
	mpio_data->CfgPcieAriSupport                   = 1;
	mpio_data->CfgNbioCTOtoSC                      = 0;
	mpio_data->CfgNbioCTOIgnoreError               = 1;
	mpio_data->AmdPcieSubsystemDeviceID            = 0x1453;
	mpio_data->AmdPcieSubsystemVendorID            = 0x1022;
	mpio_data->GppAtomicOps                        = 1;
	mpio_data->GfxAtomicOps                        = 1;
	mpio_data->AmdNbioReportEdbErrors              = 0;
	mpio_data->OpnSpare                            = 0;
	mpio_data->MPIOAncDataSupport                  = 1;
	mpio_data->AfterResetDelay                     = 0;
	mpio_data->CfgEarlyLink                        = 0;
	mpio_data->AmdCfgExposeUnusedPciePorts         = 1; // Show all ports
	mpio_data->CfgForcePcieGenSpeed                = 0xff;
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
	mpio_data->CfgSevSnpSupport                    = 0;
	mpio_data->CfgSevTioSupport                    = 0;
	mpio_data->PcieIdeCapSup                       = 0;
	mpio_data->Master7bitSteeringTag               = 1;

	/* TODO handle this differently on multisocket */
	mpio_data->PcieTopologyData.PlatformData[0].Flags = DESCRIPTOR_TERMINATE_LIST;
	mpio_data->PcieTopologyData.PlatformData[0].PciePortList = mpio_data->PcieTopologyData.PortList;
}

static void cxl_params_config(void)
{
	CXLCLASS_DATA_BLK *cxl_data = SilFindStructure(SilId_CxlClass, 0);
	CXLCLASS_INPUT_BLK *input = &cxl_data->CxlInputBlock;
	input->AmdPcieAerReportMechanism  = 1;
}

static void nbio_params_config(void)
{
	NBIOCLASS_DATA_BLOCK *nbio_data = SilFindStructure(SilId_NbioClass, 0);
	NBIO_CONFIG_DATA *input = &nbio_data->NbioConfigData;
	input->EsmEnableAllRootPorts        = false;
	input->EsmTargetSpeed               = 16;
	input->CfgRxMarginPersistenceMode   = 1;
	input->SevSnpSupport                = false;
	input->AerEnRccDev0                 = false;
	input->CfgAEREnable                 = true;
	input->AtomicRoutingEnStrap5        = true;
	input->CfgSriovEnDev0F1             = true;
	input->CfgAriEnDev0F1               = true;
	input->CfgAerEnDev0F1               = true;
	input->CfgAcsEnDev0F1               = true;
	input->CfgAtsEnDev0F1               = true;
	input->CfgPasidEnDev0F1             = true;
	input->CfgRtrEnDev0F1               = true;
	input->CfgPriEnDev0F1               = true;
	input->CfgPwrEnDev0F1               = true;
	input->AtcEnable                    = true;
	input->NbifDev0F1AtomicRequestEn    = true;
	input->AcsEnRccDev0                 = true;
	input->AcsP2pReq                    = true;
	input->AcsSourceVal                 = true;
	input->RccDev0E2EPrefix             = true;
	input->RccDev0ExtendedFmtSupported  = true;
	input->CfgSyshubMgcgClkGating       = 1;
	input->IoApicIdPreDefineEn          = true;
	/* Up to 16 IOAPICs for 2 sockets (8 per socket) */
	input->IoApicIdBase                 = 240;
	input->IommuAvicSupport             = true;

	if (CONFIG(XAPIC_ONLY) || CONFIG(X2APIC_LATE_WORKAROUND))
		input->AmdApicMode = xApicMode;
	else if (CONFIG(X2APIC_ONLY))
		input->AmdApicMode = x2ApicMode;
	else
		input->AmdApicMode = ApicAutoMode;
}

static void setup_bmc_lanes(uint8_t lane, uint8_t socket)
{
	DFX_RCMGR_INPUT_BLK *rc_mgr_input_block = SilFindStructure(SilId_RcManager,  0);
	rc_mgr_input_block->BmcSocket = socket;
	rc_mgr_input_block->EarlyBmcLinkLaneNum = lane;

	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);
	mpio_data->EarlyBmcLinkSocket                  = socket;
	mpio_data->EarlyBmcLinkLaneNum                 = lane;
	mpio_data->EarlyBmcLinkDie                     = 0;
	mpio_data->EarlyBmcLinkTraining                = true;
}

void opensil_mpio_per_device_config(struct device *dev)
{
	/* Cache *mpio_data from SilFindStructure */
	static MPIOCLASS_INPUT_BLK *mpio_data = NULL;
	if (mpio_data == NULL) {
		mpio_data = SilFindStructure(SilId_MpioClass, 0);
	}

	static uint32_t slot_num;
	const uint32_t domain = dev_get_domain_id(dev);
	const uint32_t devfn = dev->path.pci.devfn;
	const struct drivers_amd_opensil_mpio_config *const config = dev->chip_info;
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

void opensil_mpio_global_config(void)
{
	mpio_params_config();
	cxl_params_config();
	nbio_params_config();
}
