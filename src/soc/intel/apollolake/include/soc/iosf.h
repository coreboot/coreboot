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
 */

#ifndef _SOC_APOLLOLAKE_IOSF_H_
#define _SOC_APOLLOLAKE_IOSF_H_

#include <arch/io.h>

inline static void iosf_write(uint16_t port, uint16_t reg, uint32_t val)
{
	uintptr_t base = CONFIG_IOSF_BASE_ADDRESS | (port << 16) | (reg & ~3);
	write32((void *)base, val);
}

inline static uint32_t iosf_read(uint16_t port, uint16_t reg)
{
	uintptr_t base = CONFIG_IOSF_BASE_ADDRESS | (port << 16) | (reg & ~3);
	return read32((void *)base);
}

#endif /* _SOC_APOLLOLAKE_IOSF_H_ */
