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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/******************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * $Workfile:: cache_as_ram.inc
 *
 * Description: cache_as_ram.inc - AGESA Module Entry Point for GCC complier
 *
 ******************************************************************************
 */

#include "gcccar.inc"
#include <cpu/x86/cache.h>

/*
 * XMM map:
 *   xmm0: BIST
 *   xmm1: backup ebx -- cpu_init_detected
 */

.code32
.globl cache_as_ram_setup, disable_cache_as_ram, cache_as_ram_setup_out

cache_as_ram_setup:

  post_code(0xa0)

  /* enable SSE2 128bit instructions */
  /* Turn on OSFXSR [BIT9] and OSXMMEXCPT [BIT10] onto CR4 register */

  movl %cr4, %eax
  orl $(3<<9), %eax
  movl %eax, %cr4

  /* Get the cpu_init_detected */
  mov $1, %eax
  cpuid
  shr $24, %ebx

  /* Save the BIST result */
  cvtsi2sd  %ebp, %xmm0

  /* for normal part %ebx already contain cpu_init_detected from fallback call */

  /* Save the cpu_init_detected */
  cvtsi2sd  %ebx, %xmm1

  post_code(0xa1)
  AMD_ENABLE_STACK

  post_code(0xa1)

  /* Restore the BIST result */
  cvtsd2si  %xmm0, %edx

  /* Restore the  cpu_init_detected */
  cvtsd2si  %xmm1, %ebx

  pushl %ebx  /* init detected */
  pushl %edx  /* bist */
  call  cache_as_ram_main

  /* Should never see this postcode */
  post_code(0xaf)
stop:
  jmp stop

disable_cache_as_ram:
  /* Save return stack */
  movd 0(%esp), %xmm1
  movd %esp, %xmm0

  /* Disable cache */
  movl	%cr0, %eax
  orl	$CR0_CacheDisable, %eax
  movl	%eax, %cr0

  AMD_DISABLE_STACK

  /* enable cache */
  movl %cr0, %eax
  andl $0x9fffffff, %eax
  movl %eax, %cr0
  xorl %eax, %eax

  /* Restore the return stack */
  wbinvd
  movd %xmm0, %esp
  movd %xmm1, (%esp)
  ret

cache_as_ram_setup_out:
