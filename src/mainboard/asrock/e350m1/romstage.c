/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <console/loglevel.h>
#include "agesawrapper.h"
#include "cpu/x86/bist.h"
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "pc80/i8254.c"
#include "pc80/i8259.c"
#include "SbEarly.h"
#include "SBPLATFORM.h"

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx);

#define SERIAL_DEV PNP_DEV(CONFIG_SIO_PORT, W83627HF_SP1)

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
  u32 val;
  u8 reg8;

  // all cores: set pstate 0 (1600 MHz) early to save a few ms of boot time
  __writemsr (0xc0010062, 0);

  // early enable of PrefetchEnSPIFromHost
  if (boot_cpu())
    {
    __outdword (0xcf8, 0x8000a3b8);
    __outdword (0xcfc, __indword (0xcfc) | 1 << 24);
    }

  // early enable of SPI 33 MHz fast mode read
  if (boot_cpu())
    {
    volatile u32 *spiBase = (void *) 0xa0000000;
    u32 save;
    __outdword (0xcf8, 0x8000a3a0);
    save = __indword (0xcfc);
    __outdword (0xcfc, (u32) spiBase | 2); // set temp MMIO base
    spiBase [3] = (spiBase [3] & ~(3 << 14)) | (1 << 14);
    spiBase [0] |= 1 << 18; // fast read enable
    __outdword (0xcfc, save); // clear temp base
    }

  if (!cpu_init_detectedx && boot_cpu()) {
    post_code(0x30);
    sb_poweron_init();

    post_code(0x31);
    w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
    console_init();
  }
	//reg8 = pmio_read(0x24);
  outb(0x24, 0xCD6);
  reg8 = inb(0xCD7);
	reg8 |= 1;
	reg8 &= ~(1 << 1);
	//pmio_write(0x24, reg8);
	outb(0x24, 0xCD6);
	outb(reg8, 0xCD7);

	*(volatile u32 *)(0xFED80000+0xE00+0x40) &= ~((1 << 0) | (1 << 2)); /* 48Mhz */
	*(volatile u32 *)(0xFED80000+0xE00+0x40) |= 1 << 1; /* 48Mhz */

  /* Halt if there was a built in self test failure */
  post_code(0x34);
  report_bist_failure(bist);

  // Load MPB
  val = cpuid_eax(1);
  printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
  printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

  post_code(0x35);
  val = agesawrapper_amdinitmmio();

  post_code(0x37);
  val = agesawrapper_amdinitreset();
  if(val) {
    printk(BIOS_DEBUG, "agesawrapper_amdinitreset failed: %x \n", val);
  }

  post_code(0x38);
  printk(BIOS_DEBUG, "Got past sb800_early_setup\n");

  post_code(0x39);
  val = agesawrapper_amdinitearly ();
  if(val) {
    printk(BIOS_DEBUG, "agesawrapper_amdinitearly failed: %x \n", val);
  }
  printk(BIOS_DEBUG, "Got past agesawrapper_amdinitearly\n");

  post_code(0x40);
  val = agesawrapper_amdinitpost ();
  if(val) {
    printk(BIOS_DEBUG, "agesawrapper_amdinitpost failed: %x \n", val);
  }
  printk(BIOS_DEBUG, "Got past agesawrapper_amdinitpost\n");

  post_code(0x41);
  val = agesawrapper_amdinitenv ();
  if(val) {
    printk(BIOS_DEBUG, "agesawrapper_amdinitenv failed: %x \n", val);
  }
  printk(BIOS_DEBUG, "Got past agesawrapper_amdinitenv\n");

  /* Initialize i8259 pic */
  post_code(0x41);
  setup_i8259 ();

  /* Initialize i8254 timers */
  post_code(0x42);
  setup_i8254 ();

  post_code(0x50);
  copy_and_run(0);

  post_code(0x54);  // Should never see this post code.
}

