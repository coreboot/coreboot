/*
 * This file is part of the libpayload project.
 *
 * Copyright 2018 Google LLC.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <arch/apic.h>
#include <arch/cpuid.h>
#include <arch/msr.h>
#include <exception.h>

#define APIC_BASE_MSR			0x0000001B
#define APIC_BASE_MASK			(0xFFFFFFFULL << 12)

#define CPUID_XAPIC_ENABLED_BIT		(1 << 9)
#define CPUID_XAPIC2_ENABLED_BIT	(1 << 21)

#define XAPIC_ENABLED_BIT		(1 << 11)
#define X2APIC_ENABLED_BIT		(1 << 10)
#define APIC_MASKED_BIT			(1 << 16)
#define APIC_SW_ENABLED_BIT		(1 << 8)

#define APIC_ID				0x020
#define   APIC_ID_SHIFT			24
#define   APIC_ID_MASK			(0xFFUL << APIC_ID_SHIFT)
#define APIC_VERSION			0x030
#define   APIC_MAX_LVT_SHIFT		16
#define   APIC_MAX_LVT_MASK		(0xFFUL << APIC_MAX_LVT_SHIFT)
#define APIC_TASK_PRIORITY		0x080
#define   APIC_TASK_PRIORITY_MASK	0xFFUL
#define APIC_EOI			0x0B0
#define APIC_SPURIOUS			0x0F0
#define APIC_LVT_TIMER			0x320
#define APIC_TIMER_INIT_COUNT		0x380
#define APIC_TIMER_CUR_COUNT		0x390
#define APIC_TIMER_DIV_CFG		0x3E0

#define APIC_LVT_SIZE			0x010

#define APIC_TIMER_VECTOR		0x20

static uint32_t apic_bar;
static int _apic_initialized;
// TODO: Build a lookup table to avoid calculating it.
static uint32_t ticks_per_ms;
static volatile uint8_t timer_waiting;

enum APIC_CAPABILITY {
	DISABLED = 0,
	XACPI = 1 << 0,
	X2ACPI = 1 << 1
};

int apic_initialized(void)
{
	return _apic_initialized;
}

static inline uint32_t apic_read32(uint32_t offset)
{
	return read32((void *)(apic_bar + offset));
}

static inline void apic_write32(uint32_t offset, uint32_t value)
{
	write32((void *)(apic_bar + offset), value);
}

uint8_t apic_id(void)
{
	die_if(!apic_bar, "APIC is not initialized");

	uint8_t id =
		(apic_read32(APIC_ID) & APIC_ID_MASK) >> APIC_ID_SHIFT;

	return id;
}

void apic_delay(unsigned int usec)
{
	die_if(!ticks_per_ms, "apic_init_timer was not run.");
	die_if(timer_waiting, "timer already started.");
	die_if(!interrupts_enabled(), "Interrupts disabled.");

	/* The order is important so we don't underflow */
	uint64_t ticks = usec * ticks_per_ms / USECS_PER_MSEC;

	/* Not enough resolution */
	if (!ticks)
		return;

	/* Disable interrupts so we don't get a race condition between
	 * starting the timer and the hlt instruction. */
	disable_interrupts();

	timer_waiting = 1;

	apic_write32(APIC_TIMER_INIT_COUNT, ticks);

	/* Loop in case another interrupt has fired and resumed execution. */
	do {
		asm volatile(
			"sti\n\t"
			"hlt\n\t"
			/* Disable interrupts to prevent a race condition
			 * between checking timer_waiting and executing the hlt
			 * instruction again. */
			"cli\n\t");
	} while (timer_waiting);

	/* Leave hardware interrupts enabled. */
	enable_interrupts();
}

static void timer_interrupt_handler(u8 vector)
{
	timer_waiting = 0;
}

void apic_eoi(void)
{
	die_if(!apic_bar, "APIC is not initialized");

	apic_write32(APIC_EOI, 0);
}

static enum APIC_CAPABILITY apic_capabilities(void)
{
	uint32_t eax, ebx, ecx, edx;

	cpuid(1, eax, ebx, ecx, edx);

	enum APIC_CAPABILITY capabilities = DISABLED;

	if (edx & CPUID_XAPIC_ENABLED_BIT)
		capabilities |= XACPI;

	if (ecx & CPUID_XAPIC2_ENABLED_BIT)
		capabilities |= X2ACPI;

	return capabilities;
}

static uint8_t apic_max_lvt_entries(void)
{
	die_if(!apic_bar, "APIC is not initialized");

	uint32_t reg = apic_read32(APIC_VERSION);
	reg &= APIC_MAX_LVT_MASK;
	reg >>= APIC_MAX_LVT_SHIFT;

	return (uint8_t)reg;
}

static void apic_reset_all_lvts(void)
{
	uint8_t max = apic_max_lvt_entries();
	for (int i = 0; i <= max; ++i) {
		uint32_t offset = APIC_LVT_TIMER + APIC_LVT_SIZE * i;
		apic_write32(offset, APIC_MASKED_BIT);
	}
}

static void apic_set_task_priority(uint8_t priority)
{
	die_if(!apic_bar, "APIC is not initialized");

	uint32_t tpr = apic_read32(APIC_TASK_PRIORITY);
	tpr &= ~APIC_TASK_PRIORITY_MASK;
	tpr |= priority;

	apic_write32(APIC_TASK_PRIORITY, priority);
}

static void apic_init_timer(void)
{
	die_if(!apic_bar, "APIC is not initialized");

	apic_write32(APIC_LVT_TIMER, APIC_MASKED_BIT);

	/* Divide the clock by 1. */
	apic_write32(APIC_TIMER_DIV_CFG, 0xB);

	/* Calibrate the APIC timer */
	if (!ticks_per_ms) {
		/* Set APIC init counter to MAX and count for 1 ms */
		apic_write32(APIC_TIMER_INIT_COUNT, UINT32_MAX);

		/* This is safe because apic_initialized() returns false so
		 * arch_ndelay() falls back to a busy loop. */
		mdelay(1);

		ticks_per_ms =
			UINT32_MAX - apic_read32(APIC_TIMER_CUR_COUNT);
	}

	/* Clear the count so we don't get any stale interrupts */
	apic_write32(APIC_TIMER_INIT_COUNT, 0);

	/* Unmask the timer and set the vector. */
	apic_write32(APIC_LVT_TIMER, APIC_TIMER_VECTOR);
}

static void apic_sw_enable(void)
{
	uint32_t reg = apic_read32(APIC_SPURIOUS);
	if (reg & APIC_SW_ENABLED_BIT)
		return;

	reg |= APIC_SW_ENABLED_BIT;

	apic_write32(APIC_SPURIOUS, reg);
}

void apic_init(void)
{
	uint64_t apic_bar_reg;

	printf("APIC Init Started\n");

	die_if(apic_initialized(), "APIC already initialized");
	die_if(!(apic_capabilities() & XACPI), "APIC is not supported");

	apic_bar_reg = _rdmsr(APIC_BASE_MSR);

	die_if(!(apic_bar_reg & XAPIC_ENABLED_BIT), "APIC is not enabled");
	die_if(apic_bar_reg & X2APIC_ENABLED_BIT,
	       "APIC is configured in x2APIC mode which is not supported");

	apic_bar = (uint32_t)(apic_bar_reg & APIC_BASE_MASK);

	apic_reset_all_lvts();
	apic_set_task_priority(0);

	apic_sw_enable();

	apic_init_timer();

	set_interrupt_handler(APIC_TIMER_VECTOR, &timer_interrupt_handler);

	_apic_initialized = 1;

	printf("APIC Configured\n");
}
