/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/device.h>
#include <device/pci.h>
#include <hob_cxlnode.h>
#include <intelblocks/cpulib.h>
#include <soc/msr.h>
#include <soc/numa.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <pc80/mc146818rtc.h>

const EWL_PRIVATE_DATA *get_ewl_hob(void)
{
	size_t hob_size;
	static const EWL_PRIVATE_DATA *hob;
	const uint8_t ewl_id_hob_guid[16] = FSP_HOB_EWLID_GUID;

	if (hob != NULL)
		return hob;

	hob = fsp_find_extension_hob_by_guid(ewl_id_hob_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);
	return hob;
}

const SYSTEM_INFO_VAR *get_system_info_hob(void)
{
	size_t hob_size;
	static const SYSTEM_INFO_VAR *hob;
	const uint8_t system_info_hob_guid[16] = FSP_HOB_SYSTEMINFO_GUID;

	if (hob != NULL)
		return hob;

	hob = fsp_find_extension_hob_by_guid(system_info_hob_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);
	return hob;
}

const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
	const struct SystemMemoryMapHob **memmap_addr;

	memmap_addr = (const struct SystemMemoryMapHob **)fsp_find_extension_hob_by_guid(
		mem_hob_guid, &hob_size);
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

bool is_pcie_iio_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX_IIO;
}

bool is_ubox_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX;
}

bool is_ioat_iio_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_DINO;
}

/*
 * Given a stack resource, figure out whether the corresponding stack has
 * CXL device.
 * It goes through pds (proximity domains) structure to see if there is any
 * generic initiator has device with bus # falls between bus base and
 * bus limit.
 */
bool is_iio_cxl_stack_res(const STACK_RES *res)
{
	/* pds should be setup ahead of this call */
	assert(pds.num_pds);

	for (uint8_t i = 0; i < pds.num_pds; i++) {
		if (pds.pds[i].pd_type != PD_TYPE_GENERIC_INITIATOR)
			continue;

		uint32_t bus = PCI_BDF(pds.pds[i].dev) >> 20;
		if (bus >= res->BusBase && bus <= res->BusLimit)
			return true;
	}

	return false;
}

const CXL_NODE_SOCKET *get_cxl_node(void)
{
	size_t hob_size;
	static const CXL_NODE_SOCKET *hob;
	static bool hob_check = 0;
	const uint8_t fsp_hob_cxl_node_socket_guid[16] = FSP_HOB_CXLNODE_GUID;

	if (hob_check == 1)
		return hob;

	hob = fsp_find_extension_hob_by_guid(fsp_hob_cxl_node_socket_guid, &hob_size);
	hob_check = 1;
	if (hob == NULL || hob_size == 0)
		printk(BIOS_DEBUG,
		       "FSP_HOB_CXLNODE_GUID not found: CXL may not be installed\n");
	return hob;
}

uint8_t get_cxl_node_count(void)
{
	const CXL_NODE_SOCKET *hob = get_cxl_node();
	uint8_t count = 0;

	if (hob != NULL) {
		for (uint8_t skt_id = 0; skt_id < MAX_SOCKET; skt_id++)
			count += hob[skt_id].CxlNodeCount;
	}

	return count;
}

void bios_done_msr(void *unused)
{
	msr_t msr = rdmsr(MSR_BIOS_DONE);
	if (!(msr.lo & XEON_SP_ENABLE_IA_UNTRUSTED)) { /* if already locked skip update */
		msr.lo |= XEON_SP_ENABLE_IA_UNTRUSTED;
		wrmsr(MSR_BIOS_DONE, msr);
	}
}

void soc_set_mrc_cold_boot_flag(bool cold_boot_required)
{
	uint8_t mrc_status = cmos_read(CMOS_OFFSET_MRC_STATUS);
	uint8_t new_mrc_status = (mrc_status & 0xfe) | cold_boot_required;
	printk(BIOS_SPEW, "MRC status: 0x%02x want 0x%02x\n", mrc_status, new_mrc_status);
	if (new_mrc_status != mrc_status)
		cmos_write(new_mrc_status, CMOS_OFFSET_MRC_STATUS);
}

bool is_memtype_reserved(uint16_t mem_type)
{
	return !!(mem_type & MEM_TYPE_RESERVED);
}

bool is_memtype_non_volatile(uint16_t mem_type)
{
	return !(mem_type & MEMTYPE_VOLATILE_MASK);
}

bool is_memtype_processor_attached(uint16_t mem_type)
{
	/*
	 * Refer to the definition of MEM_TYPE enum type in
	 * vendorcode/intel/fsp/fsp2_0/sapphirerapids_sp/MemoryMapDataHob.h,
	 * values less than MemTypeCxlAccVolatileMem represents
	 * processor attached memory
	 */
	return (mem_type < MemTypeCxlAccVolatileMem);
}

unsigned int get_prmrr_count(void)
{
	return 0x7;
}

bool get_mmio_high_base_size(resource_t *base, resource_t *size)
{
	const IIO_UDS *hob = get_iio_uds();
	*base = hob->PlatformData.PlatGlobalMmio64Base;
	*size = hob->PlatformData.PlatGlobalMmio64Limit - (*base) + 1;

	return true;
}
