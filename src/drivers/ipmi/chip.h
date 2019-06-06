/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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

#ifndef _IMPI_CHIP_H_
#define _IPMI_CHIP_H_

struct drivers_ipmi_config {
	u8 bmc_i2c_address;
	u8 have_nv_storage;
	u8 nv_storage_device_address;
	u8 have_gpe;
	u8 gpe_interrupt;
	u8 have_apic;
	u32 apic_interrupt;
};

#endif /* _IMPI_CHIP_H_ */
