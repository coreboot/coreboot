/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/debug.h>
#include <fsp/soc_binding.h>
#include <fsp/util.h>
#include <lib.h>
#include <uuid.h>

#include "fsp_hob.h"

struct guid_name_map {
	const guid_t *guid;
	const char *name;
};

static const struct guid_name_map guid_names[] = {
	{&fsp_hob_fia_override_status_guid, "FSP_HOB_FIA_OVERRIDE_STATUS_GUID"},
	{&fsp_hob_iio_uds_data_guid,        "FSP_HOB_IIO_UDS_DATA_GUID"       },
	{&fsp_hob_kti_cache_guid,           "FSP_HOB_KTI_HOST_NVRAM_DATA_GUID"},
	{&fsp_hob_smbios_memory_info_guid,  "FSP_HOB_SMBIOS_MEMORY_INFO_GUID" },
};

const char *soc_get_guid_name(const uint8_t *guid)
{
	size_t index;

	/* Compare the GUID values in this module */
	for (index = 0; index < ARRAY_SIZE(guid_names); index++)
		if (fsp_guid_compare(guid, guid_names[index].guid->b))
			return guid_names[index].name;

	return NULL;
}

static void soc_display_fsp_iio_uds_data_hob(const BL_IIO_UDS *hob)
{
	if (!hob) {
		return;
	}

	printk(BIOS_DEBUG, "IIO UDS\n");
	printk(BIOS_DEBUG, "\t Platform Data\n");
	printk(BIOS_DEBUG, "\t\t PlatGlobalIoBase: 0x%x\n", hob->PlatformData.PlatGlobalIoBase);
	printk(BIOS_DEBUG, "\t\t PlatGlobalIoLimit: 0x%x\n",
	       hob->PlatformData.PlatGlobalIoLimit);
	printk(BIOS_DEBUG, "\t\t PlatGlobalMmio32Base: 0x%x\n",
	       hob->PlatformData.PlatGlobalMmio32Base);
	printk(BIOS_DEBUG, "\t\t PlatGlobalMmio32Limit: 0x%x\n",
	       hob->PlatformData.PlatGlobalMmio32Limit);
	printk(BIOS_DEBUG, "\t\t PlatGlobalMmio64Base: 0x%llx\n",
	       hob->PlatformData.PlatGlobalMmio64Base);
	printk(BIOS_DEBUG, "\t\t PlatGlobalMmio64Limit: 0x%llx\n",
	       hob->PlatformData.PlatGlobalMmio64Limit);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		const BL_QPI_CPU_DATA *cpu_qpi_info = &hob->PlatformData.CpuQpiInfo[socket];
		printk(BIOS_DEBUG, "\t\t CpuQpiInfo[%d]\n", socket);
		printk(BIOS_DEBUG, "\t\t\t Valid: 0x%x\n", cpu_qpi_info->Valid);
		for (int bar = 0; bar < BL_TYPE_MAX_MMIO_BAR; bar++) {
			printk(BIOS_DEBUG, "\t\t\t MmioBar[%d]: 0x%x\n", bar,
			       cpu_qpi_info->MmioBar[bar]);
		}
		printk(BIOS_DEBUG, "\t\t\t PcieSegment: 0x%x\n", cpu_qpi_info->PcieSegment);
		printk(BIOS_DEBUG, "\t\t\t SegMmcfgBase: 0x%llx\n",
		       cpu_qpi_info->SegMmcfgBase.Data);
		printk(BIOS_DEBUG, "\t\t\t stackPresentBitmap: 0x%x\n",
		       cpu_qpi_info->stackPresentBitmap);
		printk(BIOS_DEBUG, "\t\t\t M2PciePresentBitmap: 0x%x\n",
		       cpu_qpi_info->M2PciePresentBitmap);
		printk(BIOS_DEBUG, "\t\t\t TotM3Kti: 0x%x\n", cpu_qpi_info->TotM3Kti);
		printk(BIOS_DEBUG, "\t\t\t TotCha: 0x%x\n", cpu_qpi_info->TotCha);
		for (int cha = 0; cha < BL_MAX_CHA_MAP; cha++) {
			printk(BIOS_DEBUG, "\t\t\t ChaList[%d]: 0x%x\n", cha,
			       cpu_qpi_info->ChaList[cha]);
		}
		printk(BIOS_DEBUG, "\t\t\t SocId: 0x%x\n", cpu_qpi_info->SocId);
		for (int peer = 0; peer < BL_MAX_FW_KTI_PORTS; peer++) {
			const BL_QPI_PEER_DATA *peer_info = &cpu_qpi_info->PeerInfo[peer];
			printk(BIOS_DEBUG, "\t\t\t PeerInfo[%d]\n", peer);
			printk(BIOS_DEBUG, "\t\t\t\t Valid: 0x%x\n", peer_info->Valid);
			printk(BIOS_DEBUG, "\t\t\t\t PeerSocId: 0x%x\n", peer_info->PeerSocId);
			printk(BIOS_DEBUG, "\t\t\t\t PeerSocType: 0x%x\n",
			       peer_info->PeerSocType);
			printk(BIOS_DEBUG, "\t\t\t\t PeerPort: 0x%x\n", peer_info->PeerPort);
		}
	}
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		const BL_QPI_IIO_DATA *iio_qpi_info = &hob->PlatformData.IioQpiInfo[socket];
		printk(BIOS_DEBUG, "\t\t IioQpiInfo[%d]\n", socket);
		printk(BIOS_DEBUG, "\t\t\t SocId: 0x%x\n", iio_qpi_info->SocId);
		for (int peer = 0; peer < BL_MAX_SOCKET; peer++) {
			const BL_QPI_PEER_DATA *peer_info = &iio_qpi_info->PeerInfo[peer];
			printk(BIOS_DEBUG, "\t\t\t PeerInfo[%d]\n", peer);
			printk(BIOS_DEBUG, "\t\t\t\t Valid: 0x%x\n", peer_info->Valid);
			printk(BIOS_DEBUG, "\t\t\t\t PeerSocId: 0x%x\n", peer_info->PeerSocId);
			printk(BIOS_DEBUG, "\t\t\t\t PeerSocType: 0x%x\n",
			       peer_info->PeerSocType);
			printk(BIOS_DEBUG, "\t\t\t\t PeerPort: 0x%x\n", peer_info->PeerPort);
		}
	}
	printk(BIOS_DEBUG, "\t\t MemTsegSize: 0x%x\n", hob->PlatformData.MemTsegSize);
	printk(BIOS_DEBUG, "\t\t MemIedSize: 0x%x\n", hob->PlatformData.MemIedSize);
	printk(BIOS_DEBUG, "\t\t PciExpressBase: 0x%llx\n", hob->PlatformData.PciExpressBase);
	printk(BIOS_DEBUG, "\t\t PciExpressSize: 0x%x\n", hob->PlatformData.PciExpressSize);
	printk(BIOS_DEBUG, "\t\t MemTolm: 0x%x\n", hob->PlatformData.MemTolm);
	for (uint8_t socket = 0; socket < hob->PlatformData.numofIIO; socket++) {
		const BL_IIO_RESOURCE_INSTANCE *iio_res =
			&hob->PlatformData.IIO_resource[socket];
		printk(BIOS_DEBUG, "\t\t IIO_resource[%d]\n", socket);
		printk(BIOS_DEBUG, "\t\t\t Valid: 0x%x\n", iio_res->Valid);
		printk(BIOS_DEBUG, "\t\t\t SocketID: 0x%x\n", iio_res->SocketID);
		printk(BIOS_DEBUG, "\t\t\t BusBase: 0x%x\n", iio_res->BusBase);
		printk(BIOS_DEBUG, "\t\t\t BusLimit: 0x%x\n", iio_res->BusLimit);
		printk(BIOS_DEBUG, "\t\t\t PciResourceIoBase: 0x%x\n",
		       iio_res->PciResourceIoBase);
		printk(BIOS_DEBUG, "\t\t\t PciResourceIoLimit: 0x%x\n",
		       iio_res->PciResourceIoLimit);
		printk(BIOS_DEBUG, "\t\t\t IoApicBase: 0x%x\n", iio_res->IoApicBase);
		printk(BIOS_DEBUG, "\t\t\t IoApicLimit: 0x%x\n", iio_res->IoApicLimit);
		printk(BIOS_DEBUG, "\t\t\t Mmio32Base: 0x%x\n", iio_res->Mmio32Base);
		printk(BIOS_DEBUG, "\t\t\t Mmio32Limit: 0x%x\n", iio_res->Mmio32Limit);
		printk(BIOS_DEBUG, "\t\t\t Mmio64Base: 0x%llx\n", iio_res->Mmio64Base);
		printk(BIOS_DEBUG, "\t\t\t Mmio64Limit: 0x%llx\n", iio_res->Mmio64Limit);
		for (int stack = 0; stack < BL_MAX_LOGIC_IIO_STACK; stack++) {
			const BL_STACK_RES *stack_res = &iio_res->StackRes[stack];
			printk(BIOS_DEBUG, "\t\t\t StackRes[%d]\n", stack);
			printk(BIOS_DEBUG, "\t\t\t\t Personality: 0x%x\n",
			       stack_res->Personality);
			printk(BIOS_DEBUG, "\t\t\t\t BusBase: 0x%x\n", stack_res->BusBase);
			printk(BIOS_DEBUG, "\t\t\t\t BusLimit: 0x%x\n", stack_res->BusLimit);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceIoBase: 0x%x\n",
			       stack_res->PciResourceIoBase);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceIoLimit: 0x%x\n",
			       stack_res->PciResourceIoLimit);
			printk(BIOS_DEBUG, "\t\t\t\t IoApicBase: 0x%x\n",
			       stack_res->IoApicBase);
			printk(BIOS_DEBUG, "\t\t\t\t IoApicLimit: 0x%x\n",
			       stack_res->IoApicLimit);
			printk(BIOS_DEBUG, "\t\t\t\t Mmio32Base: 0x%x\n",
			       stack_res->Mmio32Base);
			printk(BIOS_DEBUG, "\t\t\t\t Mmio32Limit: 0x%x\n",
			       stack_res->Mmio32Limit);
			printk(BIOS_DEBUG, "\t\t\t\t Mmio64Base: 0x%llx\n",
			       stack_res->Mmio64Base);
			printk(BIOS_DEBUG, "\t\t\t\t Mmio64Limit: 0x%llx\n",
			       stack_res->Mmio64Limit);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceMem32Base: 0x%x\n",
			       stack_res->PciResourceMem32Base);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceMem32Limit: 0x%x\n",
			       stack_res->PciResourceMem32Limit);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceMem64Base: 0x%llx\n",
			       stack_res->PciResourceMem64Base);
			printk(BIOS_DEBUG, "\t\t\t\t PciResourceMem64Limit: 0x%llx\n",
			       stack_res->PciResourceMem64Limit);
			printk(BIOS_DEBUG, "\t\t\t\t VtdBarAddress: 0x%x\n",
			       stack_res->VtdBarAddress);
			printk(BIOS_DEBUG, "\t\t\t\t Mmio32MinSize: 0x%x\n",
			       stack_res->Mmio32MinSize);
		}
		printk(BIOS_DEBUG, "\t\t\t RcBaseAddress: 0x%x\n", iio_res->RcBaseAddress);
		printk(BIOS_DEBUG, "\t\t\t PcieInfo\n");
		for (int port = 0; port < BL_NUMBER_PORTS_PER_SOCKET; port++) {
			const BL_IIO_PORT_INFO *port_info = &iio_res->PcieInfo.PortInfo[port];
			printk(BIOS_DEBUG, "\t\t\t\t PortInfo[%d]\n", port);
			printk(BIOS_DEBUG, "\t\t\t\t\t Device: 0x%x, Function: 0x%x\n",
			       port_info->Device, port_info->Function);
		}
		printk(BIOS_DEBUG, "\t\t\t DmaDeviceCount: 0x%x\n", iio_res->DmaDeviceCount);
	}
	printk(BIOS_DEBUG, "\t\t numofIIO: 0x%x\n", hob->PlatformData.numofIIO);
	printk(BIOS_DEBUG, "\t\t MaxBusNumber: 0x%x\n", hob->PlatformData.MaxBusNumber);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		printk(BIOS_DEBUG, "\t\t packageBspApicID[%d]: 0x%x\n", socket,
		       hob->PlatformData.packageBspApicID[socket]);
	}
	printk(BIOS_DEBUG, "\t\t EVMode: 0x%x\n", hob->PlatformData.EVMode);
	printk(BIOS_DEBUG, "\t\t Pci64BitResourceAllocation: %d\n",
	       hob->PlatformData.Pci64BitResourceAllocation);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		printk(BIOS_DEBUG, "\t\t SkuPersonality[%d]: 0x%x\n", socket,
		       hob->PlatformData.SkuPersonality[socket]);
	}
	for (int iio = 0; iio < BL_MaxIIO; iio++) {
		for (int iio_stack = 0; iio_stack < BL_MAX_IIO_STACK; iio_stack++) {
			printk(BIOS_DEBUG, "\t\t VMDStackEnable[%d][%d]: 0x%x\n", iio,
			       iio_stack, hob->PlatformData.VMDStackEnable[iio][iio_stack]);
		}
	}
	printk(BIOS_DEBUG, "\t\t IoGranularity: 0x%x\n", hob->PlatformData.IoGranularity);
	printk(BIOS_DEBUG, "\t\t MmiolGranularity: 0x%x\n", hob->PlatformData.MmiolGranularity);
	printk(BIOS_DEBUG, "\t\t MmiohGranularity: 0x%llx\n",
	       hob->PlatformData.MmiohGranularity.Data);
	printk(BIOS_DEBUG, "\t\t RemoteRequestThreshold: 0x%x\n",
	       hob->PlatformData.RemoteRequestThreshold);
	printk(BIOS_DEBUG, "\t\t UboxMmioSize: 0x%x\n", hob->PlatformData.UboxMmioSize);
	printk(BIOS_DEBUG, "\t\t MaxAddressBits: 0x%x\n", hob->PlatformData.MaxAddressBits);

	printk(BIOS_DEBUG, "\t System Status\n");
	printk(BIOS_DEBUG, "\t\t CurrentUpiiLinkSpeed: 0x%x\n",
	       hob->SystemStatus.CurrentUpiiLinkSpeed);
	printk(BIOS_DEBUG, "\t\t CurrentUpiLinkFrequency: 0x%x\n",
	       hob->SystemStatus.CurrentUpiLinkFrequency);
	printk(BIOS_DEBUG, "\t\t OutKtiCpuSktHotPlugEn: 0x%x\n",
	       hob->SystemStatus.OutKtiCpuSktHotPlugEn);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++)
		printk(BIOS_DEBUG, "\t\t OutKtiPerLinkL1En: 0x%x\n",
		       hob->SystemStatus.OutKtiPerLinkL1En[socket]);
	printk(BIOS_DEBUG, "\t\t IsocEnable: 0x%x\n", hob->SystemStatus.IsocEnable);
	printk(BIOS_DEBUG, "\t\t meRequestedSize: 0x%x\n", hob->SystemStatus.meRequestedSize);
	printk(BIOS_DEBUG, "\t\t ieRequestedSize: 0x%x\n", hob->SystemStatus.ieRequestedSize);
	printk(BIOS_DEBUG, "\t\t DmiVc1: 0x%x\n", hob->SystemStatus.DmiVc1);
	printk(BIOS_DEBUG, "\t\t DmiVcm: 0x%x\n", hob->SystemStatus.DmiVcm);
	printk(BIOS_DEBUG, "\t\t CpuPCPSInfo: 0x%x\n", hob->SystemStatus.CpuPCPSInfo);
	printk(BIOS_DEBUG, "\t\t cpuSubType: 0x%x\n", hob->SystemStatus.cpuSubType);
	printk(BIOS_DEBUG, "\t\t SystemRasType: 0x%x\n", hob->SystemStatus.SystemRasType);
	printk(BIOS_DEBUG, "\t\t numCpus: 0x%x\n", hob->SystemStatus.numCpus);
	printk(BIOS_DEBUG, "\t\t tolmLimit: 0x%x\n", hob->SystemStatus.tolmLimit);
	printk(BIOS_DEBUG, "\t\t tohmLimit: 0x%x\n", hob->SystemStatus.tohmLimit);
	printk(BIOS_DEBUG, "\t\t RcVersion\n");
	printk(BIOS_DEBUG, "\t\t\t Major: 0x%x\n", hob->SystemStatus.RcVersion.Major);
	printk(BIOS_DEBUG, "\t\t\t Minor: 0x%x\n", hob->SystemStatus.RcVersion.Minor);
	printk(BIOS_DEBUG, "\t\t\t Revision: 0x%x\n", hob->SystemStatus.RcVersion.Revision);
	printk(BIOS_DEBUG, "\t\t\t BuildNumber: 0x%x\n",
	       hob->SystemStatus.RcVersion.BuildNumber);
	printk(BIOS_DEBUG, "\t\t MsrTraceEnable: 0x%x\n", hob->SystemStatus.MsrTraceEnable);
	printk(BIOS_DEBUG, "\t\t DdrXoverMode: 0x%x\n", hob->SystemStatus.DdrXoverMode);
	printk(BIOS_DEBUG, "\t\t bootMode: 0x%x\n", hob->SystemStatus.bootMode);
	printk(BIOS_DEBUG, "\t\t OutClusterOnDieEn: 0x%x\n",
	       hob->SystemStatus.OutClusterOnDieEn);
	printk(BIOS_DEBUG, "\t\t OutSncEn: 0x%x\n", hob->SystemStatus.OutSncEn);
	printk(BIOS_DEBUG, "\t\t OutNumOfCluster: 0x%x\n", hob->SystemStatus.OutNumOfCluster);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		for (int imc = 0; imc < BL_MAX_IMC; imc++) {
			printk(BIOS_DEBUG, "\t\t imcEnabled[%d][%d]: 0x%x\n", socket, imc,
			       hob->SystemStatus.imcEnabled[socket][imc]);
		}
	}
	printk(BIOS_DEBUG, "\t\t LlcSizeReg: 0x%x\n", hob->SystemStatus.LlcSizeReg);
	for (int socket = 0; socket < BL_MAX_SOCKET; socket++) {
		for (int ch = 0; ch < BL_MAX_CH; ch++) {
			printk(BIOS_DEBUG, "\t\t chEnabled[%d][%d]: 0x%x\n", socket, ch,
			       hob->SystemStatus.chEnabled[socket][ch]);
		}
	}
	for (int node = 0; node < BL_MC_MAX_NODE; node++) {
		printk(BIOS_DEBUG, "\t\t memNode[%d]: 0x%x\n", node,
		       hob->SystemStatus.memNode[node]);
	}
	printk(BIOS_DEBUG, "\t\t IoDcMode: 0x%x\n", hob->SystemStatus.IoDcMode);
	printk(BIOS_DEBUG, "\t\t DfxRstCplBitsEn: 0x%x\n", hob->SystemStatus.DfxRstCplBitsEn);
}

