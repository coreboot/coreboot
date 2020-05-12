/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <assert.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/soc_util.h>

static const uint8_t fsp_hob_iio_uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
static const uint8_t fsp_hob_memmap_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;

struct guid_name_map {
	const void *guid;
	const char *name;
};

static const struct guid_name_map  guid_names[] = {
	{ fsp_hob_iio_uds_guid, "FSP_HOB_IIO_UNIVERSAL_DATA_GUID" },
	{ fsp_hob_memmap_guid, "FSP_SYSTEM_MEMORYMAP_HOB_GUID" },
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

void soc_display_hob(const struct hob_header *hob)
{
	const struct hob_resource *res;

	res = fsp_hob_header_to_resource(hob);
	assert(res != NULL);
	printk(BIOS_DEBUG, "\tResource type: 0x%x, attribute: 0x%x, addr: 0x%08llx, len: 0x%08llx\n",
		res->type, res->attribute_type, res->addr, res->length);
	printk(BIOS_DEBUG, "\tOwner GUID: ");
	fsp_print_guid(res->owner_guid);
	printk(BIOS_DEBUG, " (%s)\n", fsp_get_guid_name(res->owner_guid));

	if (fsp_guid_compare(res->owner_guid, fsp_hob_iio_uds_guid))
		soc_display_iio_universal_data_hob();
	else if (fsp_guid_compare(res->owner_guid, fsp_hob_memmap_guid))
		soc_display_memmap_hob();
}

void soc_display_memmap_hob(void)
{
	size_t hob_size;
	const struct SystemMemoryMapHob *hob;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;

	hob = (const struct SystemMemoryMapHob *)fsp_find_extension_hob_by_guid(
		mem_hob_guid, &hob_size);
	assert(hob != NULL);

	printk(BIOS_DEBUG, "===================== MEMORY MAP HOB DATA =====================\n");
	printk(BIOS_DEBUG, "hob: %p\n", hob);
	printk(BIOS_DEBUG, "\tlowMemBase: 0x%x, lowMemSize: 0x%x, highMemBase: 0x%x, "
		"highMemSize: 0x%x\n",
		hob->lowMemBase, hob->lowMemSize, hob->highMemBase, hob->highMemSize);
	printk(BIOS_DEBUG, "\tmemSize: 0x%x, memFreq: 0x%x\n",
		hob->memSize, hob->memFreq);

	printk(BIOS_DEBUG, "\tSystemMemoryMapElement Entries: %d\n", hob->numberEntries);
	for (int e = 0; e < hob->numberEntries; ++e) {
		const struct SystemMemoryMapElement *mem_element = &hob->Element[e];
		printk(BIOS_DEBUG, "\t\tmemory_map %d BaseAddress: 0x%x, ElementSize: 0x%x, Type: 0x%x\n",
			e, mem_element->BaseAddress,
			mem_element->ElementSize, mem_element->Type);
	}
}

void soc_display_iio_universal_data_hob(void)
{
	size_t hob_size;
	const IIO_UDS *hob;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;

	hob = (const IIO_UDS *)fsp_find_extension_hob_by_guid(
		fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL);

	printk(BIOS_DEBUG, "===================== IIO_UDS HOB DATA =====================\n");

	printk(BIOS_DEBUG, "\t===================== SYSTEM STATUS =====================\n");
	printk(BIOS_DEBUG, "\tnumCpus: 0x%x\n", hob->SystemStatus.numCpus);
	printk(BIOS_DEBUG, "\ttolmLimit: 0x%x\n", hob->SystemStatus.tolmLimit);
	printk(BIOS_DEBUG, "\ttohmLimit: 0x%x\n", hob->SystemStatus.tohmLimit);

	printk(BIOS_DEBUG, "\t===================== PLATFORM DATA =====================\n");
	printk(BIOS_DEBUG, "\tPlatGlobalIoBase: 0x%x\n", hob->PlatformData.PlatGlobalIoBase);
	printk(BIOS_DEBUG, "\tPlatGlobalIoLimit: 0x%x\n", hob->PlatformData.PlatGlobalIoLimit);
	printk(BIOS_DEBUG, "\tPlatGlobalMmiolBase: 0x%x\n",
		hob->PlatformData.PlatGlobalMmiolBase);
	printk(BIOS_DEBUG, "\tPlatGlobalMmiolLimit: 0x%x\n",
		hob->PlatformData.PlatGlobalMmiolLimit);
	printk(BIOS_DEBUG, "\tPlatGlobalMmiohBase: 0x%llx\n",
		hob->PlatformData.PlatGlobalMmiohBase);
	printk(BIOS_DEBUG, "\tPlatGlobalMmiohLimit: 0x%llx\n",
		hob->PlatformData.PlatGlobalMmiohLimit);
	printk(BIOS_DEBUG, "\tMemTsegSize: 0x%x\n", hob->PlatformData.MemTsegSize);
	printk(BIOS_DEBUG, "\tMemIedSize: 0x%x\n", hob->PlatformData.MemIedSize);
	printk(BIOS_DEBUG, "\tPciExpressBase: 0x%llx\n", hob->PlatformData.PciExpressBase);
	printk(BIOS_DEBUG, "\tPciExpressSize: 0x%x\n", hob->PlatformData.PciExpressSize);
	printk(BIOS_DEBUG, "\tMemTolm: 0x%x\n", hob->PlatformData.MemTolm);
	printk(BIOS_DEBUG, "\tnumofIIO: 0x%x\n", hob->PlatformData.numofIIO);
	printk(BIOS_DEBUG, "\tMaxBusNumber: 0x%x\n", hob->PlatformData.MaxBusNumber);
	printk(BIOS_DEBUG, "\tIoGranularity: 0x%x\n", hob->PlatformData.IoGranularity);
	printk(BIOS_DEBUG, "\tMmiolGranularity: 0x%x\n", hob->PlatformData.MmiolGranularity);
	printk(BIOS_DEBUG, "\tMmiohGranularity: hi: 0x%x, lo:0x%x\n",
		hob->PlatformData.MmiohGranularity.hi, hob->PlatformData.MmiohGranularity.lo);
	printk(BIOS_DEBUG, "\tPci64BitResourceAllocation: %d\n",
		hob->PlatformData.Pci64BitResourceAllocation);

	for (uint8_t s = 0; s < hob->PlatformData.numofIIO; ++s) {
		printk(BIOS_DEBUG, "\t============ Socket %d Info ================\n", s);
		printk(BIOS_DEBUG, "\tValid: 0x%x\n",
			hob->PlatformData.IIO_resource[s].Valid);
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
		printk(BIOS_DEBUG, "\tPciResourceMem32Base: 0x%x\n",
			hob->PlatformData.IIO_resource[s].PciResourceMem32Base);
		printk(BIOS_DEBUG, "\tPciResourceMem32Limit: 0x%x\n",
			hob->PlatformData.IIO_resource[s].PciResourceMem32Limit);
		printk(BIOS_DEBUG, "\tPciResourceMem64Base: 0x%llx\n",
			hob->PlatformData.IIO_resource[s].PciResourceMem64Base);
		printk(BIOS_DEBUG, "\tPciResourceMem64Limit: 0x%llx\n",
			hob->PlatformData.IIO_resource[s].PciResourceMem64Limit);

		printk(BIOS_DEBUG, "\t============ Stack Info ================\n");
		for (int x = 0; x < MAX_LOGIC_IIO_STACK; ++x) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			printk(BIOS_DEBUG, "\t\t========== Stack %d ===============\n", x);
			printk(BIOS_DEBUG, "\t\tPersonality: 0x%x\n", ri->Personality);
			printk(BIOS_DEBUG, "\t\tBusBase: 0x%x\n", ri->BusBase);
			printk(BIOS_DEBUG, "\t\tBusLimit: 0x%x\n", ri->BusLimit);
			printk(BIOS_DEBUG, "\t\tPciResourceIoBase: 0x%x\n",
				ri->PciResourceIoBase);
			printk(BIOS_DEBUG, "\t\tPciResourceIoLimit: 0x%x\n",
				ri->PciResourceIoLimit);
			printk(BIOS_DEBUG, "\t\tIoApicBase: 0x%x\n", ri->IoApicBase);
			printk(BIOS_DEBUG, "\t\tIoApicLimit: 0x%x\n", ri->IoApicLimit);
			printk(BIOS_DEBUG, "\t\tPciResourceMem32Base: 0x%x\n",
				ri->PciResourceMem32Base);
			printk(BIOS_DEBUG, "\t\tPciResourceMem32Limit: 0x%x\n",
				ri->PciResourceMem32Limit);
			printk(BIOS_DEBUG, "\t\tPciResourceMem64Base: 0x%llx\n",
				ri->PciResourceMem64Base);
			printk(BIOS_DEBUG, "\t\tPciResourceMem64Limit: 0x%llx\n",
				ri->PciResourceMem64Limit);
			printk(BIOS_DEBUG, "\t\tVtdBarAddress: 0x%x\n", ri->VtdBarAddress);
		}

		printk(BIOS_DEBUG, "\t============ PcieInfo ================\n");
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[s];
		for (int p = 0; p < NUMBER_PORTS_PER_SOCKET; ++p) {
			printk(BIOS_DEBUG, "\t\tPort: %d, Device: 0x%x, Function: 0x%x\n",
				p, iio_resource.PcieInfo.PortInfo[p].Device,
				iio_resource.PcieInfo.PortInfo[p].Function);
		}
	}
}
