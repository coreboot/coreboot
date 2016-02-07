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

#include <fsp/car.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

void car_soc_pre_console_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART1))
		set_base_address_and_enable_uart(0, HSUART1_DEV, HSUART1_FUNC,
			UART_BASE_ADDRESS);
}
