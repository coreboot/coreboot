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
#include <stdlib.h>
#include <string.h>
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

bool is_iio_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX_IIO || res->Personality == TYPE_DINO;
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
	for (uint8_t i = 0; i < pds.num_pds; i++) {
		if (pds.pds[i].pd_type == PD_TYPE_PROCESSOR)
			continue;

		uint32_t bus = pds.pds[i].device_handle >> 20;
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

uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack)
{
	const IIO_UDS *hob = get_iio_uds();

	assert(socket < hob->SystemStatus.numCpus && stack < MAX_LOGIC_IIO_STACK);

	return hob->PlatformData.IIO_resource[socket].StackRes[stack].BusBase;
}

uint32_t get_ubox_busno(uint32_t socket, uint8_t offset)
{
	const IIO_UDS *hob = get_iio_uds();

	assert(socket < hob->SystemStatus.numCpus);
	for (int stack = 0; stack < MAX_LOGIC_IIO_STACK; ++stack) {
		if (hob->PlatformData.IIO_resource[socket].StackRes[stack].Personality
		    == TYPE_UBOX)
			return (hob->PlatformData.IIO_resource[socket].StackRes[stack].BusBase
				+ offset);
	}
	die("Unable to locate UBOX BUS NO");
}

uint32_t get_socket_ubox_busno(uint32_t socket)
{
	return get_ubox_busno(socket, UNCORE_BUS_1);
}

void bios_done_msr(void *unused)
{
	msr_t msr = rdmsr(MSR_BIOS_DONE);
	if (!(msr.lo & XEON_SP_ENABLE_IA_UNTRUSTED)) { /* if already locked skip update */
		msr.lo |= XEON_SP_ENABLE_IA_UNTRUSTED;
		wrmsr(MSR_BIOS_DONE, msr);
	}
}

void set_cmos_mrc_cold_boot_flag(bool cold_boot_required)
{
	uint8_t mrc_status = cmos_read(CMOS_OFFSET_MRC_STATUS);
	uint8_t new_mrc_status = (mrc_status & 0xfe) | cold_boot_required;
	printk(BIOS_SPEW, "MRC status: 0x%02x want 0x%02x\n", mrc_status, new_mrc_status);
	if (new_mrc_status != mrc_status)
		cmos_write(new_mrc_status, CMOS_OFFSET_MRC_STATUS);

}
