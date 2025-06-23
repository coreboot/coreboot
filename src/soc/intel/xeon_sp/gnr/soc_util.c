/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <soc/util.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <stdlib.h>
#include <string.h>
#include <MemoryMapDataHob.h>

const char *pciroot_res_to_domain_type(const UDS_STACK_RES *sr, const UDS_PCIROOT_RES *pr)
{
	int index = 0;
	int instance = -1;

	for (; index < sr->PciRootBridgeNum; index++) {
		if (sr->PciRoot[index].UidType == pr->UidType)
			instance++;
		if (sr->PciRoot[index].BusBase == pr->BusBase)
			break;
	}

	if (index == sr->PciRootBridgeNum)
		return NULL;

	switch (pr->UidType) {
	case PC_UID:
		return DOMAIN_TYPE_PCIE;
	case DINO_UID:
		return DOMAIN_TYPE_DINO;
	case CPM0_UID:
		return DOMAIN_TYPE_CPM0;
	case HQM0_UID:
		return DOMAIN_TYPE_HQM0;
	case UB_UID:
		return (instance == 0) ? DOMAIN_TYPE_UBX0 : DOMAIN_TYPE_UBX1;
	default:
		return NULL;
	}
}

static bool is_domain_type_supported_on_stack(const xSTACK_RES *sr, const char *dt)
{
	for (unsigned int index = 0; index < sr->PciRootBridgeNum; index++)
		if (!strcmp(dt, pciroot_res_to_domain_type(sr, &sr->PciRoot[index])))
			return true;

	return false;
}

bool is_pcie_iio_stack_res(const xSTACK_RES *res)
{
	return is_domain_type_supported_on_stack(res, DOMAIN_TYPE_PCIE);
}

bool is_ioat_iio_stack_res(const xSTACK_RES *res)
{
	return (is_domain_type_supported_on_stack(res, DOMAIN_TYPE_DINO) ||
		is_domain_type_supported_on_stack(res, DOMAIN_TYPE_CPM0) ||
		is_domain_type_supported_on_stack(res, DOMAIN_TYPE_HQM0));

}

bool is_ubox_stack_res(const xSTACK_RES *res)
{
	return (is_domain_type_supported_on_stack(res, DOMAIN_TYPE_UBX0) ||
		is_domain_type_supported_on_stack(res, DOMAIN_TYPE_UBX1));
}

bool is_iio_cxl_stack_res(const xSTACK_RES *res)
{
	return false;
}

const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const EFI_GUID mem_hob_guid = MEMORY_MAP_HOB_GUID;
	const struct SystemMemoryMapHob **memmap_addr;

	memmap_addr = (const struct SystemMemoryMapHob **)
		fsp_find_extension_hob_by_guid((uint8_t *)&mem_hob_guid, &hob_size);
	/* hob_size is the size of the 8-byte address not the hob data */
	assert(memmap_addr != NULL && hob_size != 0);
	/* assert the pointer to the hob is not NULL */
	assert(*memmap_addr != NULL);

	return *memmap_addr;
}

const struct SystemMemoryMapElement *get_system_memory_map_elment(uint8_t *num)
{
	const struct SystemMemoryMapHob *hob = get_system_memory_map();
	if (!hob)
		return NULL;

	*num = hob->numberEntries;
	return hob->Element;
}

const CXL_NODE_SOCKET *get_cxl_node(void)
{
	size_t hob_size;
	static const CXL_NODE_SOCKET *hob;
	const EFI_GUID fsp_hob_cxl_node_socket_guid = CXL_NODE_HOB_GUID;

	if (hob != NULL)
		return hob;

	hob = fsp_find_extension_hob_by_guid((uint8_t *)&fsp_hob_cxl_node_socket_guid, &hob_size);
	if (hob == NULL || hob_size == 0) {
		printk(BIOS_DEBUG, "CXL_NODE_HOB_GUID not found: CXL may not be installed\n");
		return NULL;
	}
	return hob;
}

uint8_t get_cxl_node_count(void)
{
	const CXL_NODE_SOCKET *hob = get_cxl_node();
	uint8_t count = 0;

	if (hob == NULL)
		return 0;
	for (unsigned int skt_id = 0 ; skt_id < MAX_SOCKET; skt_id++)
		count += hob[skt_id].CxlNodeCount;

	return count;
}

unsigned int get_prmrr_count(void)
{
	uint32_t cpu_id = cpu_get_cpuid();

	switch (cpu_id & CPUID_ALL_STEPPINGS_MASK) {
	case CPUID_GRANITERAPIDS:
		return 0x7;
	case CPUID_SIERRAFOREST:
		return 0x4;
	default:
		return 0;
	}
}

bool is_memtype_reserved(uint16_t mem_type)
{
	return false;
}

bool is_memtype_non_volatile(uint16_t mem_type)
{
	return false;
}

bool is_memtype_processor_attached(uint16_t mem_type)
{
	return true;
}

bool get_mmio_high_base_size(resource_t *base, resource_t *size)
{
	const IIO_UDS *hob = get_iio_uds();
	*base = hob->PlatformData.SystemMmio64Base;
	*size = hob->PlatformData.SystemMmio64Limit - (*base) + 1;

	return true;
}
