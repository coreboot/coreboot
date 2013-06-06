/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/car.h>
#include <lib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <ec/acpi/ec.h>
#include <delay.h>
#include <timestamp.h>

#include "dock.h"
#include "southbridge/intel/ibexpeak/pch.h"
#include "southbridge/intel/ibexpeak/gpio.h"

#include "northbridge/intel/nehalem/nehalem.h"
#include "northbridge/intel/nehalem/raminit.h"
#include "southbridge/intel/ibexpeak/pch.h"
#include "southbridge/intel/ibexpeak/gpio.h"
#include "southbridge/intel/ibexpeak/me.h"

//#define SERIALICE 1

static void pch_enable_lpc(void)
{
	/* Parrot EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN | COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, 0x1c1681);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, (0x68 & ~3) | 0x00040001);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);
}


static void rcba_config(void)
{
  static const u32 rcba_dump3[] = {
    /* 30fc */    0x00000000,
    /* 3100 */    0x04341200, 0x00000000, 0x40043214, 0x00014321, 
    /* 3110 */    0x00000002, 0x30003214, 0x00000001, 0x00000002,
    /* 3120 */    0x00000000, 0x00002321, 0x00000000, 0x00000000, 
    /* 3130 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3140 */    0x00003107, 0x76543210, 0x00000010, 0x00007654,
    /* 3150 */    0x00000004, 0x00000000, 0x00000000, 0x00003210,
    /* 3160 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3170 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3180 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3190 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 31a0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    /* 31b0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 31c0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 31d0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 31e0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,    
    /* 31f0 */    0x00000000, 0x00000000, 0x00000000, 0x03000000, 
    /* 3200 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3210 */    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    /* 3220 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3230 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3240 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3250 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3260 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3270 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3280 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3290 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 32a0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 32b0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 32c0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 32d0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,    
    /* 32e0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    /* 32f0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    /* 3300 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3310 */    0x02060100, 0x0000000f, 0x01020000, 0x80000000,
    /* 3320 */    0x00000000, 0x04000000, 0x00000000, 0x00000000,
    /* 3330 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3340 */    0x000fffff, 0x00000000, 0x00000000, 0x00000000,
    /* 3350 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3360 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3370 */    0x00000000, 0x00000000, 0x7f8fdfff, 0x00000000,
    /* 3380 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3390 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 33a0 */    0x00003900, 0x00000000, 0x00000000, 0x00000000,
    /* 33b0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 33c0 */    0x00010000, 0x00000000, 0x00000000, 0x0001004b,
    /* 33d0 */    0x06000008, 0x00010000, 0x00000000, 0x00000000,
    /* 33e0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 33f0 */    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 3400 */    0x0000001c, 0x00000080, 0x00000000, 0x00000000,
    /* 3410 */    0x00000c61, 0x00000000, 0x16e61fe1, 0xbf4f001f,
    /* 3420 */    0x00000000, 0x00060010, 0x0000001d, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,    
    0xdeaddeed, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x20000557, 0x2000055f, 0x2000074b, 0x2000074b,
    0x20000557, 0x2000014b, 0x2000074b, 0x2000074b,
    0x2000074b, 0x2000074b, 0x2000055f, 0x2000055f,
    0x20000557, 0x2000055f, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x000026a3, 0x00040002, 0x01000052,
    0x02000772, 0x16000f8f, 0x1800ff4f, 0x0001d630,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xfc000201, 0x3c000201, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0a001f00, 0x00000000, 0x00000000, 0x00000001,
    0x00010000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x089c0018, 0x00000000, 0x00000000, 
    0x11111111, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x4e564d49, 0x00000000, 0x00000000,
  };
  unsigned i;
  for (i = 0; i < sizeof (rcba_dump3) / 4; i++)
    {
      RCBA32 (4 * i + 0x30fc) = rcba_dump3[i];
      (void) RCBA32 (4 * i + 0x30fc);
    }

