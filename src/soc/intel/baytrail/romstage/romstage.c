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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stddef.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#if CONFIG_EC_GOOGLE_CHROMEEC
#include <ec/google/chromeec/ec.h>
#endif
#include <elog.h>
#include <ramstage_cache.h>
#include <ramstage_cache.h>
#include <romstage_handoff.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <baytrail/gpio.h>
#include <baytrail/iomap.h>
#include <baytrail/lpc.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/reset.h>
#include <baytrail/romstage.h>
#include <baytrail/smm.h>

static inline uint64_t timestamp_get(void)
{
	return rdtscll();
}

static inline tsc_t ts64_to_tsc(uint64_t ts)
{
	tsc_t tsc = {
		.lo = ts,
		.hi = ts >> 32,
	};
	return tsc;
}

/* The cache-as-ram assembly file calls romstage_main() after setting up
 * cache-as-ram.  romstage_main() will then call the mainboards's
 * mainboard_romstage_entry() function. That function then calls
 * romstage_common() below. The reason for the back and forth is to provide
 * common entry point from cache-as-ram while still allowing for code sharing.
 * Because we can't use global variables the stack is used for allocations --
 * thus the need to call back and forth. */

static void *setup_stack_and_mttrs(void);

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

static inline void mark_ts(struct romstage_params *rp, uint64_t ts)
{
	struct romstage_timestamps *rt = &rp->ts;

	rt->times[rt->count] = ts;
	rt->count++;
}

/* Entry from cache-as-ram.inc. */
void * asmlinkage romstage_main(unsigned long bist,
                                uint32_t tsc_low, uint32_t tsc_hi)
{
	struct romstage_params rp = {
		.bist = bist,
		.mrc_params = NULL,
	};

	/* Save initial timestamp from bootblock. */
	mark_ts(&rp, (((uint64_t)tsc_hi) << 32) | (uint64_t)tsc_low);
	/* Save romstage begin */
	mark_ts(&rp, timestamp_get());

	program_base_addresses();

	tco_disable();

	byt_config_com1_and_enable();

	console_init();

	set_max_freq();

	punit_init();

	gfx_init();

#if CONFIG_EC_GOOGLE_CHROMEEC
	/* Ensure the EC is in the right mode for recovery */
	google_chromeec_early_init();
#endif

	/* Call into mainboard. */
	mainboard_romstage_entry(&rp);

	return setup_stack_and_mttrs();
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
static int chipset_prev_sleep_state(void)
{
	uint16_t pm1_sts;
	uint32_t pm1_cnt;
	uint32_t gen_pmcon1;
	/* Default to S0. */
	int prev_sleep_state = 0;

	pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);

	if (pm1_sts & WAK_STS) {
		switch ((pm1_cnt & SLP_TYP) >> SLP_TYP_SHIFT) {
	#if CONFIG_HAVE_ACPI_RESUME
		case SLP_TYP_S3:
			prev_sleep_state = 3;
			break;
	#endif
		case SLP_TYP_S5:
			prev_sleep_state = 5;
		}
		/* Clear SLP_TYP. */
		outl(pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
	}

	gen_pmcon1 = read32(PMC_BASE_ADDRESS + GEN_PMCON1);
	if (gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR)) {
		/* Clear power failure bits. */
		write32(PMC_BASE_ADDRESS + GEN_PMCON1, gen_pmcon1);
		prev_sleep_state = 5;
	}

	printk(BIOS_DEBUG, "pm1_sts = %04x pm1_cnt = %08x gen_pmcon1 = %08x\n",
	       pm1_sts, pm1_cnt, gen_pmcon1);

	return prev_sleep_state;
}

#if CONFIG_CHROMEOS
static inline void chromeos_init(int prev_sleep_state)
{
	/* Normalize the sleep state to what init_chromeos() wants for S3: 2. */
	init_chromeos(prev_sleep_state == 3 ? 2 : 0);
}
#else
static inline void chromeos_init(int prev_sleep_state) {}
#endif

