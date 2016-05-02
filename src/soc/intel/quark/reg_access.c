/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <soc/reg_access.h>

void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MCR,
		(opcode << QNC_MCR_OP_OFFSET)
		| ((uint32_t)port << QNC_MCR_PORT_OFFSET)
		| ((reg_address & QNC_MCR_MASK) << QNC_MCR_REG_OFFSET)
		| QNC_MCR_BYTE_ENABLES);
}

uint32_t mdr_read(void)
{
	return pci_read_config32(MC_BDF, QNC_ACCESS_PORT_MDR);
}

void mdr_write(uint32_t value)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MDR, value);
}

void mea_write(uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MEA, reg_address
		& QNC_MEA_MASK);
}

static uint32_t reg_usb_read(uint32_t reg_address)
{
	/* Read the USB register */
	mea_write(reg_address);
	mcr_write(QUARK_ALT_OPCODE_READ, QUARK_SC_USB_AFE_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_usb_write(uint32_t reg_address, uint32_t value)
{
	/* Write the USB register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_ALT_OPCODE_WRITE, QUARK_SC_USB_AFE_SB_PORT_ID,
		reg_address);
}

static uint64_t reg_read(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;
	uint64_t value = 0;

	switch (step->id) {
	default:
		printk(BIOS_ERR,
			"ERROR - Unknown register set (0x%08x)!\n",
			step->id);
		ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		return 0;

	case USB_PHY_REGS:
		ctx->display_prefix = "USB PHY";
		value = reg_usb_read(step->reg);
		break;
	}
	return value;
}

static void reg_write(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;

	switch (step->id) {
	default:
		printk(BIOS_ERR,
			"ERROR - Unknown register set (0x%08x)!\n",
			step->id);
		ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		return;

	case USB_PHY_REGS:
		ctx->display_prefix = "USB PHY";
		reg_usb_write(step->reg, (uint32_t)step->value);
		break;
	}
}

const struct reg_script_bus_entry soc_reg_script_bus_table = {
	SOC_TYPE, reg_read, reg_write
};

REG_SCRIPT_BUS_ENTRY(soc_reg_script_bus_table);
