/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <hob_cxlnode.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <lib.h>
#include <soc/soc_util.h>

static const uint8_t fsp_hob_iio_uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
static const uint8_t fsp_hob_memmap_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
static const uint8_t fsp_hob_cxlnode_guid[16] = FSP_HOB_CXLNODE_GUID;

struct guid_name_map {
	const void *guid;
	const char *name;
};

static const struct guid_name_map guid_names[] = {
	{fsp_hob_iio_uds_guid, "FSP_HOB_IIO_UNIVERSAL_DATA_GUID"},
	{fsp_hob_memmap_guid, "FSP_SYSTEM_MEMORYMAP_HOB_GUID"},
	{fsp_hob_cxlnode_guid, "FSP_HOB_CXLNODE_GUID"},
};

const char *soc_get_guid_name(const uint8_t *guid)
{
	size_t index;

	/* Compare the GUID values in this module */
	for (index = 0; index < ARRAY_SIZE(guid_names); index++)
		if (fsp_guid_compare(guid, guid_names[index].guid))
			return guid_names[index].name;

	return NULL;
}

void soc_display_memmap_hob(const struct SystemMemoryMapHob **hob_addr)
{
	struct SystemMemoryMapHob *hob = (struct SystemMemoryMapHob *)*hob_addr;

	printk(BIOS_DEBUG, "================== MEMORY MAP HOB DATA ==================\n");
	printk(BIOS_DEBUG, "hob: %p, structure size: 0x%lx\n", hob, sizeof(*hob));

	printk(BIOS_DEBUG,
	       "\tlowMemBase: 0x%x, lowMemSize: 0x%x, highMemBase: 0x%x, "
	       "highMemSize: 0x%x\n",
	       hob->lowMemBase, hob->lowMemSize, hob->highMemBase, hob->highMemSize);
	printk(BIOS_DEBUG, "\tmemSize: 0x%x, memFreq: 0x%x\n", hob->memSize, hob->memFreq);

	printk(BIOS_DEBUG, "\tNumChPerMC: %d\n", hob->NumChPerMC);
	printk(BIOS_DEBUG, "\tSystemMemoryMapElement Entries: %d, entry size: %ld\n",
	       hob->numberEntries, sizeof(SYSTEM_MEMORY_MAP_ELEMENT));

	for (int e = 0; e < hob->numberEntries; ++e) {
		const struct SystemMemoryMapElement *mem_element = &hob->Element[e];
		printk(BIOS_DEBUG,
		       "\t\tmemory_map %d, SocketId: 0x%x, BaseAddress: 0x%x, ElementSize: 0x%x, Type: 0x%x\n",
		       e, mem_element->SocketId, mem_element->BaseAddress,
		       mem_element->ElementSize, mem_element->Type);
	}

	printk(BIOS_DEBUG,
	       "^^^  MEMMAP_SOCKET: %ld, ChannelDevice: %ld, MEMMAP_DIMM_DEVICE_INFO_STRUCT: %ld\n",
	       sizeof(MEMMAP_SOCKET), sizeof(struct ChannelDevice),
	       sizeof(MEMMAP_DIMM_DEVICE_INFO_STRUCT));
	printk(BIOS_DEBUG, "^^^  Element Offset: %ld\n",
	       offsetof(SYSTEM_MEMORY_MAP_HOB, Element));
	printk(BIOS_DEBUG, "^^^  Socket Offset: %ld\n",
	       offsetof(SYSTEM_MEMORY_MAP_HOB, Socket));
	printk(BIOS_DEBUG, "^^^  ChannelInfo Offset: %ld\n",
	       offsetof(MEMMAP_SOCKET, ChannelInfo));
	printk(BIOS_DEBUG, "^^^  DimmInfo Offset: %ld\n",
	       offsetof(struct ChannelDevice, DimmInfo));
	printk(BIOS_DEBUG, "^^^  DimmSize Offset: %ld\n",
	       offsetof(struct DimmDevice, DimmSize));

	for (int s = 0; s < MAX_SOCKET; ++s) {
		if (!hob->Socket[s].SocketEnabled)
			continue;
		for (int ch = 0; ch < MAX_CH; ++ch) {
			if (!hob->Socket[s].ChannelInfo[ch].Enabled)
				continue;
			for (int dimm = 0; dimm < MAX_DIMM; ++dimm) {
				if (!hob->Socket[s].ChannelInfo[ch].DimmInfo[dimm].Present)
					continue;
				printk(BIOS_DEBUG,
				       "\tsocket: %d, ch: %d, dimm: %d, enabled: %d, DimmSize: 0x%x\n",
				       s, ch, dimm,
				       hob->Socket[s].ChannelInfo[ch].DimmInfo[dimm].Enabled,
				       hob->Socket[s].ChannelInfo[ch].DimmInfo[dimm].DimmSize);
			}
		}
	}

	printk(BIOS_DEBUG, "\tBiosFisVersion: 0x%x\n", hob->BiosFisVersion);
	printk(BIOS_DEBUG, "\tMmiohBase: 0x%x\n", hob->MmiohBase);

	hexdump(hob, sizeof(*hob));
}

