/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
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

#include "ppc.h"
#include "ppcreg.h"

unsigned ppc_getmsr(void)
{
	unsigned result;   
	__asm__ volatile ("mfmsr %0" : "=r" (result));
	return result;
}

unsigned ppc_gethid0(void)
{
	unsigned result;
	__asm__ volatile ("mfspr %0,1008" : "=r" (result));
	return result;
}

unsigned ppc_gethid1(void)
{
	unsigned result;
	__asm__ volatile ("mfspr %0,1009" : "=r" (result));
	return result;
}

void ppc_sethid0(unsigned value)
{
	__asm__ volatile ("mtspr 1008,%0" : : "r" (value));
}

unsigned ppc_getpvr(void)
{
	unsigned result;
	__asm__("mfspr %0, 287" : "=r" (result));
	return result;
}

void ppc_setmsr(unsigned value)
{
	__asm__ volatile ("mtmsr %0; sync" :: "r" (value));   
}

void ppc_set1015(unsigned value)
{
	__asm__ volatile ("mtspr 1015,%0" : : "r" (value));
}

