/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <cbfs.h>

#ifdef CONFIG_LZMA
#define CBFS_CORE_WITH_LZMA
#include <lzma.h>
#endif

#define ERROR(x...) printf(x)
#define LOG(x...)

static uint32_t host_virt_to_phys(void *addr);
static void *host_phys_to_virt(uint32_t addr);

uint32_t romstart(void);
uint32_t romend(void);

#include <arch/virtual.h>
static uint32_t host_virt_to_phys(void *addr)
{
	return virt_to_phys(addr);
}

static void *host_phys_to_virt(uint32_t addr)
{
	return phys_to_virt(addr);
}
#undef virt_to_phys
#undef phys_to_virt

uint32_t (*virt_to_phys)(void *) = host_virt_to_phys;
void* (*phys_to_virt)(uint32_t) = host_phys_to_virt;


uint32_t _romstart = 0xffffffff;
uint32_t _romend = 0;

uint32_t romstart(void)
{
	return _romstart;
}

uint32_t romend(void)
{
	return _romend;
}

#include "cbfs_core.c"

static uint32_t ram_cbfs_offset;

static uint32_t ram_virt_to_phys(void *addr)
{
	return (uint32_t)addr - ram_cbfs_offset;
}

static void *ram_phys_to_virt(uint32_t addr)
{
	return (void*)addr + ram_cbfs_offset;
}

void setup_cbfs_from_ram(void* start, uint32_t size)
{
	/* assumes rollover */
	ram_cbfs_offset = (uint32_t)start + size;
	virt_to_phys = ram_virt_to_phys;
	phys_to_virt = ram_phys_to_virt;
}

void setup_cbfs_from_flash(void)
{
	virt_to_phys = host_virt_to_phys;
	phys_to_virt = host_phys_to_virt;
}
