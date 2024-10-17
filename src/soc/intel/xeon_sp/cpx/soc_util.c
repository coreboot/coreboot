/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <pc80/mc146818rtc.h>

const struct SystemMemoryMapHob *get_system_memory_map(void)
{
	size_t hob_size;
	const uint8_t mem_hob_guid[16] = FSP_SYSTEM_MEMORYMAP_HOB_GUID;
	const struct SystemMemoryMapHob **memmap_addr;

	memmap_addr = (const struct SystemMemoryMapHob **)
		fsp_find_extension_hob_by_guid(mem_hob_guid, &hob_size);
	/* hob_size is the size of the 8-byte address not the hob data */
	assert(memmap_addr && hob_size != 0);
	/* assert the pointer to the hob is not NULL */
	assert(*memmap_addr);

	return *memmap_addr;
}

bool is_pcie_iio_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX_IIO;
}

bool is_ubox_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX;
}

/*
 * EX: CPX-SP
 * Ports    Stack   Stack(HOB)  IioConfigIou
 * ==========================================
 * 0        CSTACK      stack 0     IOU0
 * 1A..1D   PSTACKZ     stack 1     IOU1
 * 2A..2D   PSTACK1     stack 2     IOU2
 * 3A..3D   PSTACK2     stack 4     IOU3
 */
int soc_get_stack_for_port(int port)
{
	if (port == PORT_0)
		return CSTACK;
	else if (port >= PORT_1A && port <= PORT_1D)
		return PSTACK0;
	else if (port >= PORT_2A && port <= PORT_2D)
		return PSTACK1;
	else if (port >= PORT_3A && port <= PORT_3D)
		return PSTACK2;
	else
		return -1;
}

uint8_t soc_get_iio_ioapicid(int socket, int stack)
{
	uint8_t ioapic_id = socket ? 0xf : 0x9;
	switch (stack) {
	case CSTACK:
		break;
	case PSTACK0:
		ioapic_id += 1;
		break;
	case PSTACK1:
		ioapic_id += 2;
		break;
	case PSTACK2:
		ioapic_id += 3;
		break;
	default:
		return 0xff;
	}
	return ioapic_id;
}

void soc_set_mrc_cold_boot_flag(bool cold_boot_required)
{
	uint8_t mrc_status = cmos_read(CMOS_OFFSET_MRC_STATUS);
	uint8_t new_mrc_status = (mrc_status & 0xfe) | cold_boot_required;
	printk(BIOS_SPEW, "MRC status: 0x%02x want 0x%02x\n", mrc_status, new_mrc_status);
	if (new_mrc_status != mrc_status) {
		cmos_write(new_mrc_status, CMOS_OFFSET_MRC_STATUS);
	}
}

void get_iiostack_info(struct iiostack_resource *info)
{
	const IIO_UDS *hob = get_iio_uds();

	// copy IIO Stack info from FSP HOB
	info->no_of_stacks = 0;
	for (int socket = 0, iio = 0; iio < hob->PlatformData.numofIIO; ++socket) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		iio++;
		for (int x = 0; x < MAX_IIO_STACK; ++x) {
			const STACK_RES *ri;
			ri = &hob->PlatformData.IIO_resource[socket].StackRes[x];
			if (!is_pcie_iio_stack_res(ri))
				continue;
			assert(info->no_of_stacks < (CONFIG_MAX_SOCKET * MAX_IIO_STACK));
			memcpy(&info->res[info->no_of_stacks++], ri, sizeof(STACK_RES));
		}
	}
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
