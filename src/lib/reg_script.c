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

#if CONFIG_SOC_INTEL_BAYTRAIL
#include <baytrail/iosf.h>
#endif

#define POLL_DELAY 100 /* 100us */
#if defined(__PRE_RAM__)
#define EMPTY_DEV 0
#else
#define EMPTY_DEV NULL
#endif

struct reg_script_context {
	device_t dev;
	struct resource *res;
	const struct reg_script *step;
};

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
		return read8(step->reg);
	case REG_SCRIPT_SIZE_16:
		return read16(step->reg);
	case REG_SCRIPT_SIZE_32:
		return read32(step->reg);
	}
	return 0;
}

static void reg_script_write_mmio(struct reg_script_context *ctx)
{
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->size) {
	case REG_SCRIPT_SIZE_8:
		write8(step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_16:
		write16(step->reg, step->value);
		break;
	case REG_SCRIPT_SIZE_32:
		write32(step->reg, step->value);
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

static uint32_t reg_script_read_iosf(struct reg_script_context *ctx)
{
#if CONFIG_SOC_INTEL_BAYTRAIL
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->id) {
	case IOSF_PORT_BUNIT:
		return iosf_bunit_read(step->reg);
	case IOSF_PORT_DUNIT_CH0:
		return iosf_dunit_ch0_read(step->reg);
	case IOSF_PORT_PMC:
		return iosf_punit_read(step->reg);
	case IOSF_PORT_USBPHY:
		return iosf_usbphy_read(step->reg);
	case IOSF_PORT_USHPHY:
		return iosf_ushphy_read(step->reg);
	}
#endif
	return 0;
}

static void reg_script_write_iosf(struct reg_script_context *ctx)
{
#if CONFIG_SOC_INTEL_BAYTRAIL
	const struct reg_script *step = reg_script_get_step(ctx);

	switch (step->id) {
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
	case IOSF_PORT_USHPHY:
		iosf_ushphy_write(step->reg, step->value);
		break;
	}
#endif
}

static uint32_t reg_script_read(struct reg_script_context *ctx)
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
	case REG_SCRIPT_TYPE_IOSF:
		return reg_script_read_iosf(ctx);
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
	case REG_SCRIPT_TYPE_IOSF:
		reg_script_write_iosf(ctx);
		break;
	}
}

static void reg_script_rmw(struct reg_script_context *ctx)
{
	uint32_t value;
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

static void reg_script_run_with_context(struct reg_script_context *ctx)
{
	uint32_t value = 0, try;

	while (1) {
		const struct reg_script *step = reg_script_get_step(ctx);

		if (step->command == REG_SCRIPT_COMMAND_END)
			break;

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
				printk(BIOS_WARNING, "%s: POLL timeout waiting "
				       "for 0x%08x to be 0x%08x, got 0x%08x\n",
				       __func__, step->reg, step->value, value);
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

void reg_script_run(const struct reg_script *step)
{
	struct reg_script_context ctx;

	reg_script_set_dev(&ctx, EMPTY_DEV);
	reg_script_set_step(&ctx, step);
	reg_script_run_with_context(&ctx);
}
