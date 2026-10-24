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
#include <string.h>

#include "chip.h"

_Static_assert(CONFIG_MAX_SOCKET > 0 && CONFIG_MAX_SOCKET <= PROJ_MAX_SOCKETS_SUPPORTED,
	       "The Turin MPIO topology exceeds openSIL's maximum socket count");
_Static_assert(CONFIG_MAX_SOCKET == CONFIG_PLAT_NUMBER_SOCKETS,
	       "The host and generated openSIL socket counts must agree");

static void mpio_params_config(void)
{
	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);
	struct device *gnb = DEV_PTR(gnb_0);
	struct device *iommu = DEV_PTR(iommu_0);
	struct device *psp = DEV_PTR(asp);
	struct device *nbif = DEV_PTR(nbif_0);

	if (!mpio_data) {
		printk(BIOS_ERR, "OpenSIL: MPIO block not found\n");
		return;
	}

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
	mpio_data->Enable2SpcGen5                      = 0;
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

	/* Empty socket lists are NULL, as accepted by MpioGetTopologyForSocket(). */
	memset(&mpio_data->PcieTopologyData, 0, sizeof(mpio_data->PcieTopologyData));
	for (size_t socket = 0; socket < CONFIG_MAX_SOCKET; socket++) {
		MPIO_COMPLEX_DESCRIPTOR *complex_desc =
			&mpio_data->PcieTopologyData.PlatformData[socket];

		complex_desc->SocketId = socket;
		if (socket == CONFIG_MAX_SOCKET - 1)
			complex_desc->Flags = DESCRIPTOR_TERMINATE_LIST;
	}
}

static void cxl_params_config(void)
{
	CXLCLASS_DATA_BLK *cxl_data = SilFindStructure(SilId_CxlClass, 0);

	if (!cxl_data) {
		printk(BIOS_ERR, "OpenSIL: CXL block not found\n");
		return;
	}

	CXLCLASS_INPUT_BLK *input = &cxl_data->CxlInputBlock;
	input->AmdPcieAerReportMechanism  = 1;
}

static void nbio_params_config(void)
{
	NBIOCLASS_DATA_BLOCK *nbio_data = SilFindStructure(SilId_NbioClass, 0);
	if (!nbio_data) {
		printk(BIOS_ERR, "OpenSIL: NBIO block not found\n");
		return;
	}

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

/* Per-device walk state, reset by opensil_mpio_global_config(). */
static size_t mpio_ports;
static size_t socket_ports[CONFIG_MAX_SOCKET];
static uint32_t slot_num;
static bool bmc_configured;
static uint8_t bmc_lane;

static bool setup_bmc_lanes(uint8_t lane, uint8_t socket)
{
	DFX_RCMGR_INPUT_BLK *rc_mgr_input_block = SilFindStructure(SilId_RcManager,  0);
	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);

	if (!rc_mgr_input_block || !mpio_data) {
		printk(BIOS_ERR, "OpenSIL: Resource Manager or MPIO block not found, "
				 "BMC link not configured\n");
		return false;
	}

	rc_mgr_input_block->BmcSocket = socket;
	rc_mgr_input_block->EarlyBmcLinkLaneNum = lane;

	mpio_data->EarlyBmcLinkSocket                  = socket;
	mpio_data->EarlyBmcLinkLaneNum                 = lane;
	mpio_data->EarlyBmcLinkDie                     = 0;
	mpio_data->EarlyBmcLinkTraining                = true;

	return true;
}

