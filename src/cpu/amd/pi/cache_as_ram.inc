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
  orl $(3 << 9), %eax
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

  /* Align the stack. */
  and     $0xFFFFFFF0, %esp

#ifdef __x86_64__
  /* switch to 64 bit long mode */
  mov     %esi, %ecx
  add     $0, %ecx # core number
  xor     %eax, %eax
  lea     (0x1000+0x23)(%ecx), %edi
  mov     %edi, (%ecx)
  mov     %eax, 4(%ecx)

  lea     0x1000(%ecx), %edi
  movl    $0x000000e3, 0x00(%edi)
  movl    %eax, 0x04(%edi)
  movl    $0x400000e3, 0x08(%edi)
  movl    %eax, 0x0c(%edi)
  movl    $0x800000e3, 0x10(%edi)
  movl    %eax, 0x14(%edi)
  movl    $0xc00000e3, 0x18(%edi)
  movl    %eax, 0x1c(%edi)

  # load ROM based identity mapped page tables
  mov     %ecx, %eax
  mov     %eax, %cr3

  # enable PAE
  mov     %cr4, %eax
  bts     $5, %eax
  mov     %eax, %cr4

  # enable long mode
  mov     $0xC0000080, %ecx
  rdmsr
  bts     $8, %eax
  wrmsr

  # enable paging
  mov     %cr0, %eax
  bts     $31, %eax
  mov     %eax, %cr0

  # use call far to switch to 64-bit code segment
  ljmp $0x18, $1f
1:
  /* Pass the cpu_init_detected */
  cvtsd2si        %xmm1, %esi

  /* Pass the BIST result */
  cvtsd2si        %xmm0, %edi


  .code64
  call    cache_as_ram_main
  .code32

#else

  /* Restore the BIST result */
  cvtsd2si  %xmm0, %edx

  /* Restore the  cpu_init_detected */
  cvtsd2si  %xmm1, %ebx

  /* Must maintain 16-byte stack alignment here. */
  pushl $0x0
  pushl $0x0
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
