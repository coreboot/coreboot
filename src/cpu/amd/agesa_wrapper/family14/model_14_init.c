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

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>

#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/multicore.h>
#include <cpu/amd/amdfam14.h>

#define MCI_STATUS 0x401

msr_t rdmsr_amd(u32 index)
{
  msr_t result;
  __asm__ __volatile__(
    "rdmsr"
    :"=a"(result.lo), "=d"(result.hi)
    :"c"(index), "D"(0x9c5a203a)
  );
  return result;
}

void wrmsr_amd(u32 index, msr_t msr)
{
  __asm__ __volatile__(
    "wrmsr"
    : /* No outputs */
    :"c"(index), "a"(msr.lo), "d"(msr.hi), "D"(0x9c5a203a)
  );
}

static void model_14_init(device_t dev)
{
  printk(BIOS_DEBUG, "Model 14 Init - a no-op.\n");

  u8 i;
  msr_t msr;
  int msrno;
  struct node_core_id id;
#if CONFIG_LOGICAL_CPUS == 1
  u32 siblings;
#endif

//  id = get_node_core_id(read_nb_cfg_54());  /* nb_cfg_54 can not be set */
//  printk(BIOS_DEBUG, "nodeid = %02d, coreid = %02d\n", id.nodeid, id.coreid);

  disable_cache ();
  /* Enable access to AMD RdDram and WrDram extension bits */
  msr = rdmsr(SYSCFG_MSR);
  msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
  wrmsr(SYSCFG_MSR, msr);

   // BSP: make a0000-bffff UC, c0000-fffff WB, same as OntarioApMtrrSettingsList for APs
   msr.lo = msr.hi = 0;
   wrmsr (0x259, msr);
   msr.lo = msr.hi = 0x1e1e1e1e;
   for (msrno = 0x268; msrno <= 0x26f; msrno++)
      wrmsr (msrno, msr);

  /* disable access to AMD RdDram and WrDram extension bits */
  msr = rdmsr(SYSCFG_MSR);
  msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
  wrmsr(SYSCFG_MSR, msr);
  enable_cache ();

  /* zero the machine check error status registers */
  msr.lo = 0;
  msr.hi = 0;
  for (i = 0; i < 6; i++) {
    wrmsr(MCI_STATUS + (i * 4), msr);
  }

  /* Enable the local cpu apics */
  setup_lapic();

  /* Set the processor name string */
//  init_processor_name();


#if CONFIG_LOGICAL_CPUS == 1
  siblings = cpuid_ecx(0x80000008) & 0xff;

  if (siblings > 0) {
    msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
    msr.lo |= 1 << 28;
    wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

    msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
    msr.hi |= 1 << (33 - 32);
    wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
  }
  printk(BIOS_DEBUG, "siblings = %02d, ", siblings);
#endif

  /* DisableCf8ExtCfg */
  msr = rdmsr(NB_CFG_MSR);
  msr.hi &= ~(1 << (46 - 32));
  wrmsr(NB_CFG_MSR, msr);


  /* Write protect SMM space with SMMLOCK. */
  msr = rdmsr(HWCR_MSR);
  msr.lo |= (1 << 0);
  wrmsr(HWCR_MSR, msr);
}

static struct device_operations cpu_dev_ops = {
  .init = model_14_init,
};

static struct cpu_device_id cpu_table[] = {
  { X86_VENDOR_AMD, 0x500f00 },   /* ON-A0 */
  { X86_VENDOR_AMD, 0x500f01 },   /* ON-A1 */
  { X86_VENDOR_AMD, 0x500f10 },   /* ON-B0 */
  { 0, 0 },
};

static const struct cpu_driver model_14 __cpu_driver = {
  .ops      = &cpu_dev_ops,
  .id_table = cpu_table,
};
