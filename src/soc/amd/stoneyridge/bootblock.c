/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <assert.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/mtrr.h>
#include <smp/node.h>
#include <bootblock_common.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <amdblocks/amd_pci_mmconf.h>
#include <amdblocks/biosram.h>
#include <amdblocks/iomap.h>
#include <amdblocks/post_codes.h>
#include <soc/pci_devs.h>
#include <soc/cpu.h>
#include <soc/southbridge.h>
#include <timestamp.h>
#include <halt.h>

#if CONFIG_PI_AGESA_TEMP_RAM_BASE < 0x100000
#error "Error: CONFIG_PI_AGESA_TEMP_RAM_BASE must be >= 1MB"
#endif
#if CONFIG_PI_AGESA_CAR_HEAP_BASE < 0x100000
#error "Error: CONFIG_PI_AGESA_CAR_HEAP_BASE must be >= 1MB"
#endif

/* Set the MMIO Configuration Base Address, Bus Range, and misc MTRRs. */
static void amd_initmmio(void)
{
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int mtrr;

	/*
	 * todo: AGESA currently writes variable MTRRs.  Once that is
	 *       corrected, un-hardcode this MTRR.
	 *
	 *       Be careful not to use get_free_var_mtrr/set_var_mtrr pairs
	 *       where all cores execute the path.  Both cores within a compute
	 *       unit share MTRRs.  Programming core0 has the appearance of
	 *       modifying core1 too.  Using the pair again will create
	 *       duplicate copies.
	 */
	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_FLASH;
	set_var_mtrr(mtrr, FLASH_BELOW_4GB_MAPPING_REGION_BASE,
		     FLASH_BELOW_4GB_MAPPING_REGION_SIZE, MTRR_TYPE_WRPROT);

	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_CAR_HEAP;
	set_var_mtrr(mtrr, CONFIG_PI_AGESA_CAR_HEAP_BASE,
			CONFIG_PI_AGESA_HEAP_SIZE, MTRR_TYPE_WRBACK);

	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - SOC_EARLY_VMTRR_TEMPRAM;
	set_var_mtrr(mtrr, CONFIG_PI_AGESA_TEMP_RAM_BASE,
			CONFIG_PI_AGESA_HEAP_SIZE, MTRR_TYPE_UNCACHEABLE);
}

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();
	amd_initmmio();
	/*
	 * Call lib/bootblock.c main with BSP, shortcut for APs
	 */
	if (!boot_cpu()) {
		void (*ap_romstage_entry)(void) =
				(void (*)(void))get_ap_entry_ptr();

		ap_romstage_entry(); /* execution does not return */
		halt();
	}

	/* TSC cannot be relied upon. Override the TSC value passed in. */
	bootblock_main_with_basetime(timestamp_get());
}

void bootblock_soc_early_init(void)
{
	bootblock_fch_early_init();
	post_code(POST_BOOTBLOCK_SOC_EARLY_INIT);
}

void bootblock_soc_init(void)
{
	if (CONFIG(AMD_SOC_CONSOLE_UART))
		assert(CONFIG_UART_FOR_CONSOLE >= 0
					&& CONFIG_UART_FOR_CONSOLE <= 1);

	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	bootblock_fch_init();
}
