/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/southbridge.h>
#include <assert.h>
#include <compiler.h>

static const struct soc_amd_event gpio_event_table[] = {
	{ GPIO_1, GEVENT_19 },
	{ GPIO_2, GEVENT_8 },
	{ GPIO_3, GEVENT_2 },
	{ GPIO_4, GEVENT_4 },
	{ GPIO_5, GEVENT_7 },
	{ GPIO_6, GEVENT_10 },
	{ GPIO_7, GEVENT_11 },
	{ GPIO_8, GEVENT_23 },
	{ GPIO_9, GEVENT_22 },
	{ GPIO_11, GEVENT_18 },
	{ GPIO_13, GEVENT_21 },
	{ GPIO_14, GEVENT_6 },
	{ GPIO_15, GEVENT_20 },
	{ GPIO_16, GEVENT_12 },
	{ GPIO_17, GEVENT_13 },
	{ GPIO_18, GEVENT_14 },
	{ GPIO_21, GEVENT_5 },
	{ GPIO_22, GEVENT_3 },
	{ GPIO_23, GEVENT_16 },
	{ GPIO_24, GEVENT_15 },
	{ GPIO_65, GEVENT_0 },
	{ GPIO_66, GEVENT_1 },
	{ GPIO_68, GEVENT_9 },
	{ GPIO_69, GEVENT_17 },
};

static int get_gpio_gevent(uint8_t gpio)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(gpio_event_table); i++) {
		if (gpio_event_table[i].gpio == gpio)
			return (int)gpio_event_table[i].event;
	}
	return -1;
}

static void mem_read_write32(uint32_t *address, uint32_t value, uint32_t mask)
{
	uint32_t reg32;

	value &= mask;
	reg32 = read32(address);
	reg32 &= ~mask;
	reg32 |= value;
	write32(address, reg32);
}

__weak void configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level)
{
	printk(BIOS_WARNING, "Warning: SMI disabled!\n");
}

static void program_smi(uint32_t flag, int gevent_num)
{
	uint32_t trigger;

	trigger = flag & FLAGS_TRIGGER_MASK;
	/*
	 * Only level trigger is allowed for SMI. Trigger values are 0
	 * through 3, with 0-1 being level trigger and 2-3 being edge
	 * trigger. GPIO_TRIGGER_EDGE_LOW is 2, so trigger has to be
	 * less than GPIO_TRIGGER_EDGE_LOW.
	 */
	assert(trigger < GPIO_TRIGGER_EDGE_LOW);

	if (trigger == GPIO_TRIGGER_LEVEL_HIGH)
		configure_gevent_smi(gevent_num, SMI_MODE_SMI,
					SMI_SCI_LVL_HIGH);
	if (trigger == GPIO_TRIGGER_LEVEL_LOW)
		configure_gevent_smi(gevent_num, SMI_MODE_SMI,
					SMI_SCI_LVL_LOW);
}

static void route_sci(uint8_t event)
{
	smi_write8(SMI_SCI_MAP(event), event);
}

static void get_sci_config_bits(uint32_t flag, uint32_t *edge, uint32_t *level)
{
	uint32_t trigger;

	trigger = flag & FLAGS_TRIGGER_MASK;
	switch (trigger) {
	case GPIO_TRIGGER_LEVEL_LOW:
		*edge = SCI_TRIGGER_LEVEL;
		*level = 0;
		break;
	case GPIO_TRIGGER_LEVEL_HIGH:
		*edge = SCI_TRIGGER_LEVEL;
		*level = 1;
		break;
	case GPIO_TRIGGER_EDGE_LOW:
		*edge = SCI_TRIGGER_EDGE;
		*level = 0;
		break;
	case GPIO_TRIGGER_EDGE_HIGH:
		*edge = SCI_TRIGGER_EDGE;
		*level = 1;
		break;
	default:
		break;
	}
}

static uintptr_t gpio_get_address(gpio_t gpio_num)
{
	uintptr_t gpio_address;

	if (gpio_num < 64)
		gpio_address = GPIO_BANK0_CONTROL(gpio_num);
	else if (gpio_num < 128)
		gpio_address = GPIO_BANK1_CONTROL(gpio_num);
	else
		gpio_address = GPIO_BANK2_CONTROL(gpio_num);

	return gpio_address;
}

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);

	return !!(reg & GPIO_PIN_STS);
}

