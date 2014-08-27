/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/cpu.h>
#include <arch/io.h>
#include <stdint.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/ramstage.h>

enum {
	GICD_CTLR  = 0x0,
		ENABLE_GRP0 = 0x1 << 0,
		ENABLE_GRP1 = 0x1 << 1,
	GICD_TYPER = 0x4,
	GICD_ITARGETSR = 0x800,
	GICD_IGROUPR = 0x80,
	GICD_NSACR = 0xe00,

	GICC_CTLR  = 0x0,
	GICC_PMR   = 0x4,
};

struct gicd {
	uint32_t *base;
	size_t num_interrupts;
};

static inline uint32_t gic_read(uint32_t *base, size_t byte_offset)
{
	return read32(&base[byte_offset / sizeof(uint32_t)]);
}

static inline void gic_write(uint32_t *base, size_t byte_offset, uint32_t val)
{
	write32(val, &base[byte_offset / sizeof(uint32_t)]);
}

static inline uint32_t gicd_read(struct gicd *gicd, size_t byte_offset)
{
	return gic_read(gicd->base, byte_offset);
}

static inline void gicd_write(struct gicd *gicd, size_t byte_offset,
				uint32_t val)
{
	gic_write(gicd->base, byte_offset, val);
}

static void gic_write_regs(uint32_t *base, size_t offset,
				size_t tot_interrupts,
				size_t interrupts_per_reg, uint32_t val)
{
	size_t i;
	size_t bound = sizeof(uint32_t) * tot_interrupts / interrupts_per_reg;

	for (i = 0; i < bound; i += sizeof(uint32_t))
		gic_write(base, offset + i, val);
}

static void gicd_write_regs(struct gicd *gicd, size_t offset,
				size_t interrupts_per_reg, uint32_t val)
{
	gic_write_regs(gicd->base, offset, gicd->num_interrupts,
			interrupts_per_reg, val);
}

static void gicd_write_banked_regs(struct gicd *gicd, size_t offset,
				size_t interrupts_per_reg, uint32_t val)
{
	/* 1st 32 interrupts are banked per CPU. */
	gic_write_regs(gicd->base, offset, 32, interrupts_per_reg, val);
}

static void gicd_init(struct gicd *gicd, uintptr_t base)
{
	uint32_t typer;

	gicd->base = (void *)base;

	typer = gicd_read(gicd, GICD_TYPER);

	gicd->num_interrupts = 32 * ((typer & 0x1f) + 1);

	printk(BIOS_DEBUG, "GICD at %p. TYPER=%08x, %zu interrupts.\n",
		gicd->base, typer, gicd->num_interrupts);
}

void gic_init(void)
{
	struct gicd gicd;
	uint32_t * const gicc = (void *)(uintptr_t)TEGRA_GICC_BASE;
	uint32_t cpu_mask;

	gicd_init(&gicd, TEGRA_GICD_BASE);

	/* Enable Group 0 and Group 1 */
	gicd_write(&gicd, GICD_CTLR, ENABLE_GRP0 | ENABLE_GRP1);

	/* Enable Group 0 and Group 1 in GICC and enable all priroity levels. */
	gic_write(gicc, GICC_CTLR, ENABLE_GRP0 | ENABLE_GRP1);
	gic_write(gicc, GICC_PMR, 1 << 7);

	cpu_mask = 1 << smp_processor_id();
	cpu_mask |= cpu_mask << 8;
	cpu_mask |= cpu_mask << 16;

	/* Only write banked registers for secondary CPUs. */
	if (smp_processor_id()) {
		gicd_write_banked_regs(&gicd, GICD_ITARGETSR, 4, cpu_mask);
		/* Put interrupts into Group 1. */
		gicd_write_banked_regs(&gicd, GICD_IGROUPR, 32, 0xffffffff);
		/* Allow Non-secure access to everything. */
		gicd_write_banked_regs(&gicd, GICD_NSACR, 16, 0xffffffff);
		return;
	}

	/* All interrupts routed to processors that execute this function. */
	gicd_write_regs(&gicd, GICD_ITARGETSR, 4, cpu_mask);
	/* Put all interrupts into Gropup 1. */
	gicd_write_regs(&gicd, GICD_IGROUPR, 32, 0xffffffff);
	/* Allow Non-secure access to everything. */
	gicd_write_regs(&gicd, GICD_NSACR, 16, 0xffffffff);
}