#if 0
	RCBA32 (0x30fc) = 0x00000000;
	(void) RCBA32 (0x30fc);
	RCBA32 (0x3100) = 0x04341200;
	(void) RCBA32 (0x3100);
	RCBA32 (0x3104) = 0x00000000;
	(void) RCBA32 (0x3104);
	RCBA32 (0x3108) = 0x40043214;
	(void) RCBA32 (0x3108);
	RCBA32 (0x310c) = 0x00014321;
	(void) RCBA32 (0x310c);
	RCBA32 (0x3110) = 0x00000002;
	(void) RCBA32 (0x3110);
	RCBA32 (0x3114) = 0x30003214;
	(void) RCBA32 (0x3114);
	RCBA32 (0x311c) = 0x00000002;
	(void) RCBA32 (0x311c);
	RCBA32 (0x3120) = 0x00000000;
	(void) RCBA32 (0x3120);
	RCBA32 (0x3124) = 0x00002321;
	(void) RCBA32 (0x3124);
	RCBA32 (0x313c) = 0x00000000;
	(void) RCBA32 (0x313c);
	RCBA32 (0x3140) = 0x00003107;
	(void) RCBA32 (0x3140);
	RCBA32 (0x3144) = 0x76543210;
	(void) RCBA32 (0x3144);
	RCBA32 (0x3148) = 0x00000010;
	(void) RCBA32 (0x3148);
	RCBA32 (0x314c) = 0x00007654;
	(void) RCBA32 (0x314c);
	RCBA32 (0x3150) = 0x00000004;
	(void) RCBA32 (0x3150);
	RCBA32 (0x3158) = 0x00000000;
	(void) RCBA32 (0x3158);
	RCBA32 (0x315c) = 0x00003210;
	(void) RCBA32 (0x315c);
	RCBA32 (0x31fc) = 0x03000000;
	(void) RCBA32 (0x31fc);
#endif
}

#include <cbmem.h>

#ifdef SERIALICE
static void
sio_putc (char c)
{
  usbdebug_tx_byte(0, c);
  usbdebug_tx_flush (0);
}

static char
sio_getc (void)
{
  return usbdebug_rx_byte(0);
}

/* SIO helpers */

static void sio_putstring(const char *string)
{
	/* Very simple, no %d, %x etc. */
	while (*string) {
		if (*string == '\n')
			sio_putc('\r');
		sio_putc(*string);
		string++;
	}
}

#define sio_put_nibble(nibble)	\
	if (nibble > 9)		\
		nibble += ('a' - 10);	\
	else			\
		nibble += '0';	\
	sio_putc(nibble)

static void sio_put8(u8 data)
{
	u8 c;
		
	c = (data >> 4) & 0xf;
	sio_put_nibble(c);

	c = data & 0xf;
	sio_put_nibble(c);
}

static void sio_put16(u16 data)
{
	int i;
	for (i=12; i >= 0; i -= 4) {
		u8 c = (data >> i) & 0xf;
		sio_put_nibble(c);
	}
}

static void sio_put32(u32 data)
{
	int i;
	for (i=28; i >= 0; i -= 4) {
		u8 c = (data >> i) & 0xf;
		sio_put_nibble(c);
	}
}

static void sio_put64(u64 data)
{
	int i;
	for (i=60; i >= 0; i -= 4) {
		u8 c = (data >> i) & 0xf;
		sio_put_nibble(c);
	}
}

typedef struct _u128
{
  u64 lo;
  u64 hi;
} u128;

static void sio_put128(u128 data)
{
	sio_put64 (data.hi);
	sio_put64 (data.lo);
}

static u8 sio_get_nibble(void)
{
	u8 ret = 0;
	u8 nibble = sio_getc();

	if (nibble >= '0' && nibble <= '9') {
		ret = (nibble - '0');
	} else if (nibble >= 'a' && nibble <= 'f') {
		ret = (nibble - 'a') + 0xa;
	} else if (nibble >= 'A' && nibble <= 'F') {
		ret = (nibble - 'A') + 0xa;
	} else {
		sio_putstring("ERROR: parsing number\n");
	}
	return ret;
}

static u8 sio_get8(void)
{
	u8 data;
	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	return data;
}

static u16 sio_get16(void)
{
	u16 data;

	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();

	return data;
}