void gpio_set(gpio_t gpio_num, int value)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_OUTPUT_MASK;
	reg |=  !!value << GPIO_OUTPUT_SHIFT;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pulldown(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_PULLUP_ENABLE;
	reg |=  GPIO_PULLDOWN_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input_pullup(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_PULLDOWN_ENABLE;
	reg |=  GPIO_PULLUP_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_input(gpio_t gpio_num)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg &= ~GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
}

void gpio_output(gpio_t gpio_num, int value)
{
	uint32_t reg;
	uintptr_t gpio_address = gpio_get_address(gpio_num);

	reg = read32((void *)gpio_address);
	reg |=  GPIO_OUTPUT_ENABLE;
	write32((void *)(uintptr_t)gpio_num, reg);
	gpio_set(gpio_num, value);
}

const char *gpio_acpi_path(gpio_t gpio)
{
	return "\\_SB.GPIO";
}

uint16_t gpio_acpi_pin(gpio_t gpio)
{
	return gpio;
}

void sb_program_gpios(const struct soc_amd_gpio *gpio_list_ptr, size_t size)
{
	uint8_t *mux_ptr;
	uint32_t *gpio_ptr;
	uint32_t control, control_flags, edge_level, direction;
	uint32_t mask, bit_edge, bit_level;
	uint8_t mux, index, gpio;
	int gevent_num;

	direction = 0;
	edge_level = 0;
	mask = 0;
	for (index = 0; index < size; index++) {
		gpio = gpio_list_ptr[index].gpio;
		mux = gpio_list_ptr[index].function;
		control = gpio_list_ptr[index].control;
		control_flags = gpio_list_ptr[index].flags;

		mux_ptr = (uint8_t *)(uintptr_t)(gpio + AMD_GPIO_MUX);
		write8(mux_ptr, mux & AMD_GPIO_MUX_MASK);
		/* special case if pin 2 is assigned to wake */
		if ((gpio == 2) && !(mux & AMD_GPIO_MUX_MASK))
			route_sci(GPIO_2_EVENT);
		gpio_ptr = (uint32_t *)gpio_get_address(gpio);

		if (control_flags & GPIO_SPECIAL_FLAG) {
			gevent_num = get_gpio_gevent(gpio);
			if (gevent_num < 0) {
				printk(BIOS_WARNING, "Warning: GPIO pin %d has"
					" no associated gevent!\n", gpio);
				continue;
			}
			switch (control_flags & GPIO_SPECIAL_MASK) {
			case GPIO_DEBOUNCE_FLAG:
				mem_read_write32(gpio_ptr, control,
						GPIO_DEBOUNCE_MASK);
				break;
			case GPIO_WAKE_FLAG:
				mem_read_write32(gpio_ptr, control,
						INT_WAKE_MASK);
				break;
			case GPIO_INT_FLAG:
				mem_read_write32(gpio_ptr, control,
						AMD_GPIO_CONTROL_MASK);
				break;
			case GPIO_SMI_FLAG:
				mem_read_write32(gpio_ptr, control,
						INT_SCI_SMI_MASK);
				program_smi(control_flags, gevent_num);
				break;
			case GPIO_SCI_FLAG:
				mem_read_write32(gpio_ptr, control,
						INT_SCI_SMI_MASK);
				get_sci_config_bits(control_flags, &bit_edge,
							&bit_level);
				edge_level |= bit_edge << gevent_num;
				direction |= bit_level << gevent_num;
				mask |= (1 << gevent_num);
				route_sci(gevent_num);
				break;
			default:
				printk(BIOS_WARNING, "Error, flags 0x%08x\n",
							control_flags);
				break;
			}
		} else {
			mem_read_write32(gpio_ptr, control,
						AMD_GPIO_CONTROL_MASK);
		}
	}
	/* Set all SCI trigger direction (high/low) */
	mem_read_write32((uint32_t *)(uintptr_t)(APU_SMI_BASE + SMI_SCI_TRIG),
					direction, mask);

	/* Set all SCI trigger level (edge/level) */
	mem_read_write32((uint32_t *)(uintptr_t)(APU_SMI_BASE + SMI_SCI_LEVEL),
					edge_level, mask);
}

int gpio_interrupt_status(gpio_t gpio)
{
	uintptr_t gpio_address = gpio_get_address(gpio);
	uint32_t reg = read32((void *)gpio_address);

	if (reg & GPIO_INT_STATUS) {
		/* Clear interrupt status, preserve wake status */
		reg &= ~GPIO_WAKE_STATUS;
		write32((void *)gpio_address, reg);
		return 1;
	}

	return 0;
}
