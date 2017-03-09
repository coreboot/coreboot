/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_IOSF_H_
#define _SOC_APOLLOLAKE_IOSF_H_

#include <arch/io.h>

#define IOSF_RTC_PORT_ID		0xD1
#define  RTC_CONFIG			0x3400
#define   RTC_CONFIG_UCMOS_ENABLE	(1 << 2)

static inline void *iosf_address(uint16_t port, uint16_t reg)
{
	uintptr_t addr = (CONFIG_IOSF_BASE_ADDRESS | (port << 16) | (reg & ~3));
	return (void *)addr;
}

static inline void iosf_write(uint16_t port, uint16_t reg, uint32_t val)
{
	write32(iosf_address(port, reg), val);
}

static inline uint32_t iosf_read(uint16_t port, uint16_t reg)
{
	return read32(iosf_address(port, reg));
}
#endif /* _SOC_APOLLOLAKE_IOSF_H_ */
