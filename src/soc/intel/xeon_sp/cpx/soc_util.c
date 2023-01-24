/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>

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

bool is_iio_stack_res(const STACK_RES *res)
{
	return res->Personality == TYPE_UBOX_IIO;
}

uint8_t get_stack_busno(const uint8_t stack)
{
	if (stack >= MAX_IIO_STACK) {
		printk(BIOS_ERR, "%s: Stack %u does not exist!\n", __func__, stack);
		return 0;
	}
	const pci_devfn_t dev = PCI_DEV(UBOX_DECS_BUS, UBOX_DECS_DEV, UBOX_DECS_FUNC);
	const uint16_t offset = stack / 4 ? UBOX_DECS_CPUBUSNO1_CSR : UBOX_DECS_CPUBUSNO_CSR;
	return pci_io_read_config32(dev, offset) >> (8 * (stack % 4)) & 0xff;
}

uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack)
{
	const IIO_UDS *hob = get_iio_uds();

	assert(socket < hob->SystemStatus.numCpus && stack < MAX_LOGIC_IIO_STACK);

	return hob->PlatformData.IIO_resource[socket].StackRes[stack].BusBase;
}

uint32_t get_socket_ubox_busno(uint32_t socket)
{
	if (socket == 0)
		return get_stack_busno(PCU_IIO_STACK);

	return get_socket_stack_busno(socket, PCU_IIO_STACK);
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
