/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/resource.h>
#include <device/pci.h>
#include <stdint.h>
#include <reg_script.h>

#if CONFIG_ARCH_X86
#include <cpu/x86/msr.h>
#endif

#if CONFIG_SOC_INTEL_BAYTRAIL
#include <baytrail/iosf.h>
#endif

#define POLL_DELAY 100 /* 100us */
#if defined(__PRE_RAM__)
#define EMPTY_DEV 0
#else
#define EMPTY_DEV NULL
#endif

static inline void reg_script_set_dev(struct reg_script_context *ctx,
                                      device_t dev)
{
	ctx->dev = dev;
	ctx->res = NULL;
}

static inline void reg_script_set_step(struct reg_script_context *ctx,
                                       const struct reg_script *step)
{
	ctx->step = step;
}

static inline const struct reg_script *
reg_script_get_step(struct reg_script_context *ctx)
{
	return ctx->step;
}

static struct resource *reg_script_get_resource(struct reg_script_context *ctx)
{
#if defined(__PRE_RAM__)
	return NULL;
#else
	struct resource *res;
	const struct reg_script *step = reg_script_get_step(ctx);

	res = ctx->res;

	if (res != NULL && res->index == step->res_index)
		return res;

	res = find_resource(ctx->dev, step->res_index);
	ctx->res = res;
	return res;
#endif
}

static uint32_t reg_script_read_pci(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		return pci_read_config8(ctx->dev, step->reg);
	case REG_SCRIPT_SIZE_16:
		return pci_read_config16(ctx->dev, step->reg);
	case REG_SCRIPT_SIZE_32:
		return pci_read_config32(ctx->dev, step->reg);
	}
	return 0;
}

static void reg_script_write_pci(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		pci_write_config8(ctx->dev, step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_16:
		pci_write_config16(ctx->dev, step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_32:
		pci_write_config32(ctx->dev, step->reg, step->value);
		break;
	}
}

static uint32_t reg_script_read_io(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		return inb(step->reg);
	case REG_SCRIPT_SIZE_16:
		return inw(step->reg);
	case REG_SCRIPT_SIZE_32:
		return inl(step->reg);
	}
	return 0;
}

static void reg_script_write_io(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		outb(step->value, step->reg);
		break;
	case REG_SCRIPT_SIZE_16:
		outw(step->value, step->reg);
		break;
	case REG_SCRIPT_SIZE_32:
		outl(step->value, step->reg);
		break;
	}
}

static uint32_t reg_script_read_mmio(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		return read8((u8 *)step->reg);
	case REG_SCRIPT_SIZE_16:
		return read16((u16 *)step->reg);
	case REG_SCRIPT_SIZE_32:
		return read32((u32 *)step->reg);
	}
	return 0;
}

