/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * cache.c: Cache Maintenance Instructions
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 */

#include <stdint.h>

#include <arch/lib_helpers.h>

void dccisw(uint64_t cisw)
{
	__asm__ __volatile__("dc cisw, %0\n\t" : : "r" (cisw) :"memory");
}

void dccivac(uint64_t civac)
{
	__asm__ __volatile__("dc civac, %0\n\t" : : "r" (civac) :"memory");
}

void dccsw(uint64_t csw)
{
	__asm__ __volatile__("dc csw, %0\n\t" : : "r" (csw) :"memory");
}

void dccvac(uint64_t cvac)
{
	__asm__ __volatile__("dc cvac, %0\n\t" : : "r" (cvac) :"memory");
}

void dccvau(uint64_t cvau)
{
	__asm__ __volatile__("dc cvau, %0\n\t" : : "r" (cvau) :"memory");
}

void dcisw(uint64_t isw)
{
	__asm__ __volatile__("dc isw, %0\n\t" : : "r" (isw) :"memory");
}

void dcivac(uint64_t ivac)
{
	__asm__ __volatile__("dc ivac, %0\n\t" : : "r" (ivac) :"memory");
}

void dczva(uint64_t zva)
{
	__asm__ __volatile__("dc zva, %0\n\t" : : "r" (zva) :"memory");
}

void iciallu(void)
{
	__asm__ __volatile__("ic iallu\n\t" : : :"memory");
}

void icialluis(void)
{
	__asm__ __volatile__("ic ialluis\n\t" : : :"memory");
}

void icivau(uint64_t ivau)
{
	__asm__ __volatile__("ic ivau, %0\n\t" : : "r" (ivau) :"memory");
}