static u32 sio_get32(void)
{
	u32 data;

	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();

	return data;
}

static u64 sio_get64(void)
{
	u64 data = 0;
	int i;

	for (i = 0; i < 64; i+=4)
	  {
	    data |= sio_get_nibble();
	    data = data << 4;
	  }

	return data;
}

static u128 sio_get128(void)
{
	u128 data;
	data.hi = sio_get64 ();
	data.lo = sio_get64 ();

	return data;
}

static u64
read64 (u32 addr)
{
  u64 ret;
  u64 stor;
  asm volatile ("movq %%xmm0, %0\n"
                "movq (%2), %%xmm0\n"
		"movq %%xmm0, %1\n"
		"movq %0, %%xmm0":"+m"(stor),"=m"(ret):"r"(addr));

  return ret;
}

static void
write64 (u32 addr, u64 val)
{
  u64 stor;
  asm volatile ("movq %%xmm0, %0\n"
		"movq %%xmm0, %1\n"
                "movq (%2), %%xmm0\n"
		"movq %0, %%xmm0":"+m"(stor):"m"(val),"r"(addr));
}

static u128
read128 (u32 addr)
{
  u128 ret;
  u128 stor;
  asm volatile ("movdqu %%xmm0, %0\n"
                "movdqa (%2), %%xmm0\n"
		"movdqu %%xmm0, %1\n"
		"movdqu %0, %%xmm0":"+m"(stor),"=m"(ret):"r"(addr));

  return ret;
}

static void
write128 (u32 addr, u128 val)
{
  u128 stor;
  asm volatile ("movdqu %%xmm0, %0\n"
		"movdqu %%xmm0, %1\n"
                "movdqa (%2), %%xmm0\n"
		"movdqu %0, %%xmm0":"+m"(stor):"m"(val),"r"(addr));
}

/* Accessor functions */

static void serialice_read_memory(void)
{
	u8 width;
	u32 addr;

	// Format:
	// *rm00000000.w
	addr = sio_get32();
	sio_getc();	// skip .
	width = sio_getc();

	sio_putc('\r'); sio_putc('\n');

	switch (width) {
	case 'b': sio_put8(read8(addr)); break;
	case 'w': sio_put16(read16(addr)); break;
	case 'l': sio_put32(read32(addr)); break;
	case 'q': sio_put64(read64(addr)); break;
	case 'p': sio_put128(read128(addr)); break;
	}
}

static void serialice_clflush(void)
{
	u32 addr;

	// Format:
	// *fm00000000
	addr = sio_get32();

	sio_putc('\r'); sio_putc('\n');

	asm volatile ("clflush (%0)" : : "r" (addr));
}

static void serialice_write_memory(void)
{
	u8 width;
	u32 addr;
	u64 data;
	u128 data128;

	// Format:
	// *wm00000000.w=0000
	addr = sio_get32();
	sio_getc();	// skip .
	width = sio_getc();
	sio_getc();	// skip =

	switch (width) {
	case 'b': data = sio_get8(); write8(addr, (u8)data); break;
	case 'w': data = sio_get16(); write16(addr, (u16)data); break;
	case 'l': data = sio_get32(); write32(addr, (u32)data); break;
	case 'q': data = sio_get64(); write64(addr, data); break;
	case 'p': data128 = sio_get128(); write128(addr, data128); break;
	}
}

static void serialice_read_io(void)
{
	u8 width;
	u16 port;

	// Format:
	// *ri0000.w
	port = sio_get16();
	sio_getc();	// skip .
	width = sio_getc();

	sio_putc('\r'); sio_putc('\n');

	switch (width) {
	case 'b': sio_put8(inb(port)); break;
	case 'w': sio_put16(inw(port)); break;
	case 'l': sio_put32(inl(port)); break;
	}
}

static void serialice_write_io(void)
{
	u8 width;
	u16 port;
	u32 data;

	// Format:
	// *wi0000.w=0000
	port = sio_get16();
	sio_getc();	// skip .
	width = sio_getc();
	sio_getc();	// skip =

	switch (width) {
	case 'b': data = sio_get8(); outb((u8)data, port); break;
	case 'w': data = sio_get16(); outw((u16)data, port); break;
	case 'l': data = sio_get32(); outl((u32)data, port); break;
	}
}

