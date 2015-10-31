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
#ifdef __x86_64__
  /* switch to 64 bit long mode */
  .intel_syntax noprefix

  mov     ecx, esi
  add     ecx, 0 # core number
  xor     eax, eax
  lea     edi, [ecx+0x1000+0x23]
  mov     dword ptr [ecx+0], edi
  mov     dword ptr [ecx+4], eax

  lea     edi, [ecx+0x1000]
  mov     dword ptr [edi+0x00], 0x000000e3
  mov     dword ptr [edi+0x04], eax
  mov     dword ptr [edi+0x08], 0x400000e3
  mov     dword ptr [edi+0x0c], eax
  mov     dword ptr [edi+0x10], 0x800000e3
  mov     dword ptr [edi+0x14], eax
  mov     dword ptr [edi+0x18], 0xc00000e3
  mov     dword ptr [edi+0x1c], eax

  # load rom based identity mapped page tables
  mov     eax, ecx
  mov     cr3,eax

  # enable PAE
  mov     eax, cr4
  bts     eax, 5
  mov     cr4, eax

  # enable long mode
  mov     ecx, 0xC0000080
  rdmsr
  bts     eax, 8
  wrmsr

  # enable paging
  mov     eax, cr0
  bts     eax, 31
  mov     cr0, eax

  # use call far to switch to 64-bit code segment
  jmp 0x18,.+7

  /* Pass the BIST result */
  cvtsd2si        esi, xmm1

  /* Pass the cpu_init_detected */
  cvtsd2si        edi, xmm0

  /* align the stack */
  and     esp, 0xFFFFFFF0

  .code64
  call    cache_as_ram_main
  .code32

  .att_syntax prefix
#else
  /* Restore the BIST result */
  cvtsd2si  %xmm0, %edx

  /* Restore the  cpu_init_detected */
  cvtsd2si  %xmm1, %ebx

  pushl %ebx  /* init detected */
  pushl %edx  /* bist */
  call  cache_as_ram_main
#endif

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
#ifdef __x86_64__
.code64
#endif