static void reg_script_write_mmio(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		write8((u8 *)step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_16:
		write16((u16 *)step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_32:
		write32((u32 *)step->reg, step->value);
		break;
	}
}

static uint32_t reg_script_read_res(struct reg_script_context *ctx)
{
	struct resource *res;
	uint32_t val = 0;
	const struct reg_script *step = reg_script_get_step(ctx);

	res = reg_script_get_resource(ctx);

	if (res == NULL)
		return val;

	if (res->flags & IORESOURCE_IO) {
		const struct reg_script io_step = {
			.size = step->size,
			.reg = res->base + step->reg,
		};
		reg_script_set_step(ctx, &io_step);
		val = reg_script_read_io(ctx);
	}
	else if (res->flags & IORESOURCE_MEM) {
		const struct reg_script mmio_step = {
			.size = step->size,
			.reg = res->base + step->reg,
		};
		reg_script_set_step(ctx, &mmio_step);
		val = reg_script_read_mmio(ctx);
	}
	reg_script_set_step(ctx, step);
	return val;
}

static void reg_script_write_res(struct reg_script_context *ctx)
{
	struct resource *res;
	const struct reg_script *step = reg_script_get_step(ctx);

	res = reg_script_get_resource(ctx);

	if (res == NULL)
		return;

	if (res->flags & IORESOURCE_IO) {
		const struct reg_script io_step = {
			.size = step->size,
			.reg = res->base + step->reg,
			.value = step->value,
		};
		reg_script_set_step(ctx, &io_step);
		reg_script_write_io(ctx);
	}
	else if (res->flags & IORESOURCE_MEM) {
		const struct reg_script mmio_step = {
			.size = step->size,
			.reg = res->base + step->reg,
			.value = step->value,
		};
		reg_script_set_step(ctx, &mmio_step);
		reg_script_write_mmio(ctx);
	}
	reg_script_set_step(ctx, step);
}

#if CONFIG_SOC_INTEL_BAYTRAIL
static uint32_t reg_script_read_iosf(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->id) {
	case IOSF_PORT_AUNIT:
		return iosf_aunit_read(step->reg);
	case IOSF_PORT_CPU_BUS:
		return iosf_cpu_bus_read(step->reg);
	case IOSF_PORT_BUNIT:
		return iosf_bunit_read(step->reg);
	case IOSF_PORT_DUNIT_CH0:
		return iosf_dunit_ch0_read(step->reg);
	case IOSF_PORT_PMC:
		return iosf_punit_read(step->reg);
	case IOSF_PORT_USBPHY:
		return iosf_usbphy_read(step->reg);
	case IOSF_PORT_SEC:
		return iosf_sec_read(step->reg);
	case IOSF_PORT_0x45:
		return iosf_port45_read(step->reg);
	case IOSF_PORT_0x46:
		return iosf_port46_read(step->reg);
	case IOSF_PORT_0x47:
		return iosf_port47_read(step->reg);
	case IOSF_PORT_SCORE:
		return iosf_score_read(step->reg);
	case IOSF_PORT_0x55:
		return iosf_port55_read(step->reg);
	case IOSF_PORT_0x58:
		return iosf_port58_read(step->reg);
	case IOSF_PORT_0x59:
		return iosf_port59_read(step->reg);
	case IOSF_PORT_0x5a:
		return iosf_port5a_read(step->reg);
	case IOSF_PORT_USHPHY:
		return iosf_ushphy_read(step->reg);
	case IOSF_PORT_SCC:
		return iosf_scc_read(step->reg);
	case IOSF_PORT_LPSS:
		return iosf_lpss_read(step->reg);
	case IOSF_PORT_0xa2:
		return iosf_porta2_read(step->reg);
	case IOSF_PORT_CCU:
		return iosf_ccu_read(step->reg);
	case IOSF_PORT_SSUS:
		return iosf_ssus_read(step->reg);
	default:
		printk(BIOS_DEBUG, "No read support for IOSF port 0x%x.\n",
		       step->id);
		break;
	}
	return 0;
}

static void reg_script_write_iosf(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->id) {
	case IOSF_PORT_AUNIT:
		iosf_aunit_write(step->reg, step->value);
		break;
	case IOSF_PORT_CPU_BUS:
		iosf_cpu_bus_write(step->reg, step->value);
		break;
	case IOSF_PORT_BUNIT:
		iosf_bunit_write(step->reg, step->value);
		break;
	case IOSF_PORT_DUNIT_CH0:
		iosf_dunit_write(step->reg, step->value);
		break;
	case IOSF_PORT_PMC:
		iosf_punit_write(step->reg, step->value);
		break;
	case IOSF_PORT_USBPHY:
		iosf_usbphy_write(step->reg, step->value);
		break;
	case IOSF_PORT_SEC:
		iosf_sec_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x45:
		iosf_port45_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x46:
		iosf_port46_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x47:
		iosf_port47_write(step->reg, step->value);
		break;
	case IOSF_PORT_SCORE:
		iosf_score_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x55:
		iosf_port55_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x58:
		iosf_port58_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x59:
		iosf_port59_write(step->reg, step->value);
		break;
	case IOSF_PORT_0x5a:
		iosf_port5a_write(step->reg, step->value);
		break;
	case IOSF_PORT_USHPHY:
		iosf_ushphy_write(step->reg, step->value);
		break;
	case IOSF_PORT_SCC:
		iosf_scc_write(step->reg, step->value);
		break;
	case IOSF_PORT_LPSS:
		iosf_lpss_write(step->reg, step->value);
		break;
	case IOSF_PORT_0xa2:
		iosf_porta2_write(step->reg, step->value);
		break;
	case IOSF_PORT_CCU:
		iosf_ccu_write(step->reg, step->value);
		break;
	case IOSF_PORT_SSUS:
		iosf_ssus_write(step->reg, step->value);
		break;
	default:
		printk(BIOS_DEBUG, "No write support for IOSF port 0x%x.\n",
		       step->id);
		break;
	}
}
#endif