static inline msr_t serialice_rdmsr(u32 index, u32 key)
{
	msr_t result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (index), "D" (key)
	);
	return result;
}

static inline void serialice_wrmsr(u32 index, msr_t msr, u32 key)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (key)
	);
}


static void serialice_read_msr(void)
{
	u32 addr, key;
	msr_t msr;

	// Format:
	// *rc00000000.9c5a203a
	addr = sio_get32();
	sio_getc();	   // skip .
	key = sio_get32(); // key in %edi

	sio_putc('\r'); sio_putc('\n');

	msr = serialice_rdmsr(addr, key);
	sio_put32(msr.hi);
	sio_putc('.');
	sio_put32(msr.lo);
}

static void serialice_write_msr(void)
{
	u32 addr, key;
	msr_t msr;

	// Format:
	// *wc00000000.9c5a203a=00000000.00000000
	addr = sio_get32();
	sio_getc();	// skip .
	key = sio_get32(); // read key in %edi
	sio_getc();	// skip =
	msr.hi = sio_get32();
	sio_getc();	// skip .
	msr.lo = sio_get32();

	serialice_wrmsr(addr, msr, key);
}

/* CPUID functions */

static inline u32 cpuid_eax(u32 op, u32 op2)
{
        u32 eax;

        __asm__("cpuid"
                : "=a" (eax)
                : "a" (op), "c" (op2)
                : "ebx", "edx" );
        return eax;
}

static inline u32 cpuid_ebx(u32 op, u32 op2)
{
        u32 ebx;

        __asm__("cpuid"
                : "=b" (ebx)
                : "a" (op), "c" (op2)
                : "edx");
        return ebx;
}

static inline u32 cpuid_ecx(u32 op, u32 op2)
{
        u32 ecx;

        __asm__("cpuid"
                : "=c" (ecx)
                : "a" (op), "c" (op2)
                : "ebx", "edx" );
        return ecx;
}

static inline u32 cpuid_edx(u32 op, u32 op2)
{
        u32 edx;

        __asm__("cpuid"
                : "=d" (edx)
                : "a" (op), "c" (op2)
                : "ebx");
        return edx;
}

static void serialice_cpuinfo(void)
{
	u32 eax, ecx;
	u32 reg32;

	// Format:
	//    --EAX--- --ECX---
	// *ci00000000.00000000
	eax = sio_get32();
	sio_getc(); // skip .
	ecx = sio_get32();

	sio_putc('\r'); sio_putc('\n');

	/* This code looks quite crappy but this way we don't
 	 * have to worry about running out of registers if we
 	 * occupy eax, ebx, ecx, edx at the same time 
 	 */
	reg32 = cpuid_eax(eax, ecx);
	sio_put32(reg32);
	sio_putc('.');

	reg32 = cpuid_ebx(eax, ecx);
	sio_put32(reg32);
	sio_putc('.');

	reg32 = cpuid_ecx(eax, ecx);
	sio_put32(reg32);
	sio_putc('.');

	reg32 = cpuid_edx(eax, ecx);
	sio_put32(reg32);
}

static void serialice_mainboard(void)
{
	sio_putc('\r'); sio_putc('\n');

	/* must be defined in mainboard/<boardname>.c */
	sio_putstring("Lenovo X201");
}

static void serialice_version(void)
{
	sio_putstring("\nSerialICE vphcoder (" __DATE__ ")\n");
}

static void s3_checksum (void)
{
		int i, j;
		u32 s;
		u32 b = 0;

		printk (BIOS_ERR, "S3 test\n")		;

		for (i = 0; i < 2048; i++)
		  {
		    s = 0;
		    for (j = ((i == 72) ? 0x800 : 0); j < (i ? 0x100000 : 0xa0000); j += 4)
		      {
			u32 v = *(u32 *) ((i << 20) | j);
			if (v != ((i << 20) | j) && i >= 1)
			  b++;
			s += v;
		      }
		    if (((u8 *)(72 << 20))[i] != s % 255)
		      {
			printk (BIOS_ERR, "MiB %d corrupted %x vs %x\n", i,
				((u8 *)(72 << 20))[i], s % 255);
		      }
		  }

		printk (BIOS_ERR, "S3 test end, bad=%x\n", b);
}