/* Entry from the mainboard. */
void romstage_common(struct romstage_params *params)
{
	struct romstage_handoff *handoff;
	int prev_sleep_state;

	mark_ts(params, timestamp_get());

#if CONFIG_ELOG_BOOT_COUNT
	boot_count_increment();
#endif

	prev_sleep_state = chipset_prev_sleep_state();

	printk(BIOS_DEBUG, "prev_sleep_state = S%d\n", prev_sleep_state);

	/* Initialize RAM */
	raminit(params->mrc_params, prev_sleep_state);

	mark_ts(params, timestamp_get());

	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = (prev_sleep_state == 3);
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	chromeos_init(prev_sleep_state);

	/* Save timestamp information. */
	timestamp_init(ts64_to_tsc(params->ts.times[0]));
	timestamp_add(TS_START_ROMSTAGE, ts64_to_tsc(params->ts.times[1]));
	timestamp_add(TS_BEFORE_INITRAM, ts64_to_tsc(params->ts.times[2]));
	timestamp_add(TS_AFTER_INITRAM, ts64_to_tsc(params->ts.times[3]));
}

static void open_up_spi(void)
{
	const uintptr_t sbase = SPI_BASE_ADDRESS;

	/* Disable generating SMI when setting WPD bit. */
	write32(sbase + 0xf8, read32(sbase + 0xf8) & ~(1 << 7));
	/* Disable the SMM-only BIOS write and set WPD bit. */
	write32(sbase + 0xfc, 1 | (read32(sbase + 0xfc) & ~(1 << 5)));
}

void asmlinkage romstage_after_car(void)
{
	/* Allow BIOS to program SPI part. */
	open_up_spi();

	timestamp_add_now(TS_END_ROMSTAGE);

	/* Run vboot verification if configured. */
	vboot_verify_firmware(romstage_handoff_find_or_add());

	/* Load the ramstage. */
	copy_and_run();
	while (1);
}

static inline uint32_t *stack_push(u32 *stack, u32 value)
{
	stack = &stack[-1];
	*stack = value;
	return stack;
}

/* Romstage needs quite a bit of stack for decompressing images since the lzma
 * lib keeps its state on the stack during romstage. */
static unsigned long choose_top_of_stack(void)
{
	unsigned long stack_top;
	const unsigned long romstage_ram_stack_size = 0x5000;

	/* cbmem_add() does a find() before add(). */
	stack_top = (unsigned long)cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK,
	                                     romstage_ram_stack_size);
	stack_top += romstage_ram_stack_size;
	return stack_top;
}

/* setup_stack_and_mttrs() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
static void *setup_stack_and_mttrs(void)
{
	unsigned long top_of_stack;
	int num_mtrrs;
	uint32_t *slot;
	uint32_t mtrr_mask_upper;
	uint32_t top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	top_of_stack = choose_top_of_stack() & ~3;
	slot = (void *)top_of_stack;
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
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CONFIG_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CONFIG_RAMTOP - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = (uint32_t)cbmem_top();
	/* Cache 8MiB below the top of ram. The top of ram under 4GiB is the
	 * start of the TSEG region. It is required to be 8MiB aligned. Set
	 * this area as cacheable so it can be used later for ramstage before
	 * setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of ram. Top of ram is where the TSEG
	 * region resides. However, it is not restricted to SMM mode until
	 * SMM has been relocated. By setting the region to cacheable it
	 * provides faster access when relocating the SMM handler as well
	 * as using the TSEG region for other purposes. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRRphysMaskValid);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, top_of_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Save the number of MTRRs to setup. Return the stack location
	 * pointing to the number of MTRRs. */
	slot = stack_push(slot, num_mtrrs);

	return slot;
}

struct ramstage_cache *ramstage_cache_location(long *size)
{
	char *smm_base;
	/* 1MiB cache size */
	const long cache_size = CONFIG_SMM_RESERVED_SIZE;

	/* Ramstage cache lives in TSEG region which is the definition of
	 * cbmem_top(). */
	smm_base = cbmem_top();
	*size = cache_size;
	return (void *)&smm_base[smm_region_size() - cache_size];
}

void ramstage_cache_invalid(struct ramstage_cache *cache)
{
#if CONFIG_RESET_ON_INVALID_RAMSTAGE_CACHE
	/* Perform cold reset on invalid ramstage cache. */
	cold_reset();
#endif
}

