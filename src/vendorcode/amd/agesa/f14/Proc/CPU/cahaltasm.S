/*
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

.include "src/vendorcode/amd/agesa/f14/gcccar.inc"

.code32
.align 4
.globl ExecuteFinalHltInstruction
    .type   ExecuteFinalHltInstruction, @function
/* ExecuteFinalHltInstruction (
  IN       UINT32 HaltFlags,
  IN       AP_MTRR_SETTINGS  *ApMtrrSettingsList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
*/

/* This function disables CAR. We don't care about the stack on this CPU */
ExecuteFinalHltInstruction:
  movl 4(%esp),  %esi               /* HaltFlags*/
  movl 8(%esp),  %edi               /* ApMtrrSettingList */

/*  Do these special steps in case if the core is part of a compute unit
 *  Note: The following bits are family specific flags, that gets set during build time,
 *           and indicates things like "family cache control methodology", etc.
 *  esi bit0 = 0  -> not a Primary core
 *  esi bit0 = 1  -> Primary core
 *  esi bit1 = 0  -> Cache disable
 *  esi bit1 = 1  -> Cache enable
 */

  bt $1, %esi                     /* .if (esi & 2h) */
  jz  0f
    /* Set CombineCr0Cd bit */
    movl $CU_CFG3,  %ecx
    rdmsr
    bts $(COMBINE_CR0_CD - 32),  %edx
    wrmsr
    /* Clear the CR0.CD bit */
    movl %cr0,  %eax                /* Make sure cache is enabled for all APs */
    btr $CR0_CD,  %eax
    btr $CR0_NW,  %eax
    mov %eax,  %cr0                 /*  Write back to CR0 */
    jmp 1f                          /* .else */
0:
    movl %cr0,  %eax                /* Make sure cache is disabled for all APs */
    bts $CR0_CD,  %eax              /* Disable cache */
    bts $CR0_NW,  %eax
    movl %eax,  %cr0                /* Write back to CR0 */
1:                                  /* .endif */

  bt $0,  %esi                     /* .if (esi & 1h) */
  jz  2f
    /* This core is a primary core and needs to do all the MTRRs, including shared MTRRs. */
    movl %edi,  %esi                /* Get ApMtrrSettingList */

    /* Configure the MTRRs on the AP so
     * when it runs remote code it will execute
     * out of RAM instead of ROM.
     */

    /* Disable MTRRs and turn on modification enable bit */
    movl $MTRR_SYS_CFG, %ecx
    rdmsr
    btr $MTRR_VAR_DRAM_EN,  %eax      /* Disable */
    bts $MTRR_FIX_DRAM_MOD_EN,  %eax  /* Enable */
    btr $MTRR_FIX_DRAM_EN,  %eax      /* Disable */
    bts $SYS_UC_LOCK_EN,  %eax
    wrmsr

    /* Setup default values for Fixed-Sized MTRRs */
    /* Set 7FFFh-00000h as WB */
    movl $AMD_AP_MTRR_FIX64k_00000,  %ecx
    movl $0x1E1E1E1E,  %eax
    movl %eax,  %edx
    wrmsr

    /* Set 9FFFFh-80000h also as WB */
    movl $AMD_AP_MTRR_FIX16k_80000,  %ecx
    wrmsr

    /* Set BFFFFh-A0000h as Uncacheable Memory-mapped IO */
    movl $AMD_AP_MTRR_FIX16k_A0000,  %ecx
    xorl %eax,  %eax
    xorl %edx,  %edx
    wrmsr

    /* Set DFFFFh-C0000h as Uncacheable Memory-mapped IO */
    xorl %eax,  %eax
    xorl %edx,  %edx
    movl $AMD_AP_MTRR_FIX4k_C0000,  %ecx

CDLoop:
    wrmsr
    inc %ecx
    cmp $AMD_AP_MTRR_FIX4k_D8000,  %ecx
    jbe CDLoop

    /* Set FFFFFh-E0000h as Uncacheable Memory */
    movl $0x18181818,  %eax
    movl %eax,  %edx

    mov $AMD_AP_MTRR_FIX4k_E0000, %ecx

EFLoop:
    wrmsr
    inc %ecx
    cmp $AMD_AP_MTRR_FIX4k_F8000, %ecx
    jbe EFLoop

    /* If IBV provided settings for Fixed-Sized MTRRs,
     * overwrite the default settings. */
    cmp $0,  %esi           /*.if ((esi != 0) && (esi != 0FFFFFFFFh)) */
    jz 4f
    cmp $0xFFFFFFFF,  %esi
    jz 4f
      5:
      mov (%esi),  %ecx         /* (AP_MTRR_SETTINGS ptr [esi]).MsrAddr */
      /* While we are not at the end of the list */
      cmp $CPU_LIST_TERMINAL,  %ecx /* .while (ecx != CPU_LIST_TERMINAL)*/
      je 4f
        /* TODO - coreboot isn't checking for valid data.
         * Ensure that the MSR address is valid for Fixed-Sized MTRRs */
        /*.if ( ((ecx >= AMD_AP_MTRR_FIX4k_C0000) && (ecx <= AMD_AP_MTRR_FIX4k_F8000)) || \
               (ecx == AMD_AP_MTRR_FIX64k_00000) || (ecx == AMD_AP_MTRR_FIX16k_80000 ) || \
               (ecx == AMD_AP_MTRR_FIX16k_A0000))
         */
          mov 4(%esi),  %eax                /* MsrData */
          mov 8(%esi),  %edx                /* MsrData */
          wrmsr
        /* .endif */
        add $12,  %esi                   /* sizeof (AP_MTRR_SETTINGS) */
        jmp 5b                               /* .endw */
    4: /* .endif */

    /* restore variable MTTR6 and MTTR7 to default states */
    movl $AMD_MTRR_VARIABLE_BASE6,  %ecx  /* clear MTRRPhysBase6 MTRRPhysMask6 */
    xor %eax,  %eax                         /* and MTRRPhysBase7 MTRRPhysMask7 */
    xor %edx,  %edx
    cmp $10,  %ecx                      /* .while (cl < 010h) */
    jge 6f
      wrmsr
      inc %ecx
    6:                                  /* .endw */

    /* Enable fixed-range and variable-range MTRRs */
    mov $AMD_MTRR_DEFTYPE,  %ecx
    rdmsr
    bts $MTRR_DEF_TYPE_EN,  %eax      /* MtrrDefTypeEn */
    bts $MTRR_DEF_TYPE_FIX_EN,  %eax  /* MtrrDefTypeFixEn */
    wrmsr

    /* Enable Top-of-Memory setting */
    /* Enable use of RdMem/WrMem bits attributes */
    mov $MTRR_SYS_CFG,  %ecx
    rdmsr
    bts $MTRR_VAR_DRAM_EN,  %eax       /* Enable */
    btr $MTRR_FIX_DRAM_MOD_EN,  %eax   /* Disable */
    bts $MTRR_FIX_DRAM_EN,  %eax       /* Enable */
    wrmsr

    bts $FLAG_IS_PRIMARY,  %esi
    jmp 3f /* .else                            ; end if primary core */
  2:
    xor %esi,  %esi
  3: /* .endif*/

  /* Make sure not to touch any Shared MSR from this point on */

  AMD_DISABLE_STACK_FAMILY_HOOK

  xor  %eax,  %eax

7:
  cli
  hlt
  jmp 7b  /* ExecuteHltInstruction */

    .size   ExecuteFinalHltInstruction, .-ExecuteFinalHltInstruction