static void soc_display_fsp_fia_override_status(const BL_FIA_OVERRIDE_STATUS_HOB *hob)
{
	if (!hob)
		return;
	printk(BIOS_DEBUG, "FIA Override Status\n");
	printk(BIOS_DEBUG, "\t FiaMuxConfigGetStatus: 0x%08x\n", hob->FiaMuxConfigGetStatus);
	printk(BIOS_DEBUG, "\t FiaMuxConfigSetStatus: 0x%08x\n", hob->FiaMuxConfigSetStatus);
	printk(BIOS_DEBUG, "\t FiaMuxConfigSetRequired: 0x%08x\n",
	       hob->FiaMuxConfigSetRequired);
}

static void soc_display_fsp_smbios_memory_info(const FSP_SMBIOS_MEMORY_INFO *hob)
{
	if (!hob)
		return;

	printk(BIOS_DEBUG, "SMBIOS Memory Info\n");
	printk(BIOS_DEBUG, "\t Revision: 0x%x\n", hob->Revision);
	printk(BIOS_DEBUG, "\t DataWidth: 0x%x\n", hob->DataWidth);
	printk(BIOS_DEBUG, "\t MemoryType: 0x%x\n", hob->MemoryType);
	printk(BIOS_DEBUG, "\t MemoryFrequencyInMHz: 0x%x\n", hob->MemoryFrequencyInMHz);
	printk(BIOS_DEBUG, "\t ErrorCorrectionType: 0x%x\n", hob->ErrorCorrectionType);
	printk(BIOS_DEBUG, "\t ChannelCount: 0x%x\n", hob->ChannelCount);
	for (uint8_t channel = 0; channel < hob->ChannelCount; channel++) {
		const CHANNEL_INFO *channel_info = &hob->ChannelInfo[channel];
		printk(BIOS_DEBUG, "\t ChannelInfo[%d]\n", channel);
		printk(BIOS_DEBUG, "\t\t ChannelId: 0x%x\n", channel_info->ChannelId);
		printk(BIOS_DEBUG, "\t\t DimmCount: 0x%x\n", channel_info->DimmCount);
		for (uint8_t dimm = 0; dimm < channel_info->DimmCount; dimm++) {
			const DIMM_INFO *dimm_info = &channel_info->DimmInfo[dimm];
			printk(BIOS_DEBUG, "\t\t\t DimmInfo[%d]\n", dimm);
			printk(BIOS_DEBUG, "\t\t\t\t DimmId: 0x%x\n", dimm_info->DimmId);
			printk(BIOS_DEBUG, "\t\t\t\t SizeInMb: 0x%x\n", dimm_info->SizeInMb);
			printk(BIOS_DEBUG, "\t\t\t\t MfgId: 0x%x\n", dimm_info->MfgId);
			printk(BIOS_DEBUG, "\t\t\t\t ModulePartNum: %s\n",
			       dimm_info->ModulePartNum);
		}
	}
}

void soc_display_hob(const struct hob_header *hob)
{
	uint8_t *guid;

	if (hob->type != HOB_TYPE_GUID_EXTENSION)
		return;

	guid = (uint8_t *)fsp_hob_header_to_resource(hob);
	if (fsp_guid_compare(guid, fsp_hob_iio_uds_data_guid.b))
		soc_display_fsp_iio_uds_data_hob(
			(const BL_IIO_UDS *)(guid + sizeof(fsp_hob_iio_uds_data_guid)));
	else if (fsp_guid_compare(guid, fsp_hob_fia_override_status_guid.b))
		soc_display_fsp_fia_override_status(
			(const BL_FIA_OVERRIDE_STATUS_HOB
				 *)(guid + sizeof(fsp_hob_fia_override_status_guid)));
	else if (fsp_guid_compare(guid, fsp_hob_smbios_memory_info_guid.b))
		soc_display_fsp_smbios_memory_info(
			(const FSP_SMBIOS_MEMORY_INFO
				 *)(guid + sizeof(fsp_hob_smbios_memory_info_guid)));
}
