/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <device/mmio.h>
#include <console/console.h>
#include <gic.h>
#include <stddef.h>
#include "gic.h"

enum {
	ENABLE_GRP0 = 0x1 << 0,
	ENABLE_GRP1 = 0x1 << 1,
	FIQ_BYP_DIS_GRP0 = 0x1 << 5,
	IRQ_BYP_DIS_GRP0 = 0x1 << 6,
	FIQ_BYP_DIS_GRP1 = 0x1 << 7,
	IRQ_BYP_DIS_GRP1 = 0x1 << 8,
};

struct gic {
	struct gicd_mmio *gicd;
	struct gicc_mmio *gicc;
	size_t num_interrupts;
	unsigned int version;
	unsigned int security_extensions;
};

static struct gic *gic_get(void)
{
	static struct gic gic;

	if (gic.gicd == NULL) {
		uint32_t typer;

		gic.gicd = gicd_base();
		gic.gicc = gicc_base();
		typer = read32(&gic.gicd->typer);
		gic.num_interrupts = 32 * ((typer & 0x1f) + 1);
		gic.security_extensions = !!(typer & (1 << 10));
		gic.version = (read32(&gic.gicd->icpidr2) & 0xf0) >> 4;

		printk(BIOS_DEBUG, "GICv%d - %zu ints %s GICD=%p GICC=%p\n",
			gic.version, gic.num_interrupts,
			gic.security_extensions ? "SecExtn" : "",
			gic.gicd, gic.gicc);
	}

	return &gic;
}

static inline uint32_t gic_read(uint32_t *base)
{
	return read32(base);
}

static inline void gic_write(uint32_t *base, uint32_t val)
{
	write32(base, val);
}

static void gic_write_regs(uint32_t *base, size_t num_regs, uint32_t val)
{
	size_t i;

	for (i = 0; i < num_regs; i++)
		gic_write(base++, val);
}

static void gic_write_banked_regs(uint32_t *base, size_t interrupts_per_reg,
					uint32_t val)
{
	/* 1st 32 interrupts are banked per CPU. */
	gic_write_regs(base, 32 / interrupts_per_reg, val);
}

void gic_init(void)
{
	struct gic *gic;
	struct gicd_mmio *gicd;
	struct gicc_mmio *gicc;
	uint32_t cpu_mask;

	gic = gic_get();
	gicd = gic->gicd;
	gicc = gic->gicc;

	/* Enable Group 0 and Group 1 in GICD -- banked regs.  */
	gic_write(&gicd->ctlr, ENABLE_GRP0 | ENABLE_GRP1);

	/* Enable Group 0 and Group 1 in GICC and enable all priroity levels. */
	gic_write(&gicc->ctlr, ENABLE_GRP0 | ENABLE_GRP1);
	gic_write(&gicc->pmr, 1 << 7);

	cpu_mask = 1 << smp_processor_id();
	cpu_mask |= cpu_mask << 8;
	cpu_mask |= cpu_mask << 16;

	/* Only write banked registers for secondary CPUs. */
	if (smp_processor_id()) {
		gic_write_banked_regs(&gicd->itargetsr[0], 4, cpu_mask);
		/* Put interrupts into Group 1. */
		gic_write_banked_regs(&gicd->igroupr[0], 32, ~0x0);
		/* Allow Non-secure access to everything. */
		gic_write_banked_regs(&gicd->nsacr[0], 16, ~0x0);
		return;
	}

	/* All interrupts routed to processors that execute this function. */
	gic_write_regs(&gicd->itargetsr[0], gic->num_interrupts / 4, cpu_mask);
	/* Put all interrupts into Gropup 1. */
	gic_write_regs(&gicd->igroupr[0], gic->num_interrupts / 32, ~0x0);
	/* Allow Non-secure access to everything. */
	gic_write_regs(&gicd->nsacr[0], gic->num_interrupts / 16, ~0x0);
}

void gic_disable(void)
{
	struct gic *gic;
	struct gicc_mmio *gicc;

	gic = gic_get();
	gicc = gic->gicc;

	/* Disable secure, non-secure interrupts. */
	uint32_t val = gic_read(&gicc->ctlr);
	val &= ~(ENABLE_GRP0 | ENABLE_GRP1);
	/*
	 * Enable the IRQ/FIQ BypassDisable bits to bypass the IRQs.
	 * So the CPU can wake up from power gating state when the GIC
	 * was disabled.
	 */
	val |= FIQ_BYP_DIS_GRP0 | IRQ_BYP_DIS_GRP0 |
	       FIQ_BYP_DIS_GRP1 | IRQ_BYP_DIS_GRP1;
	gic_write(&gicc->ctlr, val);
}

void gic_enable(void)
{
	struct gic *gic;
	struct gicc_mmio *gicc;

	gic = gic_get();
	gicc = gic->gicc;

	/* Enable secure, non-secure interrupts. */
	uint32_t val = gic_read(&gicc->ctlr);
	val |= (ENABLE_GRP0 | ENABLE_GRP1);
	gic_write(&gicc->ctlr, val);
}
