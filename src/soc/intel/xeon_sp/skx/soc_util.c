/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <hob_iiouds.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/cpu.h>
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

bool is_iio_stack_res(const STACK_RES *res)
{
	// TODO: do we have situation with only bux 0 and one stack?
	return res->BusBase < res->BusLimit;
}

uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack)
{
	const IIO_UDS *hob = get_iio_uds();

	assert(socket < MAX_SOCKET && stack < MAX_IIO_STACK);

	return hob->PlatformData.CpuQpiInfo[socket].StackBus[stack];
}

void config_reset_cpl3_csrs(void)
{
	uint32_t data, plat_info, max_min_turbo_limit_ratio;

	for (uint32_t socket = 0; socket < MAX_SOCKET; ++socket) {
		uint32_t bus = get_socket_stack_busno(socket, PCU_IIO_STACK);

		/* configure PCU_CR0_FUN csrs */
		pci_devfn_t cr0_dev = PCI_DEV(bus, PCU_DEV, PCU_CR0_FUN);
		data = pci_s_read_config32(cr0_dev, PCU_CR0_P_STATE_LIMITS);
		data |= P_STATE_LIMITS_LOCK;
		pci_s_write_config32(cr0_dev, PCU_CR0_P_STATE_LIMITS, data);

		plat_info = pci_s_read_config32(cr0_dev, PCU_CR0_PLATFORM_INFO);
		dump_csr64("", cr0_dev, PCU_CR0_PLATFORM_INFO);
		max_min_turbo_limit_ratio =
			(plat_info & MAX_NON_TURBO_LIM_RATIO_MASK) >>
				MAX_NON_TURBO_LIM_RATIO_SHIFT;
		printk(BIOS_SPEW, "plat_info: 0x%x, max_min_turbo_limit_ratio: 0x%x\n",
			plat_info, max_min_turbo_limit_ratio);

		/* configure PCU_CR1_FUN csrs */
		pci_devfn_t cr1_dev = PCI_DEV(bus, PCU_DEV, PCU_CR1_FUN);

		data = pci_s_read_config32(cr1_dev, PCU_CR1_SAPMCTL);
		/* clear bits 27:31 - FSP sets this with 0x7 which needs to be cleared */
		data &= 0x0fffffff;
		data |= SAPMCTL_LOCK_MASK;
		pci_s_write_config32(cr1_dev, PCU_CR1_SAPMCTL, data);

		/* configure PCU_CR1_FUN csrs */
		pci_devfn_t cr2_dev = PCI_DEV(bus, PCU_DEV, PCU_CR2_FUN);

		data = PCIE_IN_PKGCSTATE_L1_MASK;
		pci_s_write_config32(cr2_dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK, data);

		data = KTI_IN_PKGCSTATE_L1_MASK;
		pci_s_write_config32(cr2_dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK2, data);

		data = PROCHOT_RATIO;
		printk(BIOS_SPEW, "PCU_CR2_PROCHOT_RESPONSE_RATIO_REG data: 0x%x\n", data);
		pci_s_write_config32(cr2_dev, PCU_CR2_PROCHOT_RESPONSE_RATIO_REG, data);
		dump_csr("", cr2_dev, PCU_CR2_PROCHOT_RESPONSE_RATIO_REG);

		data = pci_s_read_config32(cr2_dev, PCU_CR2_DYNAMIC_PERF_POWER_CTL);
		data |= UNOCRE_PLIMIT_OVERRIDE_SHIFT;
		pci_s_write_config32(cr2_dev, PCU_CR2_DYNAMIC_PERF_POWER_CTL, data);
	}
}

/*
 * EX: SKX-SP
 * Ports    Stack   Stack(HOB)  IioConfigIou
 * ==========================================
 * 0        CSTACK      stack 0     IOU0
 * 1A..1D   PSTACKZ     stack 1     IOU1
 * 2A..2D   PSTACK1     stack 2     IOU2
 * 3A..3D   PSTACK2     stack 3     IOU3
 * 5A..4D   PSTACK3     stack 4     IOU4
 * 5A..5D   PSTACK4     stack 5     IOU5
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
	else if (port >= PORT_4A && port <= PORT_4D)
		return PSTACK3; // MCP0
	else if (port >= PORT_5A && port <= PORT_5D)
		return PSTACK4; // MCP1
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