void opensil_mpio_per_device_config(struct device *dev)
{
	MPIOCLASS_INPUT_BLK *mpio_data = SilFindStructure(SilId_MpioClass, 0);

	if (!mpio_data) {
		printk(BIOS_ERR, "OpenSIL: MPIO block not found\n");
		return;
	}

	const struct device *domain_dev = dev_get_domain(dev);

	if (!domain_dev) {
		printk(BIOS_ERR, "MPIO: device has no owning PCI domain\n");
		return;
	}
	const uint32_t domain = domain_dev->path.domain.domain_id;
	/*
	 * Host domains are grouped by socket: 0..7 describe S0, 8..15 describe S1.
	 * These connector descriptions do not supply the second socket's SoC domains or
	 * their SMN/resource access, which must be implemented by the platform.
	 */
	const size_t socket = domain / PROJ_MAX_RBS_PER_SOCKET;

	if (socket >= CONFIG_MAX_SOCKET) {
		printk(BIOS_ERR, "MPIO: domain 0x%x exceeds the configured %u socket(s)\n",
		       domain, CONFIG_MAX_SOCKET);
		return;
	}
	const uint32_t devfn = dev->path.pci.devfn;
	const struct drivers_amd_opensil_mpio_config *const config = dev->chip_info;
	if (dev->path.type == DEVICE_PATH_PCI)
		printk(BIOS_DEBUG, "Setting MPIO port for domain 0x%x, PCI %d:%d\n",
		       domain, PCI_SLOT(devfn), PCI_FUNC(devfn));
	else
		printk(BIOS_DEBUG, "Setting MPIO connector for domain 0x%x\n",
		       domain);

	if (config->type == IFTYPE_UNUSED) {
		if (is_dev_enabled(dev)) {
			printk(BIOS_WARNING, "Unused MPIO chip, disabling PCI device.\n");
			dev->enabled = false;
		} else {
			printk(BIOS_DEBUG, "Unused MPIO chip, skipping.\n");
		}
		return;
	}

	if (config->type != IFTYPE_PCIE && config->type != IFTYPE_SATA) {
		printk(BIOS_ERR, "MPIO: unsupported interface type %u for domain 0x%x\n",
		       config->type, domain);
		return;
	}

	if (config->bmc) {
		/*
		 * Note this returns before any PortList entry is built, so the
		 * speed/aspm registers on a bmc chip are deliberately not used.
		 * A bmc chip must still use a supported type to pass the check above.
		 * The BMC link is described by the APCB, not by the host topology
		 * table. A second bmc chip would silently overwrite the first, since
		 * setup_bmc_lanes() writes one set of input-block fields.
		 */
		if (bmc_configured) {
			printk(BIOS_ERR, "MPIO: more than one bmc chip in the devicetree; "
			       "lane %d ignored, lane %d already applied\n",
			       config->start_lane, bmc_lane);
			return;
		}
		/* Record the BMC lane only after both input blocks have been updated. */
		if (!setup_bmc_lanes(config->start_lane, socket))
			return;
		bmc_configured = true;
		bmc_lane = config->start_lane;
		return;
	}

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
	/*
	 * PortList is a fixed-size array and mpio_ports is driven purely by how
	 * many mpio chips the devicetree declares, so without this a board with
	 * too many ports would corrupt whatever follows the topology in the
	 * openSIL input block rather than fail visibly.
	 *
	 * Checked before slot_num is touched: a port that cannot be stored must
	 * not consume a slot number, or every later port is numbered as though
	 * the rejected one had been accepted.
	 */
	if (mpio_ports >= ARRAY_SIZE(mpio_data->PcieTopologyData.PortList)) {
		printk(BIOS_ERR, "MPIO: no room for domain 0x%x PCI %d:%d, "
		       "PortList holds %zu ports\n", domain, PCI_SLOT(devfn),
		       PCI_FUNC(devfn),
		       ARRAY_SIZE(mpio_data->PcieTopologyData.PortList));
		return;
	}

	if (config->type == IFTYPE_PCIE) {
		port.Port.AlwaysExpose = 1;
		port.Port.SlotNum = ++slot_num;
	}

	/*
	 * Keep a contiguous range for each socket in openSIL's shared array.
	 * Device traversal may interleave sockets, so append inside this socket's
	 * range and shift later ranges. No capacity is reserved for an empty socket.
	 */
	MPIO_PORT_DESCRIPTOR *ports = mpio_data->PcieTopologyData.PortList;
	size_t insert = 0;

	for (size_t i = 0; i <= socket; i++)
		insert += socket_ports[i];
	memmove(&ports[insert + 1], &ports[insert], (mpio_ports - insert) * sizeof(*ports));
	if (socket_ports[socket])
		ports[insert - 1].Flags &= ~DESCRIPTOR_TERMINATE_LIST;
	ports[insert] = port;
	socket_ports[socket]++;
	mpio_ports++;

	size_t offset = 0;
	for (size_t i = 0; i < CONFIG_MAX_SOCKET; i++) {
		mpio_data->PcieTopologyData.PlatformData[i].PciePortList =
			socket_ports[i] ? &ports[offset] : NULL;
		offset += socket_ports[i];
	}
}

void opensil_mpio_global_config(void)
{
	/* Rebuild the topology on every per-device walk. */
	mpio_ports = 0;
	memset(socket_ports, 0, sizeof(socket_ports));
	slot_num = 0;
	bmc_configured = false;
	bmc_lane = 0;

	mpio_params_config();
	/* A new walk starts with the board's default BMC link, not a previous walk's override. */
	setup_bmc_lanes(CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE, 0);
	cxl_params_config();
	nbio_params_config();
}
