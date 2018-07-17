/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2012 Samsung Electronics
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

#include <arch/cpu.h>
#include <arch/io.h>
#include <soc/cpu.h>
#include <soc/power.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

/* ACTLR, L2CTLR L2ACTLR constants used in SMP core power up. */

#define ACTLR_SMP					(1 << 6)

#define L2CTLR_ECC_PARITY				(1 << 21)
#define L2CTLR_DATA_RAM_LATENCY_MASK			(7 << 0)
#define L2CTLR_TAG_RAM_LATENCY_MASK			(7 << 6)
#define L2CTLR_DATA_RAM_LATENCY_CYCLES_3		(2 << 0)
#define L2CTLR_TAG_RAM_LATENCY_CYCLES_3			(2 << 6)

#define L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL	(1 << 3)
#define L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT		(1 << 7)
#define L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE	(1 << 27)

/* Part number in CPU ID (MPIDR). */
#define PART_NUMBER_CORTEX_A15				(0xc0f)

/* State of CPU cores in Exynos 5420. */
#define CORE_STATE_RESET				(1 << 0)
#define CORE_STATE_SECONDARY_RESET			(1 << 1)
#define CORE_STATE_SWITCH_CLUSTER			(1 << 4)

/* The default address to re-power on a code. */
#define CORE_RESET_INIT_ADDRESS				((void *)0x00000000)

/* Vectors in BL1 (0x02020000 = base of iRAM). */
#define VECTOR_CORE_SEV_HANDLER			((void *)(intptr_t)0x02020004)
#define VECTOR_LOW_POWER_FLAG			((void *)(intptr_t)0x02020028)
#define VECTOR_LOW_POWER_ADDRESS		((void *)(intptr_t)0x0202002C)

/* The data structure for the "CPU state" memory page (shared with kernel)
 * controlling cores in active cluster. Kernel will put starting address for one
 * core in "hotplug_address" before power on. Note the address is hard-coded in
 * kernel (EXYNOS5420_PA_SYSRAM_NS = 0x02073000). */
volatile struct exynos5420_cpu_states
{
	uint32_t _reserved[2];		/* RESV, +0x00 */
	uint32_t resume_address;	/* REG0, +0x08 */
	uint32_t resume_flag;		/* REG1, +0x0C */
	uint32_t _reg2;			/* REG2, +0x10 */
	uint32_t _reg3;			/* REG3, +0x14 */
	uint32_t switch_address;	/* REG4, +0x18, cluster switching */
	uint32_t hotplug_address;	/* REG5, +0x1C, core hotplug */
	uint32_t _reg6;			/* REG6, +0x20 */
	uint32_t c2_address;		/* REG7, +0x24, C2 state change */

	/* Managed per core status for active cluster, offset: +0x28~0x38 */
	uint32_t cpu_states[4];

	/* Managed per core GIC status for active cluster, offset: 0x38~0x48 */
	uint32_t cpu_gic_states[4];
} *exynos_cpu_states = (volatile struct exynos5420_cpu_states*)0x02073000;

/* When leaving core handlers and jump to hot-plug address (or cluster
 * switching), we are not sure if the destination is Thumb or ARM mode.
 * So a BX command is required.
 */
inline static void jump_bx(void *address)
{
	asm volatile ("bx %0" : : "r"(address));
	/* never returns. */
}

/* Extracts arbitrary bits from a 32-bit unsigned int. */
inline static uint32_t get_bits(uint32_t value, uint32_t start, uint32_t len)
{
	return ((value << (sizeof(value) * 8 - len - start)) >>
		(sizeof(value) * 8 - len));
}

/* Waits the referenced address to be ready (non-zero) and then jump into it. */
static void wait_and_jump(volatile uint32_t *reference)
{
	while (!*reference) {
		wfe();
	}
	jump_bx((void *)*reference);
}

/* Configures L2 Control Register to use 3 cycles for DATA/TAG RAM latency. */
static void configure_l2ctlr(void)
{
   uint32_t val;

   val = read_l2ctlr();
   val &= ~(L2CTLR_DATA_RAM_LATENCY_MASK | L2CTLR_TAG_RAM_LATENCY_MASK);
   val |= (L2CTLR_DATA_RAM_LATENCY_CYCLES_3 | L2CTLR_TAG_RAM_LATENCY_CYCLES_3 |
	   L2CTLR_ECC_PARITY);
   write_l2ctlr(val);
}

/* Configures L2 Auxiliary Control Register for Cortex A15. */
static void configure_l2actlr(void)
{
   uint32_t val;

   val = read_l2actlr();
   val |= (L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL |
	   L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT |
	   L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE);
   write_l2actlr(val);
}

/* Initializes the CPU states to reset state. */
static void init_exynos_cpu_states(void) {
	memset((void *)exynos_cpu_states, 0, sizeof(*exynos_cpu_states));
	exynos_cpu_states->cpu_states[0] = CORE_STATE_RESET;
	exynos_cpu_states->cpu_states[1] = CORE_STATE_SECONDARY_RESET;
	exynos_cpu_states->cpu_states[2] = CORE_STATE_SECONDARY_RESET;
	exynos_cpu_states->cpu_states[3] = CORE_STATE_SECONDARY_RESET;
}

/*
 * Ensures that the L2 logic has been used within the previous 256 cycles
 * before modifying the ACTLR.SMP bit. This is required during boot before
 * MMU has been enabled, or during a specified reset or power down sequence.
 */