static uint64_t reg_script_read_msr(struct reg_script_context *ctx)
{
#if CONFIG_ARCH_X86
	const struct reg_script *step = reg_script_get_step(ctx);
	msr_t msr = rdmsr(step->reg);
	uint64_t value = msr.hi;
	value = msr.hi;
	value <<= 32;
	value |= msr.lo;
	return value;
#endif
}

static void reg_script_write_msr(struct reg_script_context *ctx)
{
#if CONFIG_ARCH_X86
	const struct reg_script *step = reg_script_get_step(ctx);
	msr_t msr;
	msr.hi = step->value >> 32;
	msr.lo = step->value & 0xffffffff;
	wrmsr(step->reg, msr);
#endif
}

#ifndef __PRE_RAM__
/* Default routine provided for systems without platform specific busses */
const struct reg_script_bus_entry *__attribute__((weak))
	platform_bus_table(size_t *table_entries)
{
	/* No platform bus type table supplied */
	*table_entries = 0;
	return NULL;
}

/* Locate the structure containing the platform specific bus access routines */
static const struct reg_script_bus_entry
	*find_bus(const struct reg_script *step)
{
	const struct reg_script_bus_entry *bus;
	size_t table_entries;
	size_t i;

	/* Locate the platform specific bus */
	bus = platform_bus_table(&table_entries);
	for (i = 0; i < table_entries; i++) {
		if (bus[i].type == step->type)
			return &bus[i];
	}

	/* Bus not found */
	return NULL;
}
#endif

static uint64_t reg_script_read(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->type) {
	case REG_SCRIPT_TYPE_PCI:
		return reg_script_read_pci(ctx);
	case REG_SCRIPT_TYPE_IO:
		return reg_script_read_io(ctx);
	case REG_SCRIPT_TYPE_MMIO:
		return reg_script_read_mmio(ctx);
	case REG_SCRIPT_TYPE_RES:
		return reg_script_read_res(ctx);
	case REG_SCRIPT_TYPE_MSR:
		return reg_script_read_msr(ctx);
#if CONFIG_SOC_INTEL_BAYTRAIL
	case REG_SCRIPT_TYPE_IOSF:
		return reg_script_read_iosf(ctx);
#endif
	default:
#ifndef __PRE_RAM__
		{
			const struct reg_script_bus_entry *bus;

			/* Read from the platform specific bus */
			bus = find_bus(step);
			if (NULL != bus)
				return bus->reg_script_read(ctx);
		}
#endif
		printk(BIOS_ERR,
			"Unsupported read type (0x%x) for this device!\n",
			step->type);
		break;
	}
	return 0;
}

