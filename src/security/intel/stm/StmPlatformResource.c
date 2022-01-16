/* SPDX-License-Identifier: BSD-2-Clause */

#include <stdint.h>
#include <security/intel/stm/StmApi.h>
#include <security/intel/stm/SmmStm.h>
#include <security/intel/stm/StmPlatformResource.h>

#if CONFIG(SOUTHBRIDGE_INTEL_COMMON_PMCLIB)
#include <southbridge/intel/common/pmutil.h>
#else
#include <soc/pm.h>
#endif
#include <cpu/x86/msr.h>
#include <console/console.h>

#define RDWR_ACCS 3
#define FULL_ACCS 7

// Fixed memory ranges
//
// TSEG memory!
static STM_RSC_MEM_DESC rsc_tseg_memory = {{MEM_RANGE, sizeof(STM_RSC_MEM_DESC)},
				    0,
				    0,
				    FULL_ACCS};

// Flash part
static STM_RSC_MEM_DESC rsc_spi_memory = {
				{MEM_RANGE, sizeof(STM_RSC_MEM_DESC)},
				0xFE000000,
				0x01000000,
				FULL_ACCS};

// ACPI
static STM_RSC_IO_DESC rsc_pm_io = {{IO_RANGE, sizeof(STM_RSC_IO_DESC)}, 0, 128};

// PCIE MMIO
static STM_RSC_MMIO_DESC rsc_pcie_mmio = {{MMIO_RANGE, sizeof(STM_RSC_MMIO_DESC)},
				   0,
				   0, // Length
				   RDWR_ACCS};

// Local APIC
static STM_RSC_MMIO_DESC rsc_apic_mmio = {{MMIO_RANGE, sizeof(STM_RSC_MMIO_DESC)},
				   0,
				   0x400,
				   RDWR_ACCS};

// Software SMI
static STM_RSC_TRAPPED_IO_DESC rsc_sw_smi_trap_io = {
				{TRAPPED_IO_RANGE, sizeof(STM_RSC_TRAPPED_IO_DESC)},
				0xB2,
				2};

// End of list
static STM_RSC_END rsc_list_end __attribute__((used)) = {
			{END_OF_RESOURCES, sizeof(STM_RSC_END)}, 0};

// Common PCI devices
//
// LPC bridge
STM_RSC_PCI_CFG_DESC rsc_lpc_bridge_pci = {
	{PCI_CFG_RANGE, sizeof(STM_RSC_PCI_CFG_DESC)},
	RDWR_ACCS,
	0,
	0,
	0x1000,
	0,
	0,
	{
		{1, 1, sizeof(STM_PCI_DEVICE_PATH_NODE), LPC_FUNCTION,
		 LPC_DEVICE},
	},
};

// Template for MSR resources.
STM_RSC_MSR_DESC rsc_msr_tpl = {
	{MACHINE_SPECIFIC_REG, sizeof(STM_RSC_MSR_DESC)},
};

// MSR indices to register
typedef struct {
	uint32_t msr_index;
	uint64_t read_mask;
	uint64_t write_mask;
} MSR_TABLE_ENTRY;

MSR_TABLE_ENTRY msr_table[] = {
	// Index Read Write
	// MASK64 means need access, MASK0 means no need access.
	{SMRR_PHYSBASE_MSR, MASK64, MASK0},
	{SMRR_PHYSMASK_MSR, MASK64, MASK0},
};

/*
 *  Fix up PCIE resource.
 */
static void fixup_pciex_resource(void)
{
	// Find max bus number and PCIEX length
	rsc_pcie_mmio.length = CONFIG_ECAM_MMCONF_LENGTH; // 0x10000000;// 256 MB
	rsc_pcie_mmio.base = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
}

/*
 *  Add basic resources to BIOS resource database.
 */
static void add_simple_resources(void)
{
	int Status = 0;
	msr_t ReadMsr;

	ReadMsr = rdmsr(SMRR_PHYSBASE_MSR);
	rsc_tseg_memory.base = ReadMsr.lo & 0xFFFFF000;

	ReadMsr = rdmsr(SMRR_PHYSMASK_MSR);
	rsc_tseg_memory.length = (~(ReadMsr.lo & 0xFFFFF000) + 1);

	rsc_pm_io.base = (uint16_t)get_pmbase();

	// Local APIC. We assume that all threads are programmed identically
	// despite that it is possible to have individual APIC address for
	// each of the threads. If this is the case this programming should
	// be corrected.
	ReadMsr = rdmsr(IA32_APIC_BASE_MSR_INDEX);
	rsc_apic_mmio.base = ((uint64_t)ReadMsr.lo & 0xFFFFF000) |
				((uint64_t)(ReadMsr.hi & 0x0000000F) << 32);

	// PCIEX BAR
	fixup_pciex_resource();

	Status |= add_pi_resource((void *)&rsc_tseg_memory, 1);
	Status |= add_pi_resource((void *)&rsc_spi_memory, 1);

	Status |= add_pi_resource((void *)&rsc_pm_io, 1);
	Status |= add_pi_resource((void *)&rsc_pcie_mmio, 1);
	Status |= add_pi_resource((void *)&rsc_apic_mmio, 1);
	Status |= add_pi_resource((void *)&rsc_sw_smi_trap_io, 1);

	Status |= add_pi_resource((void *)&rsc_lpc_bridge_pci, 1);

	if (Status != 0)
		printk(BIOS_DEBUG, "STM - Error in adding simple resources\n");
}

/*
 * Add MSR resources to BIOS resource database.
 */
static void add_msr_resources(void)
{
	uint32_t Status = 0;
	uint32_t Index;

	for (Index = 0; Index < ARRAY_SIZE(msr_table); Index++) {

		rsc_msr_tpl.msr_index = (uint32_t)msr_table[Index].msr_index;
		rsc_msr_tpl.read_mask = (uint64_t)msr_table[Index].read_mask;
		rsc_msr_tpl.write_mask = (uint64_t)msr_table[Index].write_mask;

		Status |= add_pi_resource((void *)&rsc_msr_tpl, 1);
	}

	if (Status != 0)
		printk(BIOS_DEBUG, "STM - Error in adding MSR resources\n");
}

/*
 * Add resources to BIOS resource database.
 */

extern uint8_t *m_stm_resources_ptr;

void add_resources_cmd(void)
{

	m_stm_resources_ptr = NULL;

	add_simple_resources();

	add_msr_resources();
}