static void enable_smp(void)
{
	uint32_t actlr, val;

	/* Enable SMP mode */
	actlr = read_actlr();
	actlr |= ACTLR_SMP;

	/* Dummy read to assure L2 access */
	val = read32(&exynos_power->inform0);
	val &= 0;
	actlr |= val;

	write_actlr(actlr);
	dsb();
	isb();
}

/* Starts the core and jumps to correct location by its state. */
static void core_start_execution(void)
{
	u32 cpu_id, cpu_state;

	enable_smp();
	set_system_mode();

	cpu_id = read_mpidr() & 0x3;  /* up to 4 processors for one cluster. */
	cpu_state = exynos_cpu_states->cpu_states[cpu_id];

	if (cpu_state & CORE_STATE_SWITCH_CLUSTER) {
		wait_and_jump(&exynos_cpu_states->switch_address);
		/* never returns. */
	}

	/* Standard Exynos suspend/resume. */
	if (exynos_power->inform1) {
		exynos_power->inform1 = 0;
		jump_bx((void *)exynos_power->inform0);
		/* never returns. */
	}

	if (cpu_state & CORE_STATE_RESET) {
		/* For Reset, U-Boot jumps to its starting address;
		 * on coreboot, seems ok to ignore for now. */
	}
	wait_and_jump(&exynos_cpu_states->hotplug_address);
	/* never returns. */
}

/* The entry point for hotplug-in and cluster switching. */
static void low_power_start(void)
{
	uint32_t sctlr, reg_val;

	/* On warm reset, because iRAM is not cleared, all cores will enter
	 * low_power_start, not the initial address. So we need to check reset
	 * status again, and jump to 0x0 in that case. */
	reg_val = read32(&exynos_power->spare0);
	if (reg_val != RST_FLAG_VAL) {
		write32(VECTOR_LOW_POWER_FLAG, 0x0);
		jump_bx(CORE_RESET_INIT_ADDRESS);
		/* restart CPU execution and never returns. */
	}

	/* Workaround for iROM EVT1.  A7 core execution may flow into incorrect
	 * path, bypassing first jump address and makes final jump address 0x0,
	 * so we try to make any core set again low_power_start address, if that
	 * becomes zero. */
	reg_val = read32(VECTOR_CORE_SEV_HANDLER);
	if (reg_val != (intptr_t)low_power_start) {
		write32(VECTOR_CORE_SEV_HANDLER, (intptr_t)low_power_start);
		dsb();
		/* ask all cores to power on again. */
		sev();
	}

	set_system_mode();

	/* Whenever a Cortex A-15 core powers on, iROM resets its L2 cache
	 * so we need to configure again. */
	if (get_bits(read_midr(), 4, 12) == PART_NUMBER_CORTEX_A15) {
		configure_l2ctlr();
		configure_l2actlr();
	}

	/* Invalidate L1 & TLB */
	tlbiall();
	iciallu();

	/* Disable MMU stuff and caches */
	sctlr = read_sctlr();
	sctlr &= ~(SCTLR_V | SCTLR_M | SCTLR_C);
	sctlr |= (SCTLR_I | SCTLR_Z | SCTLR_A);
	write_sctlr(sctlr);

	core_start_execution();
	/* The core should not return. But in order to prevent unexpected
	 * errors, a WFI command will help to put CPU back to idle state. */
	wfi();
}

/* Callback to shutdown a core, safe to be set as hot-plug address. */
static void power_down_core(void)
{
	uint32_t mpidr, core_id;

	/* MPIDR: 0~2=ID, 8~11=cluster. On Exynos 5420, cluster will be only 0
	 * or 1. */
	mpidr = read_mpidr();
	core_id = get_bits(mpidr, 0, 2) | (get_bits(mpidr, 8, 4) << 2);

	/* Set the status of the core to low.
	 * S5E5420A User Manual, 8.8.1.202, ARM_CORE0_CONFIGURATION, two bits to
	 * control power state in each power down level.
	 */
	write32(&exynos_power->arm_core[core_id].config, 0x0);

	/* S5E5420A User Manual, 8.4.2.5, after ARM_CORE*_CONFIGURATION has been
	 * set to zero, PMU will detect and wait for WFI then run power-down
	 * sequence. */
	wfi();
}

/* Configures the CPU states shared memory page and then shutdown all cores. */
static void configure_secondary_cores(void)
{
	if (get_bits(read_midr(), 4, 12) == PART_NUMBER_CORTEX_A15) {
		configure_l2ctlr();
		configure_l2actlr();
	}

	/* Currently we use power_down_core as callback for each core to
	 * shutdown itself, but it is also ok to directly set ARM_CORE*_CONFIG
	 * to zero by CPU0 because every secondary cores should be already in
	 * WFI state (in bootblock). The power_down_core will be more helpful
	 * when we want to use SMP inside firmware. */

	/* Clear boot reg (hotplug address) in CPU states */
	write32((void *)&exynos_cpu_states->hotplug_address, 0);

	/* set low_power flag and address */
	write32(VECTOR_LOW_POWER_ADDRESS, (intptr_t)low_power_start);
	write32(VECTOR_LOW_POWER_FLAG, RST_FLAG_VAL);
	write32(&exynos_power->spare0, RST_FLAG_VAL);

	/* On next SEV, shutdown all cores. */
	write32(VECTOR_CORE_SEV_HANDLER, (intptr_t)power_down_core);

	/* Ask all cores in WFE mode to shutdown. */
	dsb();
	sev();
}

/* Configures the SMP cores on Exynos 5420 SOC (and shutdown all secondary
 * cores) */
void exynos5420_config_smp(void)
{
	init_exynos_cpu_states();
	configure_secondary_cores();
}