static void reg_script_write(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->type) {
	case REG_SCRIPT_TYPE_PCI:
		reg_script_write_pci(ctx);
		break;
	case REG_SCRIPT_TYPE_IO:
		reg_script_write_io(ctx);
		break;
	case REG_SCRIPT_TYPE_MMIO:
		reg_script_write_mmio(ctx);
		break;
	case REG_SCRIPT_TYPE_RES:
		reg_script_write_res(ctx);
		break;
	case REG_SCRIPT_TYPE_MSR:
		reg_script_write_msr(ctx);
		break;
#if CONFIG_SOC_INTEL_BAYTRAIL
	case REG_SCRIPT_TYPE_IOSF:
		reg_script_write_iosf(ctx);
		break;
#endif
	default:
#ifndef __PRE_RAM__
		{
			const struct reg_script_bus_entry *bus;

			/* Write to the platform specific bus */
			bus = find_bus(step);
			if (NULL != bus) {
				bus->reg_script_write(ctx);
				return;
			}
		}
#endif
		printk(BIOS_ERR,
			"Unsupported write type (0x%x) for this device!\n",
			step->type);
		break;
	}
}

static void reg_script_rmw(struct reg_script_context *ctx)
{
	uint64_t value;
	const struct reg_script *step = reg_script_get_step(ctx);
	struct reg_script write_step = *step;

	value = reg_script_read(ctx);
	value &= step->mask;
	value |= step->value;
	write_step.value = value;
	reg_script_set_step(ctx, &write_step);
	reg_script_write(ctx);
	reg_script_set_step(ctx, step);
}

/* In order to easily chain scripts together handle the REG_SCRIPT_COMMAND_NEXT
 * as recursive call with a new context that has the same dev and resource
 * as the previous one. That will run to completion and then move on to the
 * next step of the previous context. */
static void reg_script_run_next(struct reg_script_context *ctx,
                                const struct reg_script *step);


static void reg_script_run_step(struct reg_script_context *ctx,
				const struct reg_script *step)
{
	uint64_t value = 0, try;

	switch (step->command) {
	case REG_SCRIPT_COMMAND_READ:
		(void)reg_script_read(ctx);
		break;
	case REG_SCRIPT_COMMAND_WRITE:
		reg_script_write(ctx);
		break;
	case REG_SCRIPT_COMMAND_RMW:
		reg_script_rmw(ctx);
		break;
	case REG_SCRIPT_COMMAND_POLL:
		for (try = 0; try < step->timeout; try += POLL_DELAY) {
			value = reg_script_read(ctx) & step->mask;
			if (value == step->value)
				break;
			udelay(POLL_DELAY);
		}
		if (try >= step->timeout)
			printk(BIOS_WARNING, "%s: POLL timeout waiting for "
			       "0x%x to be 0x%lx, got 0x%lx\n", __func__,
			       step->reg, (unsigned long)step->value,
			       (unsigned long)value);
		break;
	case REG_SCRIPT_COMMAND_SET_DEV:
		reg_script_set_dev(ctx, step->dev);
		break;
	case REG_SCRIPT_COMMAND_NEXT:
		reg_script_run_next(ctx, step->next);
		break;
	default:
		printk(BIOS_WARNING, "Invalid command: %08x\n",
		       step->command);
		break;
	}
}

static void reg_script_run_with_context(struct reg_script_context *ctx)
{
	while (1) {
		const struct reg_script *step = reg_script_get_step(ctx);

		if (step->command == REG_SCRIPT_COMMAND_END)
			break;

		reg_script_run_step(ctx, step);
		reg_script_set_step(ctx, step + 1);
	}
}

static void reg_script_run_next(struct reg_script_context *prev_ctx,
                                const struct reg_script *step)
{
	struct reg_script_context ctx;

	/* Use prev context as a basis but start at a new step. */
	ctx = *prev_ctx;
	reg_script_set_step(&ctx, step);
	reg_script_run_with_context(&ctx);
}

void reg_script_run_on_dev(device_t dev, const struct reg_script *step)
{
	struct reg_script_context ctx;

	reg_script_set_dev(&ctx, dev);
	reg_script_set_step(&ctx, step);
	reg_script_run_with_context(&ctx);
}

void reg_script_run(const struct reg_script *step)
{
	reg_script_run_on_dev(EMPTY_DEV, step);
}
