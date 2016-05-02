/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef _QUARK_ROMSTAGE_H_
#define _QUARK_ROMSTAGE_H_

#if !defined(__PRE_RAM__)
#error "Don't include romstage.h from a ramstage compilation unit!"
#endif

#include <fsp/romstage.h>
#include <soc/reg_access.h>

uint32_t port_reg_read(uint8_t port, uint32_t offset);
void port_reg_write(uint8_t port, uint32_t offset, uint32_t value);
void report_platform_info(void);
int set_base_address_and_enable_uart(u8 bus, u8 dev, u8 func, u32 mmio_base);

#endif /* _QUARK_ROMSTAGE_H_ */