static void serialice (void)
{
	serialice_version();

	while(1) {
		u16 c;
		sio_putstring("\n> ");

		c = sio_getc();
		if (c != '*')
			continue;

		c = sio_getc() << 8;
		c |= sio_getc();

		switch(c) {
		case (('r' << 8)|'a'):
		{
			u32 addr, len;
			sio_getc();	// skip .
			addr = sio_get32();
			sio_getc();	// skip .
			len = sio_get32 ();
			ram_check (addr, len);
			break;
		}
		case (('c' << 8)|'s'):
			s3_checksum();
			break;
		case (('q' << 8)|'r'):
			quick_ram_check();
			break;
		case (('q' << 8)|'s'):
			return;
		case (('r' << 8)|'m'): // Read Memory *rm
			serialice_read_memory();
			break;
		case (('f' << 8)|'m'): // Flush Memory *fm
			serialice_clflush();
			break;
		case (('w' << 8)|'m'): // Write Memory *wm
			serialice_write_memory();
			break;
		case (('r' << 8)|'i'): // Read IO *ri
			serialice_read_io();
			break;
		case (('w' << 8)|'i'): // Write IO *wi
			serialice_write_io();
			break;
		case (('r' << 8)|'c'): // Read CPU MSR *rc
			serialice_read_msr();
			break;
		case (('w' << 8)|'c'): // Write CPU MSR *wc
			serialice_write_msr();
			break;
		case (('c' << 8)|'i'): // Read CPUID *ci
			serialice_cpuinfo();
			break;
		case (('m' << 8)|'b'): // Read mainboard type *mb
			serialice_mainboard();
			break;
		case (('v' << 8)|'i'): // Read version info *vi
			serialice_version();
			break;
		case (('s' << 8)|'i'):
			raminit (0);
			break;
		case (('s' << 8)|'3'):
			raminit (1);
			break;
		case (('s' << 8)|'f'):
			raminit (2);
			break;
		default:
			sio_putstring("ERROR\n");
			break;
		}
	}
}
#endif

#if CONFIG_COLLECT_TIMESTAMPS
tsc_t before_spd CAR_GLOBAL, after_spd CAR_GLOBAL, before_training CAR_GLOBAL, after_training CAR_GLOBAL;
#endif

