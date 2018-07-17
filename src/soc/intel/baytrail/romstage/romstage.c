/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/early_variables.h>
#include <console/console.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
#include <ec/google/chromeec/ec.h>
#endif
#include <elog.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <string.h>
#include <timestamp.h>
#include <security/tpm/tspi.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <soc/smm.h>
#include <soc/spi.h>

/* The cache-as-ram assembly file calls romstage_main() after setting up
 * cache-as-ram.  romstage_main() will then call the mainboards's
 * mainboard_romstage_entry() function. That function then calls
 * romstage_common() below. The reason for the back and forth is to provide
 * common entry point from cache-as-ram while still allowing for code sharing.
 * Because we can't use global variables the stack is used for allocations --
 * thus the need to call back and forth. */

static void *setup_stack_and_mtrrs(void);

static void program_base_addresses(void)
{
	uint32_t reg;
	const uint32_t lpc_dev = PCI_DEV(0, LPC_DEV, LPC_FUNC);

	/* Memory Mapped IO registers. */
	reg = PMC_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PBASE, reg);
	reg = IO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IOBASE, reg);
	reg = ILB_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IBASE, reg);
	reg = SPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, SBASE, reg);
	reg = MPHY_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, MPBASE, reg);
	reg = PUNIT_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PUBASE, reg);
	reg = RCBA_BASE_ADDRESS | 1;
	pci_write_config32(lpc_dev, RCBA, reg);

	/* IO Port Registers. */
	reg = ACPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, ABASE, reg);
	reg = GPIO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, GBASE, reg);
}

static void spi_init(void)
{
	u32 *scs = (u32 *)(SPI_BASE_ADDRESS + SCS);
	u32 *bcr = (u32 *)(SPI_BASE_ADDRESS + BCR);
	uint32_t reg;

	/* Disable generating SMI when setting WPD bit. */
	write32(scs, read32(scs) & ~SMIWPEN);
	/*
	 * Enable caching and prefetching in the SPI controller. Disable
	 * the SMM-only BIOS write and set WPD bit.
	 */
	reg = (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH | BCR_WPD;
	reg &= ~EISS;
	write32(bcr, reg);
}

/* Entry from cache-as-ram.inc. */
void *asmlinkage romstage_main(unsigned long bist,
                                uint32_t tsc_low, uint32_t tsc_hi)
{
	struct romstage_params rp = {
		.bist = bist,
		.mrc_params = NULL,
	};

	/* Save initial timestamp from bootblock. */
	timestamp_init((((uint64_t)tsc_hi) << 32) | (uint64_t)tsc_low);

	/* Save romstage begin */
	timestamp_add_now(TS_START_ROMSTAGE);

	program_base_addresses();

	tco_disable();

	byt_config_com1_and_enable();

	console_init();

	spi_init();

	set_max_freq();

	punit_init();

	gfx_init();

	/* Call into mainboard. */
	mainboard_romstage_entry(&rp);

	return setup_stack_and_mtrrs();
}

static struct chipset_power_state power_state CAR_GLOBAL;

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;
	struct chipset_power_state *ps_car;

	ps_car = car_get_var_ptr(&power_state);
	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, ps_car, sizeof(*ps_cbmem));
}
ROMSTAGE_CBMEM_INIT_HOOK(migrate_power_state)

static struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	ps->gpe0_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS);
	ps->gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN);
	ps->tco_sts = inl(ACPI_BASE_ADDRESS + TCO_STS);
	ps->prsts = read32((u32 *)(PMC_BASE_ADDRESS + PRSTS));
	ps->gen_pmcon1 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON1));
	ps->gen_pmcon2 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON2));

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
		ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);
	printk(BIOS_DEBUG, "gpe0_sts: %08x gpe0_en: %08x tco_sts: %08x\n",
		ps->gpe0_sts, ps->gpe0_en, ps->tco_sts);
	printk(BIOS_DEBUG, "prsts: %08x gen_pmcon1: %08x gen_pmcon2: %08x\n",
		ps->prsts, ps->gen_pmcon1, ps->gen_pmcon2);

	return ps;
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
static int chipset_prev_sleep_state(struct chipset_power_state *ps)
{
	/* Default to S0. */
	int prev_sleep_state = ACPI_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(ps->pm1_cnt)) {
		case ACPI_S3:
			if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}
		/* Clear SLP_TYP. */
		outl(ps->pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
	}

	if (ps->gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR)) {
		prev_sleep_state = ACPI_S5;
	}

	return prev_sleep_state;
}

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	struct chipset_power_state *ps;
	int prev_sleep_state;

	timestamp_add_now(TS_BEFORE_INITRAM);

	ps = fill_power_state();
	prev_sleep_state = chipset_prev_sleep_state(ps);

	printk(BIOS_DEBUG, "prev_sleep_state = S%d\n", prev_sleep_state);

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	if (prev_sleep_state != ACPI_S3)
		boot_count_increment();
#endif


	/* Initialize RAM */
	raminit(params->mrc_params, prev_sleep_state);

	timestamp_add_now(TS_AFTER_INITRAM);

	romstage_handoff_init(prev_sleep_state == ACPI_S3);

	if (IS_ENABLED(CONFIG_TPM1) || IS_ENABLED(CONFIG_TPM2))
		tpm_setup(prev_sleep_state == ACPI_S3);
}

void asmlinkage romstage_after_car(void)
{
	/* Load the ramstage. */
	run_ramstage();
	while (1);
}

static inline uint32_t *stack_push(u32 *stack, u32 value)
{
	stack = &stack[-1];
	*stack = value;
	return stack;
}

/* setup_stack_and_mtrrs() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
static void *setup_stack_and_mtrrs(void)
{
	int num_mtrrs;
	uint32_t *slot;
	uint32_t mtrr_mask_upper;
	uint32_t top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	slot = (void *)romstage_ram_stack_top();
	num_mtrrs = 0;

	/* The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits. */
	mtrr_mask_upper = (1 << ((cpuid_eax(0x80000008) & 0xff) - 32)) - 1;

	/* The order for each MTRR is value then base with upper 32-bits of
	 * each value coming before the lower 32-bits. The reasoning for
	 * this ordering is to create a stack layout like the following:
	 *   +0: Number of MTRRs
	 *   +4: MTRR base 0 31:0
	 *   +8: MTRR base 0 63:32
	 *  +12: MTRR mask 0 31:0
	 *  +16: MTRR mask 0 63:32
	 *  +20: MTRR base 1 31:0
	 *  +24: MTRR base 1 63:32
	 *  +28: MTRR mask 1 31:0
	 *  +32: MTRR mask 1 63:32
	 */

	/* Cache the ROM as WP just below 4GiB. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = (uint32_t)cbmem_top();
	/* Cache 8MiB below the top of ram. The top of RAM under 4GiB is the
	 * start of the TSEG region. It is required to be 8MiB aligned. Set
	 * this area as cacheable so it can be used later for ramstage before
	 * setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of ram. Top of RAM is where the TSEG
	 * region resides. However, it is not restricted to SMM mode until
	 * SMM has been relocated. By setting the region to cacheable it
	 * provides faster access when relocating the SMM handler as well
	 * as using the TSEG region for other purposes. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, top_of_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Save the number of MTRRs to setup. Return the stack location
	 * pointing to the number of MTRRs. */
	slot = stack_push(slot, num_mtrrs);

	return slot;
}
