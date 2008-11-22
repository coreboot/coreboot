/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "msrtool.h"

static struct cpuid_t id;

struct cpuid_t *cpuid(void) {
	uint32_t outeax;
	asm ("cpuid" : "=a" (outeax) : "a" (1) : "%ebx", "%ecx", "%edx");
	id.stepping = outeax & 0xf;
	outeax >>= 4;
	id.model = outeax & 0xf;
	outeax >>= 4;
	id.family = outeax & 0xf;
	outeax >>= 8;
	id.ext_model = outeax & 0xf;
	outeax >>= 4;
	id.ext_family = outeax & 0xff;
	if (0xf == id.family) {
		/* Intel says always do this, AMD says only for family f */
		id.model |= (id.ext_model << 4);
		id.family += id.ext_family;
	}
	return &id;
}
