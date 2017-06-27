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

.code32
.globl cache_as_ram_setup, cache_as_ram_setup_out

cache_as_ram_setup:

  /* Preserve BIST. */
  movl %eax, %ebp

  post_code(0xa0)

  /* enable SSE2 128bit instructions */
  /* Turn on OSFXSR [BIT9] and OSXMMEXCPT [BIT10] onto CR4 register */

  movl %cr4, %eax
  orl $(3 << 9), %eax
  movl %eax, %cr4

  post_code(0xa1)

  /* NOTE: %ebx, %ebp are preserved in AMD_ENABLE_STACK. */
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

#endif

  /* Calling conventions preserve BIST in %ebp. */

  call early_all_cores

  /* Must maintain 16-byte stack alignment here. */
  pushl $0x0
  pushl $0x0
  pushl $0x0
  pushl %ebp
  call  romstage_main
  movl  %eax, %ebx

/* Register %ebx is new stacktop for remaining of romstage.
 * It is the only register preserved in AMD_DISABLE_STACK.
 */

disable_cache_as_ram:
  /* Disable cache */
  movl	%cr0, %eax
  orl	$CR0_CacheDisable, %eax
  movl	%eax, %cr0

  AMD_DISABLE_STACK

  /* enable cache */
  movl %cr0, %eax
  andl $0x9fffffff, %eax
  movl %eax, %cr0

  movl  %ebx, %esp
  call  romstage_after_car

  /* Should never see this postcode */
  post_code(0xaf)
stop:
  jmp stop

cache_as_ram_setup_out:
