/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <hob_iiouds.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_util.h>
#include <soc/util.h>


/*
 *     +-------------------------+  TOLM
 *     | System Management Mode  |
 *     |      code and data      |
 *     |         (TSEG)          |
 *     +-------------------------+  SMM base (aligned)
 *     |                         |
 *     | Chipset Reserved Memory |
 *     |                         |
 *     +-------------------------+  top_of_ram (aligned)
 *     |                         |
 *     |       CBMEM Root        |
 *     |                         |
 *     +-------------------------+
 *     |                         |
 *     |   FSP Reserved Memory   |
 *     |                         |
 *     +-------------------------+
 *     |                         |
 *     |  Various CBMEM Entries  |
 *     |                         |
 *     +-------------------------+  top_of_stack (8 byte aligned)
 *     |                         |
 *     |   stack (CBMEM Entry)   |
 *     |                         |
 *     +-------------------------+
 */

const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
	const struct SystemMemoryMapHob *memmap_addr;

	memmap_addr = fsp_find_extension_hob_by_guid(mem_hob_guid, &hob_size);
	assert(memmap_addr && hob_size != 0);

	return memmap_addr;
}

bool is_pcie_iio_stack_res(const STACK_RES *res)
{
	return res->BusBase < res->BusLimit;
}

bool is_ubox_stack_res(const STACK_RES *res)
{
	/*
	 * Unlike on later platforms there's no separate "UBOX" stack.
	 *
	 * The UBOX devices can always be found on the first bus on the stack IIO0 (CSTACK).
	 * This bus is also referred to as uncore bus 0 or B(30).
	 * It has at a fixed address the UBOX:
	 * B(30):8.0 8086:2014
	 * B(30):8.1 8086:2015
	 * B(30):8.2 8086:2016
	 *
	 * The PCU devices can always be on the first bus of the stack IIO1 (PSTACK).
	 * This bus is also referred to as uncore bus 1 or B(31).
	 * It has at a fixed address the PCU:
	 * B(31):30.0 8086:2080
	 * B(31):30.1 8086:2081
	 * B(31):30.2 8086:2082
	 */

	return false;
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
	return true;
}

uint8_t get_cxl_node_count(void)
{
	return 0;
}

bool get_mmio_high_base_size(resource_t *base, resource_t *size)
{
	return false;
}
