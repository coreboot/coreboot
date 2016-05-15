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

static uint32_t *get_gpio_address(uint32_t reg_address)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(I2CGPIO_BDF, PCI_BASE_ADDRESS_1);
	gpio_base_address &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	ASSERT (gpio_base_address != 0x00000000);

	/* Return the GPIO register address */
	return (uint32_t *)(gpio_base_address + reg_address);
}

void *get_i2c_address(void)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(I2CGPIO_BDF, PCI_BASE_ADDRESS_0);
	gpio_base_address &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	ASSERT (gpio_base_address != 0x00000000);

	/* Return the GPIO register address */
	return (void *)gpio_base_address;
}

static uint16_t get_legacy_gpio_address(uint32_t reg_address)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(LPC_BDF, R_QNC_LPC_GBA_BASE);
	ASSERT (gpio_base_address >= 0x80000000);
	gpio_base_address &= B_QNC_LPC_GPA_BASE_MASK;

	/* Return the GPIO register address */
	return (uint16_t)(gpio_base_address + reg_address);
}

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

static uint32_t reg_gpio_read(uint32_t reg_address)
{
	/* Read the GPIO register */
	return *get_gpio_address(reg_address);
}

static void reg_gpio_write(uint32_t reg_address, uint32_t value)
{
	/* Write the GPIO register */
	*get_gpio_address(reg_address) = value;
}

uint32_t reg_legacy_gpio_read(uint32_t reg_address)
{
	/* Read the legacy GPIO register */
	return inl(get_legacy_gpio_address(reg_address));
}

void reg_legacy_gpio_write(uint32_t reg_address, uint32_t value)
{
	/* Write the legacy GPIO register */
	outl(value, get_legacy_gpio_address(reg_address));
}

uint32_t reg_rmu_temp_read(uint32_t reg_address)
{
	/* Read the RMU temperature register */
	mea_write(reg_address);
	mcr_write(QUARK_OPCODE_READ, QUARK_NC_RMU_SB_PORT_ID, reg_address);
	return mdr_read();
}

static void reg_rmu_temp_write(uint32_t reg_address, uint32_t value)
{
	/* Write the RMU temperature register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_OPCODE_WRITE, QUARK_NC_RMU_SB_PORT_ID, reg_address);
}

static uint32_t reg_soc_unit_read(uint32_t reg_address)
{
	/* Read the temperature sensor register */
	mea_write(reg_address);
	mcr_write(QUARK_ALT_OPCODE_READ, QUARK_SCSS_SOC_UNIT_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_soc_unit_write(uint32_t reg_address, uint32_t value)
{
	/* Write the temperature sensor register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_ALT_OPCODE_WRITE, QUARK_SCSS_SOC_UNIT_SB_PORT_ID,
		reg_address);
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

	case GPIO_REGS:
		ctx->display_prefix = "GPIO: ";
		value = reg_gpio_read(step->reg);
		break;

	case LEG_GPIO_REGS:
		ctx->display_prefix = "Legacy GPIO: ";
		value = reg_legacy_gpio_read(step->reg);
		break;

	case RMU_TEMP_REGS:
		ctx->display_prefix = "RMU TEMP";
		value = reg_rmu_temp_read(step->reg);
		break;

	case SOC_UNIT_REGS:
		ctx->display_prefix = "SOC Unit";
		value = reg_soc_unit_read(step->reg);
		break;

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

	case GPIO_REGS:
		ctx->display_prefix = "GPIO: ";
		reg_gpio_write(step->reg, (uint32_t)step->value);
		break;

	case LEG_GPIO_REGS:
		ctx->display_prefix = "Legacy GPIO: ";
		reg_legacy_gpio_write(step->reg, (uint32_t)step->value);
		break;

	case RMU_TEMP_REGS:
		ctx->display_prefix = "RMU TEMP";
		reg_rmu_temp_write(step->reg, (uint32_t)step->value);
		break;

	case SOC_UNIT_REGS:
		ctx->display_prefix = "SOC Unit";
		reg_soc_unit_write(step->reg, (uint32_t)step->value);
		break;

	case MICROSECOND_DELAY:
		/* The actual delay is >= the requested delay */
		if (ctx->display_features) {
			/* Higher baud-rates will reduce the impact of displaying this message */
			printk(BIOS_INFO, "Delay %lld uSec\n", step->value);
			ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		}
		udelay(step->value);
		break;

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
