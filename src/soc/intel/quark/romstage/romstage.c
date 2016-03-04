/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <arch/early_variables.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <soc/intel/common/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>

void car_soc_pre_console_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART1))
		set_base_address_and_enable_uart(0, HSUART1_DEV, HSUART1_FUNC,
			UART_BASE_ADDRESS);
}

void car_soc_post_console_init(void)
{
	report_platform_info();
};

static struct chipset_power_state power_state CAR_GLOBAL;

struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	ps->prev_sleep_state = 0;
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps;
}
