/* SPDX-License-Identifier: GPL-2.0-only */

#include "atf.h"
#include <arch/mmu.h>
#include <bootblock_common.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <symbols.h>
#include <arch/mmio.h>
#include <device/mmio.h>

void bootblock_mainboard_early_init(void)
{
}

void bootblock_mainboard_init(void)
{
	mmu_init();

	/* Everything below DRAM is device memory */
	mmu_config_range((void *)0, (uintptr_t)_dram, MA_DEV | MA_RW);
	mmu_config_range(_dram, 3ul * GiB, MA_MEM | MA_RW | MA_S);

	mmu_config_range(_ttb, REGION_SIZE(ttb), MA_MEM | MA_S | MA_RW);
	extern char _mm_comm_buffer[];
	extern char _emm_comm_buffer[];

	mmu_config_range(_mm_comm_buffer, (size_t)(_emm_comm_buffer - _mm_comm_buffer), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_bootblock, REGION_SIZE(bootblock), MA_MEM | MA_S | MA_RW);
	//	mmu_config_range(_romstage, REGION_SIZE(romstage), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_ramstage, REGION_SIZE(ramstage), MA_MEM | MA_S | MA_RW);

	//	mmu_config_range(_bl31, REGION_SIZE(bl31), MA_MEM | MA_NS | MA_RW);

	//	mmu_config_range((void *)CONFIG_ECAM_MMCONF_BASE_ADDRESS, CONFIG_ECAM_MMCONF_LENGTH,
	//		 MA_DEV | MA_RW);

	/* SoC peripherals: UART, SMpro/PMpro doorbells, ... */
	mmu_config_range((void *)AC01_DEVICE_MEMORY_S0_BASE, AC01_DEVICE_MEMORY_S0_SIZE,
			 MA_DEV | MA_RW);

	/* CSR and ECAM windows of the PCIe root complexes */
	static const uint64_t pcie_csr[] = { AC01_PCIE_CSR_BASE_LIST };
	for (size_t i = 0; i < ARRAY_SIZE(pcie_csr); i++)
		mmu_config_range((void *)pcie_csr[i], AC01_PCIE_CSR_SIZE, MA_DEV | MA_RW);

	mmu_enable();

}