void main(unsigned long bist)
{
	u32 reg32;
	int s3resume = 0;
#if CONFIG_COLLECT_TIMESTAMPS
        tsc_t start_romstage_time;
        tsc_t before_dram_time;
        tsc_t after_dram_time;
        tsc_t base_time = rdtsc ();
#endif

#if CONFIG_COLLECT_TIMESTAMPS
	start_romstage_time = base_time;
#endif

	if (bist == 0)
		enable_lapic();

	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	pci_write_config16(PCI_DEV(0, 0, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);
	pci_write_config16(PCI_DEV(0, 0, 0), BCTRL, 0);

	/* Enable USB Power.  */
	ec_set_bit(0x3b, 4);

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	inw (DEFAULT_GPIOBASE | 0x38); // = 0x10
	outl (0x7963a5ff, DEFAULT_GPIOBASE);
	outl (0xffffffff, DEFAULT_GPIOBASE | 0xc);
	outl (0x87bf6aff, DEFAULT_GPIOBASE | 0x4);
	outl (0x0, DEFAULT_GPIOBASE | 0x18);
	outl (0x120c6, DEFAULT_GPIOBASE | 0x2c);
	outl (0x27706fe, DEFAULT_GPIOBASE | 0x30);
	outl (0x29fffff, DEFAULT_GPIOBASE | 0x38);
	outl (0x1b01f9f4, DEFAULT_GPIOBASE | 0x34);
	outl (0x0, DEFAULT_GPIOBASE | 0x40);
	outl (0x0, DEFAULT_GPIOBASE | 0x48);
	outl (0xf00, DEFAULT_GPIOBASE | 0x44);
	outl (0x41000000, DEFAULT_GPIOBASE | 0x60);

	nehalem_early_initialization(NEHALEM_MOBILE);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	console_init();

#ifdef SERIALICE
	serialice ();
#endif

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Read PM1_CNT */
	reg32 = inl(DEFAULT_PMBASE + 0x04);
	printk(BIOS_DEBUG, "PM1_CNT: %08x\n", reg32);
	if (((reg32 >> 10) & 7) == 5) {
	  u8 reg8;
	  reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa2);
	  printk(BIOS_DEBUG, "a2: %02x\n", reg8);
	  if (!(reg8 & 0x20))
	    {
	      outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
	      printk(BIOS_DEBUG, "Bad resume from S3 detected.\n");
	    }
	  else
	    {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		s3resume = 1;
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	    }
	}

#ifndef SERIALICE
#if CONFIG_COLLECT_TIMESTAMPS
        before_dram_time = rdtsc ();
#endif
	raminit(s3resume);
#if CONFIG_COLLECT_TIMESTAMPS
        after_dram_time = rdtsc ();
#endif
#endif

	intel_early_me_status();

	if (s3resume)
	  {
	    /* Clear SLP_TYPE. This will break stage2 but
	     * we care for that when we get there.
	     */
	    reg32 = inl(DEFAULT_PMBASE + 0x04);
	    outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
	  }

#if CONFIG_HAVE_ACPI_RESUME
	/* Start address of high memory tables */
	unsigned long high_ram_base = get_top_of_ram() - HIGH_MEMORY_SIZE;

	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */
	if (s3resume && cbmem_reinit((u64)high_ram_base)) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
#if 0
		s3_checksum ();
#endif
		/*		for (i = 0; i < 65536; i++)
		  if (read8 (i) != read8 (i + 0x100000))
			printk (BIOS_ERR, "Corruption at %x: %x vs %x\n", i,
			read8 (i), read8 (i + 0x100000));*/


		/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
		 * through stage 2. We could keep stuff like stack and heap in high tables
		 * memory completely, but that's a wonderful clean up task for another
		 * day.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)CONFIG_RAMBASE, HIGH_MEMORY_SAVE);
#if 0
		printk (BIOS_ERR, "move: %x, %x, %x\n",(unsigned)resume_backup_memory, (unsigned)CONFIG_RAMBASE, (unsigned)HIGH_MEMORY_SAVE);

		s = 0;
		for (j = 0; j <  0x100000; j += 4)
		  s += ((u32 *)resume_backup_memory)[j / 4];
		if (((u8 *)(72 << 20))[1] != s % 255)
		  {
		    printk (BIOS_ERR, "MiB 1 (copy) corrupted %x vs %x\n",
			    ((u8 *)(72 << 20))[1], s % 255);
		  }
		else
		    printk (BIOS_ERR, "MiB 1 (copy) ok %x == %x\n",
			    ((u8 *)(72 << 20))[1], s % 255);
#endif
		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafed00d);
	}
	else if (s3resume) {
		printk(BIOS_ERR, "Failed S3 resume.\n");
		ram_check (0x100000, 0x200000);

		/* Failed S3 resume, reset to come up cleanly */
		outb(0xe, 0xcf9);
		hlt();
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
		quick_ram_check();
	}
#endif

#if CONFIG_COLLECT_TIMESTAMPS
        timestamp_init(base_time);
        timestamp_add(TS_START_ROMSTAGE, start_romstage_time);
        timestamp_add(TS_BEFORE_INITRAM, before_dram_time);
        timestamp_add(101, before_spd);
        timestamp_add(102, after_spd);
        timestamp_add(103, before_training);
        timestamp_add(104, after_training);
        timestamp_add(TS_AFTER_INITRAM, after_dram_time);
        timestamp_add_now(TS_END_ROMSTAGE);
#endif

#if CONFIG_CONSOLE_CBMEM
        /* Keep this the last thing this function does. */
        cbmemc_reinit();
#endif
}