void soc_display_iio_universal_data_hob(const IIO_UDS *hob)
{
	printk(BIOS_DEBUG, "===================== IIO_UDS HOB DATA =====================\n");
	printk(BIOS_DEBUG, "hob: %p, structure size: 0x%lx\n", hob, sizeof(*hob));

	printk(BIOS_DEBUG, "\t===================== SYSTEM STATUS =====================\n");
	printk(BIOS_DEBUG, "\tnumCpus: 0x%x\n", hob->SystemStatus.numCpus);
	printk(BIOS_DEBUG, "\ttolmLimit: 0x%x\n", hob->SystemStatus.tolmLimit);

	printk(BIOS_DEBUG, "\t===================== PLATFORM DATA =====================\n");
	printk(BIOS_DEBUG, "\tPlatGlobalIoBase: 0x%x\n", hob->PlatformData.PlatGlobalIoBase);
	printk(BIOS_DEBUG, "\tPlatGlobalIoLimit: 0x%x\n", hob->PlatformData.PlatGlobalIoLimit);
	printk(BIOS_DEBUG, "\tPlatGlobalMmio32Base: 0x%x\n",
	       hob->PlatformData.PlatGlobalMmio32Base);
	printk(BIOS_DEBUG, "\tPlatGlobalMmio32Limit: 0x%x\n",
	       hob->PlatformData.PlatGlobalMmio32Limit);
	printk(BIOS_DEBUG, "\tPlatGlobalMmio64Base: 0x%llx\n",
	       hob->PlatformData.PlatGlobalMmio64Base);
	printk(BIOS_DEBUG, "\tPlatGlobalMmio64Limit: 0x%llx\n",
	       hob->PlatformData.PlatGlobalMmio64Limit);
	printk(BIOS_DEBUG, "\tMemTsegSize: 0x%x\n", hob->PlatformData.MemTsegSize);
	printk(BIOS_DEBUG, "\tPciExpressBase: 0x%llx\n", hob->PlatformData.PciExpressBase);
	printk(BIOS_DEBUG, "\tPciExpressSize: 0x%x\n", hob->PlatformData.PciExpressSize);
	printk(BIOS_DEBUG, "\tMemTolm: 0x%x\n", hob->PlatformData.MemTolm);
	printk(BIOS_DEBUG, "\tnumofIIO: 0x%x\n", hob->PlatformData.numofIIO);
	printk(BIOS_DEBUG, "\tMaxBusNumber: 0x%x\n", hob->PlatformData.MaxBusNumber);
	printk(BIOS_DEBUG, "\tIoGranularity: 0x%x\n", hob->PlatformData.IoGranularity);
	printk(BIOS_DEBUG, "\tMmiolGranularity: 0x%x\n", hob->PlatformData.MmiolGranularity);
	printk(BIOS_DEBUG, "\tMmiohGranularity: hi: 0x%x, lo:0x%x\n",
	       hob->PlatformData.MmiohGranularity.hi, hob->PlatformData.MmiohGranularity.lo);

	for (uint8_t s = 0; s < hob->PlatformData.numofIIO; ++s) {
		printk(BIOS_DEBUG, "\t============ Socket %d Info ================\n", s);
		printk(BIOS_DEBUG, "\tValid: 0x%x\n", hob->PlatformData.IIO_resource[s].Valid);
		printk(BIOS_DEBUG, "\tSocketID: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].SocketID);
		printk(BIOS_DEBUG, "\tBusBase: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].BusBase);
		printk(BIOS_DEBUG, "\tBusLimit: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].BusLimit);
		printk(BIOS_DEBUG, "\tPciResourceIoBase: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].PciResourceIoBase);
		printk(BIOS_DEBUG, "\tPciResourceIoLimit: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].PciResourceIoLimit);
		printk(BIOS_DEBUG, "\tIoApicBase: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].IoApicBase);
		printk(BIOS_DEBUG, "\tIoApicLimit: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].IoApicLimit);
		printk(BIOS_DEBUG, "\tMmio32Base: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].Mmio32Base);
		printk(BIOS_DEBUG, "\tMmio32Limit: 0x%x\n",
		       hob->PlatformData.IIO_resource[s].Mmio32Limit);
		printk(BIOS_DEBUG, "\tMmio64Base: 0x%llx\n",
		       hob->PlatformData.IIO_resource[s].Mmio64Base);
		printk(BIOS_DEBUG, "\tMmio64Limit: 0x%llx\n",
		       hob->PlatformData.IIO_resource[s].Mmio64Limit);

		printk(BIOS_DEBUG, "\t============ Stack Info ================\n");
		for (int x = 0; x < MAX_LOGIC_IIO_STACK; ++x) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			printk(BIOS_DEBUG, "\t\t========== Stack %d ===============\n", x);
			printk(BIOS_DEBUG, "\t\tPersonality: 0x%x\n", ri->Personality);
			printk(BIOS_DEBUG, "\t\tBusBase: 0x%x\n", ri->BusBase);
			printk(BIOS_DEBUG, "\t\tBusLimit: 0x%x\n", ri->BusLimit);
			printk(BIOS_DEBUG, "\t\tIoBase: 0x%x\n", ri->IoBase);
			printk(BIOS_DEBUG, "\t\tIoLimit: 0x%x\n", ri->IoLimit);
			printk(BIOS_DEBUG, "\t\tPciResourceIoBase: 0x%x\n",
			       ri->PciResourceIoBase);
			printk(BIOS_DEBUG, "\t\tPciResourceIoLimit: 0x%x\n",
			       ri->PciResourceIoLimit);
			printk(BIOS_DEBUG, "\t\tIoApicBase: 0x%x\n", ri->IoApicBase);
			printk(BIOS_DEBUG, "\t\tIoApicLimit: 0x%x\n", ri->IoApicLimit);
			printk(BIOS_DEBUG, "\t\tMmio32Base: 0x%x\n", ri->Mmio32Base);
			printk(BIOS_DEBUG, "\t\tMmio32Limit: 0x%x\n", ri->Mmio32Limit);
			printk(BIOS_DEBUG, "\t\tPciResourceMem32Base: 0x%x\n",
			       ri->PciResourceMem32Base);
			printk(BIOS_DEBUG, "\t\tPciResourceMem32Limit: 0x%x\n",
			       ri->PciResourceMem32Limit);
			printk(BIOS_DEBUG, "\t\tMmio64Base: 0x%llx\n", ri->Mmio64Base);
			printk(BIOS_DEBUG, "\t\tMmio64Limit: 0x%llx\n", ri->Mmio64Limit);
			printk(BIOS_DEBUG, "\t\tPciResourceMem64Base: 0x%llx\n",
			       ri->PciResourceMem64Base);
			printk(BIOS_DEBUG, "\t\tPciResourceMem64Limit: 0x%llx\n",
			       ri->PciResourceMem64Limit);
			printk(BIOS_DEBUG, "\t\tVtdBarAddress: 0x%x\n", ri->VtdBarAddress);
		}

		printk(BIOS_DEBUG, "\t============ PcieInfo ================\n");
		IIO_RESOURCE_INSTANCE iio_resource = hob->PlatformData.IIO_resource[s];
		for (int p = 0; p < NUMBER_PORTS_PER_SOCKET; ++p) {
			printk(BIOS_DEBUG, "\t\tPort: %d, Device: 0x%x, Function: 0x%x\n", p,
			       iio_resource.PcieInfo.PortInfo[p].Device,
			       iio_resource.PcieInfo.PortInfo[p].Function);
		}
	}

	hexdump(hob, sizeof(*hob));
}

static void soc_display_cxlnode_hob(const CXL_NODE_SOCKET *hob)
{
	printk(BIOS_DEBUG, "===================== CXLNODE HOB DATA =====================\n");
	printk(BIOS_DEBUG, "hob: %p, structure size: 0x%lx\n", hob, sizeof(*hob) * MAX_SOCKET);

	for (uint8_t skt_id = 0; skt_id < MAX_SOCKET; skt_id++) {
		printk(BIOS_DEBUG, "\tSocket:%d, CxlNodeCount: 0x%x\n", skt_id,
		       hob[skt_id].CxlNodeCount);
		for (uint8_t i = 0; i < hob[skt_id].CxlNodeCount; ++i) {
			printk(BIOS_DEBUG, "\tCxlNodeInfo[%d]:\n", i);
			const CXL_NODE_INFO node = hob[skt_id].CxlNodeInfo[i];
			printk(BIOS_DEBUG, "\t\tSerialNumber: 0x%llx\n", node.SerialNumber);
			printk(BIOS_DEBUG, "\t\tVendorId: 0x%x\n", node.VendorId);
			printk(BIOS_DEBUG, "\t\tAttr: 0x%x\n", node.Attr);
			printk(BIOS_DEBUG, "\t\tAddress: 0x%x\n", node.Address);
			printk(BIOS_DEBUG, "\t\tSize: 0x%x\n", node.Size);
			printk(BIOS_DEBUG, "\t\tWays: 0x%x\n", node.Ways);
			printk(BIOS_DEBUG, "\t\tSocketBitmap: 0x%x\n", node.SocketBitmap);
			printk(BIOS_DEBUG,
			       "\t\tPerfData format: RdLatency(0.1ns), WrLatency(0.1ns), RdBW(100MB/s), WrBW(100MB/s)\n");
			printk(BIOS_DEBUG,
			       "\t\tEfiMemType and PerfData are invalid for SPR-SP.\n");
		}
	}

	hexdump(hob, sizeof(*hob) * MAX_SOCKET);
}

void soc_display_hob(const struct hob_header *hob)
{
	uint8_t *guid;

	if (hob->type != HOB_TYPE_GUID_EXTENSION)
		return;

	guid = (uint8_t *)fsp_hob_header_to_resource(hob);

	if (fsp_guid_compare(guid, fsp_hob_iio_uds_guid))
		soc_display_iio_universal_data_hob((const IIO_UDS *)(guid + 16));
	else if (fsp_guid_compare(guid, fsp_hob_memmap_guid))
		soc_display_memmap_hob((const struct SystemMemoryMapHob **)(guid + 16));
	else if (fsp_guid_compare(guid, fsp_hob_cxlnode_guid))
		soc_display_cxlnode_hob((const CXL_NODE_SOCKET *)(guid + 16));
}
