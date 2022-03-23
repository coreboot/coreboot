/*
 * Copyright (c) 2012, Advanced Micro Devices, Inc.
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

/******************************************************************************
* AMD Generic Encapsulated Software Architecture
*
* $Workfile:: GccCar.inc    $Revision:: 32932   $
*
* Description: GccCar.inc - AGESA cache-as-RAM setup Include File for GCC complier
*
******************************************************************************/

.altmacro

BSP_STACK_BASE_ADDR     =       0x30000         /* Base address for primary cores stack   */
BSP_STACK_SIZE          =       0x10000         /* 64KB for BSP core                      */
CORE0_STACK_BASE_ADDR   =       0x80000         /* Base address for primary cores stack   */
CORE0_STACK_SIZE        =       0x4000          /* 16KB for primary cores                 */
CORE1_STACK_BASE_ADDR   =       0x40000         /* Base address for AP cores              */

#ifdef __x86_64__
CORE1_STACK_SIZE        =       0x2000          /* 8KB for each AP cores                  */
#else
CORE1_STACK_SIZE        =       0x1000          /* 4KB for each AP cores                  */
#endif

APIC_BASE_ADDRESS       =       0x0000001B
  APIC_BSC              =       8               /* Boot Strap Core  */

APIC_MSG_REG            = 0x380        # Location of BSC message
    APIC_MSG            = 0x00DE00AD   # Message data
    APIC_INVD_ALL_DONE_MSG  =     0x00AD00DE  /* Indicate all cores have invalidated */
APIC_CMD_LO_REG         = 0x300        # APIC command low
APIC_CMD_HI_REG         = 0x310        # APIC command high
    REMOTE_READ_STS       = 0x00030000 # Remote read status mask
    REMOTE_DELIVERY_PEND  = 0x00010000 # Remote read is pending
    REMOTE_DELIVERY_DONE  = 0x00020000 # Remote read is complete
    DELIVERY_STS_BIT    = 12          #Delivery status valid bit
APIC_ID_REG             = 0x0020       # Local APIC ID offset
    APIC20_APICID       = 24
APIC_REMOTE_READ_REG    = 0x00C0       # Remote read offset

# Flags can only run from bits 31 to 24.  Bits 23:0 are in use.
AMD_CU_NEED_TO_WAIT     = 31
AMD_CU_SEND_INVD_MSG    = 30
AMD_CU_RESTORE_ES       = 29

AMD_MTRR_VARIABLE_BASE0  =      0x0200
AMD_MTRR_VARIABLE_BASE6  =      0x020C
AMD_MTRR_VARIABLE_BASE7 = 0x020E
    VMTRR_VALID             =     11
    MTRR_TYPE_WB            =     0x06
    MTRR_TYPE_WP            =     0x05
    MTRR_TYPE_WT            =     0x04
    MTRR_TYPE_UC            =     0x00
AMD_MTRR_VARIABLE_MASK7 = 0x020F
AMD_MTRR_FIX64k_00000    =      0x0250
AMD_MTRR_FIX16k_80000    =      0x0258
AMD_MTRR_FIX16k_A0000    =      0x0259
AMD_MTRR_FIX4k_C0000     =      0x0268
AMD_MTRR_FIX4k_C8000     =      0x0269
AMD_MTRR_FIX4k_D0000     =      0x026A
AMD_MTRR_FIX4k_D8000     =      0x026B
AMD_MTRR_FIX4k_E0000     =      0x026C
AMD_MTRR_FIX4k_E8000     =      0x026D
AMD_MTRR_FIX4k_F0000     =      0x026E
AMD_MTRR_FIX4k_F8000     =      0x026F

/* Reproduced from AGESA.h */
AMD_AP_MTRR_FIX64k_00000  =  0x00000250
AMD_AP_MTRR_FIX16k_80000  =  0x00000258
AMD_AP_MTRR_FIX16k_A0000  =  0x00000259
AMD_AP_MTRR_FIX4k_C0000   =  0x00000268
AMD_AP_MTRR_FIX4k_C8000   =  0x00000269
AMD_AP_MTRR_FIX4k_D0000   =  0x0000026A
AMD_AP_MTRR_FIX4k_D8000   =  0x0000026B
AMD_AP_MTRR_FIX4k_E0000   =  0x0000026C
AMD_AP_MTRR_FIX4k_E8000   =  0x0000026D
AMD_AP_MTRR_FIX4k_F0000   =  0x0000026E
AMD_AP_MTRR_FIX4k_F8000   =  0x0000026F
CPU_LIST_TERMINAL         =  0xFFFFFFFF

AMD_MTRR_DEFTYPE         =      0x02FF
    WB_DRAM_TYPE         =      0x1E             /* MemType - memory type */
    MTRR_DEF_TYPE_EN     =      11               /* MtrrDefTypeEn - variable and fixed MTRRs default enabled */
    MTRR_DEF_TYPE_FIX_EN =      10               /* MtrrDefTypeEn - fixed MTRRs default enabled */

HWCR                     =      0x0C0010015      /* Hardware Configuration                                                                                                                                       */
    INVD_WBINVD          =      0x04             /* INVD to WBINVD conversion */

IORR_BASE                =      0x0C0010016      /* IO Range Regusters Base/Mask, 2 pairs */
                                                 /*  uses 16h - 19h                                                                                                                                                             */
TOP_MEM                  =      0x0C001001A      /* Top of Memory                                                                                                                                                                                */
TOP_MEM2                 =      0x0C001001D      /* Top of Memory2                                                                                                                                                                       */

    DIS_STREAM_ST        =     28                /* Family 14h:DisStreamSt - Disable Streaming Store functionality */

IC_CFG                   =      0x0C0011021      /* Instruction Cache Config Register  */
    IC_DIS_SPEC_TLB_RLD  =      9                /*   Disable speculative TLB reloads  */
    DIS_IND              =      14               /*   Family 10-14h:Disable Indirect Branch Predictor */
    DIS_I_CACHE          =      14               /*   Family 15h:DisICache - Disable Indirect Branch Predictor */

DC_CFG                   =      0x0C0011022      /* Data Cache Configuration                                                                                                                                   */
    DC_DIS_SPEC_TLB_WALK     =  4                /* Disable speculative table-walks */
    DIS_HW_PF                =  13               /*   Hardware prefetches bit                                                                                                                                    */
DE_CFG                   =      0x0C0011029      /* Decode Configuration */
    CL_FLUSH_SERIALIZE   =      23               /*   Family 12h,15h: CL Flush Serialization */

BU_CFG2                  =      0x0C001102A      /* Family 10h: Bus Unit Configuration 2 */
CU_CFG2                  =      0x0C001102A      /* Family 15h: Combined Unit Configuration 2 */
    F10_CL_LINES_TO_NB_DIS  =   15               /*   ClLinesToNbDis - allows WP code to be cached in L2 */
    IC_DIS_SPEC_TLB_WR      =   35               /*   IcDisSpecTlbWr - ITLB speculative writes */
    F16_CL_LINES_TO_L2_DIS  =   15      /*   ClLinesToL2Dis */

L2I_CFG                  = 0x0C00110A0  /* L2I Configuration */
    L2_RINSER_DIS          = 20           /*   L2 rinser disable */
    PREFETCHER_DIS        =  7           /* L2 prefetcher disable*/
    CACHE_IC_ATTR_DIS     =  3           /*  Inserting IC attributes into the L2 disable */

CR0_PE                  = 0           # Protection Enable
CR0_NW                  = 29          # Not Write-through
CR0_CD                  = 30          # Cache Disable
CR0_PG                  = 31          # Paging Enable

/* CPUID Functions */

CPUID_MODEL              =      1
AMD_CPUID_FMF            =      0x80000001       /* Family Model Features information */
AMD_CPUID_L2Cache        =      0x80000006       /* L2/L3 cache info */
AMD_CPUID_APIC           =      0x80000008       /* Long Mode and APIC info., core count */
    APIC_ID_CORE_ID_SIZE      = 12               /* ApicIdCoreIdSize bit position */

NB_CFG                   =      0x0C001001F      /* Northbridge Configuration Register */
    INIT_APIC_ID_CPU_ID_LO    = 54               /*  InitApicIdCpuIdLo - is core# in high or low half of APIC ID? */
    ENABLE_CF8_EXT_CFG        = 46               /*  EnableCf8ExtCfg - enable CF8 extended configuration cycles */

MTRR_SYS_CFG             =      0x0C0010010      /* System Configuration Register */
  CHX_TO_DIRTY_DIS       =      16               /*   ChxToDirtyDis    Change to dirty disable  */
  SYS_UC_LOCK_EN         =      17               /*   SysUcLockEn      System lock command enable */
  MTRR_FIX_DRAM_EN       =      18               /*   MtrrFixDramEn    MTRR fixed RdDram and WrDram attributes enable */
  MTRR_FIX_DRAM_MOD_EN   =      19               /*   MtrrFixDramModEn MTRR fixed RdDram and WrDram modification enable */
  MTRR_VAR_DRAM_EN       =      20               /*   MtrrVarDramEn    MTRR variable DRAM enable */
  MTRR_TOM2_EN           =      21               /*   MtrrTom2En       MTRR top of memory 2 enable */

PERF_CONTROL3            =      0x0C0010003      /* Performance event control three */
    PERF_CONTROL3_RESERVE_L  =  0x00200000       /* Preserve the reserved bits */
    PERF_CONTROL3_RESERVE_H  =  0x0FCF0          /* Preserve the reserved bits */
    CONFIG_EVENT_L           =  0x0F0E2          /* All cores with level detection */
    CONFIG_EVENT_H           =  4                /* Increment count by number of event */
                                                 /* occured in clock cycle */
    EVENT_ENABLE             =  22               /* Enable the event */
PERF_COUNTER3            =      0x0C0010007      /* Performance event counter three */

COMPUTE_UNIT_STATUS     = 0x08000C580 /* Compute Unit Status Register */
  QUAD_CORE                 = 24      /*  QuadCore         four cores of a compute unit are enabled */
  DUAL_CORE                 = 16      /*  DualCore         two cores of a compute unit are enabled */
  TRIPLE_CORE               = 8       /*  TripleCore       three cores of a compute unit are enabled */
  CU_ENABLED                = 0       /*  Enabled          at least one core of a compute unit is enabled */

FUNC_3			=	3
MCA_NB_CFG		=	0x44		 /* MCA NB Configuration */
CPU_ERR_DIS             =       6                /* CPU error response disable */
PRODUCT_INFO_REG1	=	0x1FC			/* Product Information Register 1 */

# Local use flags, in upper most byte if ESI
FLAG_UNKNOWN_FAMILY     = 24          # Signals that the family# of the installed processor is not recognized
FLAG_STACK_REENTRY      = 25          # Signals that the environment has made a re-entry (2nd) call to set up the stack
FLAG_IS_PRIMARY         = 26          # Signals that this core is the primary within the comoute unit
FLAG_CORE_NOT_IDENTIFIED     = 27          # Signals that the cores/compute units of the installed processor is not recognized
FLAG_FORCE_32K_STACK     = 28         # Signals that to force 32KB stack size for BSP core
CR0_MASK    = ((1 << CR0_CD) | (1 << CR0_NW))
MSR_MASK    = ((1 << MTRR_DEF_TYPE_EN)+(1 << MTRR_DEF_TYPE_FIX_EN))

/****************************************************************************
 *
 *                      CPU MACROS - PUBLIC
 *
 ****************************************************************************/
.macro   _WRMSR
        .byte 0x0f, 0x30
.endm

.macro  _RDMSR
        .byte   0x0F, 0x32
.endm

.macro AMD_CPUID arg0
  .ifb \arg0
    mov   $0x1, %eax
    .byte 0x0F, 0x0A2                     /* Execute instruction */
    bswap %eax
    xchg  %ah, %al                        /* Ext model in al now */
    rol   $0x08, %eax                     /* Ext model in ah, model in al */
    and   $0x0FFCF, ax                    /* Keep 23:16, 7:6, 3:0 */
  .else
    mov   \arg0, %eax
    .byte 0x0F, 0x0A2
  .endif
.endm

.macro MAKE_EXT_PCI_ADDR  Seg, Bus, Dev, Func, Offset
	mov $(1 << 31 | (Seg) << 28 | (((Offset) & (0x0F00)) >> 8) << 24 | (Bus) << 16 | (Dev) << 11 | (Func) << 8) | ((Offset) & (0xFC)), %eax
.endm
/****************************************************************************
*
* AMD_ENABLE_STACK_FAMILY_HOOK Macro - Stackless
*
*   Set any family specific controls needed to enable the use of
*   cache as general storage before main memory is available.
*
* Inputs:
*       none
* Outputs:
*       none
 ****************************************************************************/
.macro  AMD_ENABLE_STACK_FAMILY_HOOK

    AMD_ENABLE_STACK_FAMILY_HOOK_F16

.endm

/****************************************************************************
*
* AMD_DISABLE_STACK_FAMILY_HOOK Macro - Stackless
*
*   Return any family specific controls to their 'standard'
*   settings for using cache with main memory.
*
* Inputs:
*       none
* Outputs:
*       none
 ****************************************************************************/
.macro  AMD_DISABLE_STACK_FAMILY_HOOK

    AMD_DISABLE_STACK_FAMILY_HOOK_F16

.endm

/****************************************************************************
*
* GET_NODE_ID_CORE_ID Macro - Stackless
*
*   Read family specific values to determine the node and core
*   numbers for the core executing this code.
*
* Inputs:
*     none
* Outputs:
*     SI[7:0] = Core# (0..N, relative to node)
*     SI[15:8]= Node# (0..N)
*     SI[23:16]= reserved
*     SI[24]=   flag: 1=Family Unrecognized
*     SI[25]=   flag: 1=Interface re-entry call
*     SI[26]=   flag: 1=Core is primary of compute unit
*     SI[31:27]= reserved, =0
****************************************************************************/
.macro  GET_NODE_ID_CORE_ID

    mov     $-1, %si
    GET_NODE_ID_CORE_ID_F16

    /*
     * Check for unrecognized Family
     */
    cmp     $-1, %si                      # Has family (node/core) already been discovered?
    jnz     node_core_exit\@              # Br if yes

   mov     $((1 << FLAG_UNKNOWN_FAMILY)+(1 << FLAG_IS_PRIMARY)), %esi # No, Set error code, Only let BSP continue

    mov     $APIC_BASE_ADDRESS, %ecx      # MSR:0000_001B
    _RDMSR
    bt      $APIC_BSC, %eax               # Is this the BSC?
    jc      node_core_exit\@              # Br if yes
    hlt                                 # Kill APs
node_core_exit\@:

.endm

/*
***************************************************************************
                      Family 16h MACROS
***************************************************************************/
/*--------------------------------------------------

AMD_ENABLE_STACK_FAMILY_HOOK_F16 Macro - Stackless

  Set any family specific controls needed to enable the use of
  cache as general storage before main memory is available.

Inputs:
      ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
Outputs:
      none
Destroyed:
      eax, ebx, ecx, edx

Family 16h requirements (BKDG #48751 section 2.3.3):
  * Paging must be disabled.
  * MSRC001_0015[INVD_WBINVD]=0
  * MSRC001_1020[DisSS]=1
  * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
  * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
  * MSRC001_1022[DisHwPf]=1
  * If MSRC001_102B[CombineCr0Cd] == 1 then MSRC001_102B[CombineCroCd] = 0
  * No INVD or WBINVD, no exceptions, page faults or interrupts
--------------------------------------------------

	*/

.macro  AMD_ENABLE_STACK_FAMILY_HOOK_F16

    AMD_CPUID   $CPUID_MODEL
    mov     %eax, %ebx                # Save revision info to EBX
    shr     $20, %eax                 # AL = cpu extended family
    cmp     $0x07, %al                     # Is this family 16h?
    jnz     fam16_enable_stack_hook_exit\@ # Br if no

    bt      $FLAG_STACK_REENTRY , %esi                 # Check if stack has already been set
    jc      fam16_skipClearingBit4
    mov     $HWCR, %ecx                   # MSR C001_0015
    _RDMSR
    btr     $INVD_WBINVD, %eax            # disable INVD -> WBINVD conversion
    _WRMSR

fam16_skipClearingBit4:
    mov     $IC_CFG, %ecx                  # MSR:C001_1021
    _RDMSR
    bts     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on Disable speculative IC-TLB reloads bit
    _WRMSR

#    mov     %ebx, %eax                   # Restore revision info to EAX
#    shr     $16, %eax
#    and     $0x0F, %al                   # AL = cpu extended model

    mov     $DC_CFG, %ecx                # MSR:C001_1022
    _RDMSR
    bts     $DC_DIS_SPEC_TLB_WALK, %eax    # Turn on Disable speculative DC-TLB reloads bit
    bts     $DIS_HW_PF, %eax              # Turn on Disable hardware prefetches bit
    _WRMSR				# Remove KM in PI 1.1.0.0

    mov     $0x0C00110A0, %ecx                  # MSR:C001_10A0
    _RDMSR
    bts     $L2_RINSER_DIS, %eax    #Do not search for the repair single bit errors in the background
    bts     $PREFETCHER_DIS, %eax
    bts     $CACHE_IC_ATTR_DIS, %eax
    _WRMSR

fam16_enable_stack_hook_exit\@:
.endm

/*
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F16 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15:8]= Node#; [7:0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 16h requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_1020[DisSS]=0
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_HW_PF]=0
;---------------------------------------------------
*/
.macro  AMD_DISABLE_STACK_FAMILY_HOOK_F16

    AMD_CPUID   $CPUID_MODEL
    mov     %eax, %ebx                    # Save revision info to EBX
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x07, %al                     # Is this family 16h?
    jnz     fam16_disable_stack_hook_exit\@ # Br if no

    mov     %ebx, %edi                    # Save revision info to EDI
    AMD_CPUID  $AMD_CPUID_APIC
    mov     %cl,  %al                     # AL = number of cores - 1
    shr     $APIC_ID_CORE_ID_SIZE, %cx    # CL = ApicIdCoreIdSize
    mov     $1,   %bx
    shl     %cl,  %bl                     # BL = theoretical number of cores on socket
    dec     %bx                           # BL = core number on socket mask
    mov     %bl,  %ah                     # AH = core number on socket mask
    mov     %edi, %ebx                    # Restore revision info to EBX
    mov     %ax,  %di                     # DI[15:8] = core number mask, DI[7:0] = number of cores - 1

    and     $0x0F00FF, %ebx
    mov     %ebx,  %eax
    shr     $8,    %eax
    or      %ax,   %bx                    # Save Extended Model, Model and Stepping to BX
	                                  # [11:8] = Extended Model, [7:4] = Model, [3:0] = Stepping (bx=0000000000010100, ok)

    mov     $APIC_BASE_ADDRESS, %ecx
    _RDMSR                                # dx=0 ax=fee00800
    mov     %bx,  %dx                     # Save Extended Model, Model and Stepping to DX
    shl     $16,  %edx                    #EDX[31:16] = Extended Model, Model and Stepping
    mov     %eax ,%ebx                    # EBX = LAPIC base
    xor     %ecx ,%ecx                    # Zero out CU flags
    bts     $AMD_CU_NEED_TO_WAIT,  %ecx  # Default to waiting
    bts     $AMD_CU_SEND_INVD_MSG, %ecx  # Default to signaling
    mov     %cr0, %eax
    bt      $CR0_PE, %ax                  # Are we in protected mode?
    # .if (!carry?)
    jc      1f
    bts     $AMD_CU_RESTORE_ES, %ecx  # Indicate ES restore is required
    mov     %es,   %cx    # Save ES segment register to CX
    xor     %ax,   %ax
    mov     %ax,   %es    # Set ES to big real mode selector for 4GB access
    # .endif

1:
    and     $0x0F000, %bx                  # EBX = LAPIC base, offset 0
    or      $APIC_ID_REG, %bl		#
    mov     %es:(%ebx), %eax               # EAX[31:24] = APIC ID
    shr     $APIC20_APICID, %eax          # AL = APIC ID
    mov     %al, %ah                      # AH = APIC ID
    mov     %di, %dx                      # DH = core mask
    and     %dh, %ah                      # AH = core number #  ax=111 dx=01000F03

    # .if (zero?)
    jnz      1f
    #  Core 0 of a socket
        btr     $AMD_CU_SEND_INVD_MSG, %ecx  # No need to signal after INVD
        #.if (dl != 0)
	cmp     $0,  %dl
        jz      2f
        # This socket has multiple cores
        and     $0xf000, %bx            # EBX = LAPIC base, offset 0
        or      $APIC_MSG_REG, %bx
        mov     $APIC_MSG,  %edi
	mov     %edi,  %es:(%ebx)      # Signal for non core 0s to complete CAR breakdown
        jmp     1f
        #.else
2:      btr     $AMD_CU_NEED_TO_WAIT, %ecx   # No need to wait on a single core CPU
	#.endif
    # .endif
1:

    bt      $AMD_CU_NEED_TO_WAIT, %ecx  #cx = c0000000
    #.if (carry?)
    jnc    1f
        #.if (ah == dl)
        cmp  %dl,  %ah
        jnz  2f
        # This is the highest numbered core on this socket -- wait on core 0
            not  %dh                       # Flip the mask to determine local core 0's APID ID
            and   %dh, %al                 # AL = target APIC ID  # ax=310
        jmp   3f
2:      #.else
        # All other cores (including core 0) wait on the next highest core.
        # In this way, cores will halt in a cascading fashion down to 0.
            inc    %al
        #.endif
3:
        shl     $APIC20_APICID, %eax
        and     $0x0F000, %bx
        or      $APIC_CMD_HI_REG, %bx
        mov     %eax, %es:(%ebx)          # Set target APIC ID

        # Use bits 23:16 as a timeout for unresponsive cores
        ror     $8,  %ecx
        mov     $0xFF, %ch
        stc

        #.while (carry?)
5:	jnc     4f
            and     $0xF000, %bx         #EBX = LAPIC base, offset 0
            or      $APIC_CMD_LO_REG, %bx # bx = 00000000FEE00300
            mov     $0x338, %eax
            mov     %eax, %es:(%ebx)     #Fire remove read IPI
            inc     %ch                  #Pre increment the timeout
            stc
            #.while (carry?)
7:          jnc     6f
                dec     %ch                   #Check the timeout
                jz      fam16_disable_stack_remote_read_exit\@
                mov     %es:(%ebx), %eax      # ax = 0000000000020338
                bt      $DELIVERY_STS_BIT, %eax
            jmp     7b
6:          #.endw
            stc
            #.while (carry?)
7:          jnc     6f
                mov     %es:(%ebx), %eax
                and     $REMOTE_READ_STS, %eax
                #.if (eax == REMOTE_DELIVERY_PEND)
                    cmp     $REMOTE_DELIVERY_PEND, %eax
                    jnz      8f
                    dec     %ch               # Check the timeout
                    jz      fam16_disable_stack_hook_exit\@ # Branch if there is an unreponsive core
                    stc
                jmp     9f
8:              #.else
                    clc
9:              #.endif
            jmp     7b
6:          #.endw
            #.if (eax == REMOTE_DELIVERY_DONE)
            cmp     $REMOTE_DELIVERY_DONE, %eax
            jnz     6f
                and     $0x0F000, %bx        #EBX = LAPIC base, offset 0
                or      $APIC_REMOTE_READ_REG, %bl
                mov     %es:(%ebx), %eax
                #.if (eax == APIC_MSG)
                cmp     $APIC_MSG, %eax     # ax=00000000FFC5BBB2
                jnz     8f
                    clc
                jmp     9f
                #.else
8:                  stc
9:              #.endif
            jmp     7f
6:          #.else
                dec     %ch
                jz      fam16_disable_stack_remote_read_exit\@
                stc
7:          #.endif
        jmp     5b
4:      #.endw

fam16_disable_stack_remote_read_exit\@:
        rol   $8, %ecx                 # Restore ECX

1:  #.endif

    bt     $AMD_CU_RESTORE_ES,  %ecx
    #.if (carry?)
    jnc    1f
        mov    %cx, %es
1:
    mov    %ecx,   %edi
    shr    $16, %edx
    mov    %dx,  %bx

    #Handshaking complete.  Continue tearing down CAR.

    mov     $IC_CFG, %ecx                # MSR:C001_1021
    _RDMSR
    btr     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative TLB reloads
    _WRMSR

    mov     $DC_CFG, %ecx                # MSR:C001_1022
    _RDMSR
    btr     $DC_DIS_SPEC_TLB_WALK, %eax    # Turn on speculative table-walks
    #.if (bx != 0)              # Is this rev A0?
    #cmp	$0, %bx
    #jz	0f
    btr     $DIS_HW_PF, %eax              # Turn on hardware prefetches
    #.endif                              # End workaround for erratum 498
    #0:
    _WRMSR

    mov     $0x0C00110A0, %ecx                 #MSR:C001_10A0
    _RDMSR
    btr     $PREFETCHER_DIS, %eax
    btr     $CACHE_IC_ATTR_DIS, %eax
    _WRMSR

    mov     $BU_CFG2, %ecx
    _RDMSR
    btr     $F16_CL_LINES_TO_L2_DIS, %eax
    _WRMSR

    mov     $HWCR, %ecx                    # MSR:C001_0015h
    _RDMSR
    btr     $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                # Clear the cache tag RAMs

    #Do Standard Family 16 work
    mov     $HWCR, %ecx                    # MSR:C001_0015h
    _RDMSR
    bts     $INVD_WBINVD, %eax            # Turn on Conversion of INVD to WBINVD
    _WRMSR
    #.endif                              # end

    bt      $AMD_CU_SEND_INVD_MSG, %edi
    #.if (carry?)
    jnc     1f
    AMD_CPUID   $AMD_CPUID_APIC
    shr     $APIC_ID_CORE_ID_SIZE, %cx  # CL = ApicIdCoreIdSize
    mov     $1, %di
    shl     %cl, %di           #DI = theoretical number of cores on socket
    dec     %di             # DI = core number on socket mask
    # Non core zero needs to signal to core 0 to proceed
    mov     $APIC_BASE_ADDRESS, %ecx
    _RDMSR
    mov     %eax, %ebx                # EBX = LAPIC base
    and     $0x0F000, %bx              # EBX = LAPIC base, offset 0
    or      $APIC_MSG_REG, %bx
    mov     $APIC_MSG, %eax
    mov     %eax, %es:(%ebx)           # Signal for core 0 to complete CAR breakdown

    #TODO: Non core zero needs to wait for core zero to do INVD
    #A handshake is required to ensure that all cores on a node invalidate in sync.
    and    $0x0F000, %bx
    or     $APIC_ID_REG, %bl
    mov    %es:(%ebx), %eax          # EAX[31:24] = APIC ID
    shr    $APIC20_APICID, %eax      # AL = APIC ID
    mov    %di, %dx                  # Use DL as core mask
    not    %dl
    and    %dl, %al                  # Remote read message from core zero
    shl    $APIC20_APICID, %eax

    and    $0x0F000, %bx
    or     $APIC_CMD_HI_REG, %bx
    mov    %eax, %es:(%ebx)           #
        # Use bits 23:16 as a timeout for unresponsive cores
        ror     $8,  %ecx
        mov     $0xFF, %ch
        stc

        #.while (carry?)
5:	jnc     4f
            and     $0xF000, %bx         #EBX = LAPIC base, offset 0
            or      $APIC_CMD_LO_REG, %bx # bx = 00000000FEE00300
            mov     $0x338, %eax
            mov     %eax, %es:(%ebx)     #Fire remove read IPI
            inc     %ch                  #Pre increment the timeout
            stc
            #.while (carry?)
7:          jnc     6f
                dec     %ch                   #Check the timeout
                jz      fam16_invd_done_remote_read_exit\@
                mov     %es:(%ebx), %eax      # ax = 0000000000020338
                bt      $DELIVERY_STS_BIT, %eax
            jmp     7b
6:          #.endw
            stc
            #.while (carry?)
7:          jnc     6f
                mov     %es:(%ebx), %eax
                and     $REMOTE_READ_STS, %eax
                #.if (eax == REMOTE_DELIVERY_PEND)
                    cmp     $REMOTE_DELIVERY_PEND, %eax
                    jnz      8f
                    dec     %ch               # Check the timeout
                    jz      fam16_invd_done_remote_read_exit\@ # Branch if there is an unreponsive core
                    stc
                jmp     9f
8:              #.else
                    clc
9:              #.endif
            jmp     7b
6:          #.endw
            #.if (eax == REMOTE_DELIVERY_DONE)
            cmp     $REMOTE_DELIVERY_DONE, %eax
            jnz     6f
                and     $0x0F000, %bx        #EBX = LAPIC base, offset 0
                or      $APIC_REMOTE_READ_REG, %bl
                mov     %es:(%ebx), %eax
                #.if (eax == APIC_MSG)
                cmp     $APIC_MSG, %eax     # ax=00000000FFC5BBB2
                jnz     8f
                    clc
                jmp     9f
                #.else
8:                  stc
9:              #.endif
            jmp     7f
6:          #.else
                dec     %ch
                jz      fam16_invd_done_remote_read_exit\@
                stc
7:          #.endif
        jmp     5b
4:      #.endw

     jmp  2f
fam16_invd_done_remote_read_exit\@:
1: #.else
     mov    $APIC_BASE_ADDRESS, %ecx
     _RDMSR
     mov    %eax, %ebx
     and    $0x0F000, %bx
     or     $APIC_MSG_REG, %bx
     mov    $APIC_INVD_ALL_DONE_MSG, %edi
     mov    %edi,  %es:(%ebx)
2: #.endif

fam16_disable_stack_hook_exit\@:
.endm

/*
*/
.macro  GET_NODE_ID_CORE_ID_F16


#define F16_L2Size	1024
#define F16_ShareCores  4
#define F16_AllocMem    0
#define F16_AllocExe    0
#define F16_SzAddrBus   40
#define F16_pad         0
    cmp     $-1, %si                      # Has node/core already been discovered?
    jnz     node_core_f16_exit\@          # Br if yes

    AMD_CPUID   $CPUID_MODEL
    shr     $12, %eax                     # AL = cpu extended family
    cmp     $07, %ah                     # Is this family 16h?
    jnz     node_core_f16_exit\@          # Br if no
    shr     $4,  %al                     # AL = cpu extended model
    shr     $16, %ebx                    # BH = LocalApicId
    mov     %al, %bl                     # BL = cpu extended model

    # LoadTableAddress(FAM16H_INFO_STRUCT)
    # movd    mm5, eax                    # load pointer to Family Info Struc

    xor     %esi, %esi                    # Assume BSC, clear local flags
    mov     $APIC_BASE_ADDRESS, %ecx      # MSR:0000_001B
    _RDMSR
    bt      $APIC_BSC, %eax               # Is this the BSC?
    jnc      node_core_f16_AP\@            # Br if no

    # This is the BSP.
    # Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
    mov     $0x8000C06C, %eax              # PCI address for D18F0x6C Link Initialization Control Register
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    in      %dx, %eax
    btr     $0, %eax                      # Set LinkInitializationControl[RouteTblDis] = 0
    out     %eax, %dx
    jmp     6f         #node_core_f16_shared        #

node_core_f16_AP\@:
    mov     %bl, %al                        # AL = cpu extended model
    shr     $8,  %bx                        # BL = CPUID Fn0000_0001_EBX[LocalApicId]
    #.if (al == 0)
    cmp    $0,  %al                        # Is This KB?
    jnz    5f
    #.else
4:  mov     %bx, %si
    jmp     6f
5:  #node_core_f16_AP_not_KB
         bts $FLAG_CORE_NOT_IDENTIFIED, %esi
    #.endif
    #.endif
      #
      #   determine if this core shares MTRRs
      #
6:	#node_core_f16_shared
    mov     $COMPUTE_UNIT_STATUS, %eax              # Compute Unit Status
    mov     %si, %bx
    shl     $3, %bh                       # Move node# to PCI Dev# field
    add     %bh, %ah                      # Adjust for node number
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    in      %dx, %eax                     # [3:0]=Enabled# [19:16]=DualCore

                                        # BL is MyCore#
    mov     $0x04, %cx                     # Use CH as 'first of pair' core#
    #.while (cl > 0)
    jmp  0f
    8:
        bt      $CU_ENABLED, %eax                  # Is pair enabled?
        #.if (carry?)                    #
        jnc     1f
            mov     $0x01, %bh             #   flag core as primary
            #.break .if (ch == bl)   # Does 1st match MyCore#?
            cmp     %bl, %ch
            je      9f
            #inc     %ch
            xor     %bh, %bh          #     flag core as NOT primary
            #
            bt      $DUAL_CORE, %eax
            #.if (carry?)
	    jnc  5f
                 add   $1, %eax
            #.endif
            5:
	    bt      $TRIPLE_CORE, %eax
	    jnc  5f
            #.if (carry?)
                 add   $2, %eax
            #.endif
            5:
	    bt      $QUAD_CORE, %eax
            jnc  5f
            #.if (carry?)
                 add   $3, %eax
            #.endif
            5:
            #.break .if (ch >= bl) #TODO:
            cmp     %bl, %ch
            jae      9f
            inc     %ch
        #.endif
        1:
        shr     $1, %eax
        dec     %cl
    0:
    cmp $0x0, %cl
    ja	8b

    #.endw
    9:

    #.if (cl == 0)
    or	%cl, %cl
    jne 1f
        #Error - core# didn't match Compute Unit Status content
        bts     $FLAG_CORE_NOT_IDENTIFIED, %esi
        bts     $FLAG_IS_PRIMARY, %esi    #   Set Is_Primary for unknowns
    #.endif
    1:
    #.if (bh != 0)                       # Check state of primary for the matched core
    or %bh, %bh
    je 2f
        bts     $FLAG_IS_PRIMARY, %esi    #   Set shared flag into return value
    #.endif
    2:

node_core_f16_exit\@:

.endm

/*****************************************************************************
* AMD_ENABLE_STACK:  Setup a stack
*
*   In:
*       No inputs
*
*   Out:
*       SS:ESP - Our new private stack location
*
*       EAX = AGESA_STATUS
*
*       ECX = Stack size in bytes
*
*   Requirements:
*       * This routine presently is limited to a max of 64 processor cores
*   Destroyed:
*       EBX, EDX, EDI, ESI, EBP, DS, ES
*
*   Description:
* Fixed MTRR address allocation to cores:
* The BSP gets 64K of stack, Core0 of each node gets 16K of stack, all other cores get 4K.
* There is a max of 1 BSP, 7 core0s and 56 other cores.
* Although each core has it's own cache storage, they share the address space. Each core must
* be assigned a private and unique address space for its stack. To support legacy systems,
* the stack needs to be within the legacy address space (1st 1Meg). Room must also be reserved
* for the other legacy elements (Interrupt vectors, BIOS ROM, video buffer, etc.)
*
* 80000h                                        40000h                                      00000h
*     +----------+----------+----------+----------+----------+----------+----------+----------+
* 64K |          |          |          |          |          |          |          |          |  64K  ea
*  ea +----------+----------+----------+----------+----------+----------+----------+----------+
*     |                             MTRR 0000_0250 MTRRfix64K_00000                           |
*     +----------+----------+----------+----------+----------+----------+----------+----------+
*     |  7 ,  6  |  5 ,  4  |  3 ,  2  |  1 ,  0  |     0    |          |          |          | <-node
*     |7..1,7..1 |7..1,7..1 |7..1,7..1 |7..1,7..1 |     0    |          |          |          | <-core
*     +----------+----------+----------+----------+----------+----------+----------+----------+
*
* C0000h                       B0000h                      A0000h                      90000h                      80000h
*     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
*16K  |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |
* ea  +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
*     |              MTRR 0259 MTRRfix16K_A0000               |             MTRR 0258 MTRRfix16K_80000                |
*     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
*     | > Dis|play B|uffer |   <  |      |      |      |      |   7  |  6   |  5   |  4   |  3   |  2   |  1   |      | <-node
*     | >   T| e  m |p o r |a r y |  B u |f f e |r   A |r e a<|   0  |  0   |  0   |  0   |  0   |  0   |  0   |      | <-core
*     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
*
* E0000h                                            D0000h                                         C0000h
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
* 4K  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  4K  ea
*  ea +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*     |  026B MTRRfix4K_D8000 | 026A MTRRfix4K_D0000  | 0269 MTRRfix4K_C8000  | 0268 MTRRfix4K_C0000  |
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | >| V| I| D| E| O|  |B |I |O |S |  |A |r |e |a<|
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*
* 100000h                                           F0000h                                          E0000h
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  4K  ea
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*     |  026F MTRRfix4K_F8000 | 026E MTRRfix4K_F0000  | 026D MTRRfix4K_E8000  | 026C MTRRfix4K_E0000  |
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*     | >|MA|IN| B|IO|S |RA|NG|E |  |  |  |  |  |  |< | >|EX|TE|ND|ED| B|IO|S |ZO|NE|  |  |  |  |  |< |
*     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*****************************************************************************/
.macro  AMD_ENABLE_STACK

# These are local labels. Declared so linker doesn't cause 'redefined label' errors

#   Note that SS:ESP will be default stack.  Note that this stack
#   routine will not be used after memory has been initialized.  Because
#   of its limited lifetime, it will not conflict with typical PCI devices.

    # get node id and core id of current executing core
    GET_NODE_ID_CORE_ID                 # Sets ESI[23:16]=Shared core## SI[15,8]= Node## SI[7,0]= core# (relative to node)
    # Note: ESI[31:24] are used for flags:  Unrecognized Family,  Is_Primary core,  Stack already established

    # determine if stack is already enabled. We are using the DefType MSR for this determination.
    # It is =0 after reset; CAR setup sets it to enable the MTRRs
    mov     %cr0, %eax
    test    $CR0_MASK, %eax              # Is cache disabled? (CD & NW bits)
    jnz     SetupStack\@                  # Jump if yes
    mov     $AMD_MTRR_DEFTYPE, %ecx       # MSR:0000_02FF
    _RDMSR
    test    $MSR_MASK, %eax                     # Are the default types enabled? (MTRR_DEF_TYPE_EN + MTRR_DEF_TYPE_FIX_EN)
    jz      SetupStack\@                  # Jump if no
    or      $FLAG_STACK_REENTRY, %esi             # Bit25, indicate stack has already been initialized

SetupStack\@:
    # Set node to map the first 16MB to node 0# 0000_0000 to 00FF_FFFF as DRAM
    mov     %esi, %ebx                    # Get my Node/Core info
    xor     %bl, %bl
    shl     $3, %bh                       # Isolate my node#, match alignment for PCI Dev#
    mov     $0x8000C144, %eax              # D18F1x44:DRAM Base/Limit# N is Base, N+4 is Limit
    add     %bh, %ah
    mov     %eax, %ebx                    # Save PCI address for Base/Limit pair

    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    xor     %eax, %eax                    # Least Significant bit is AD24 so 0 sets mask of 00FF_FFFF (16MB)
    out     %eax, %dx                     # DRAM Limit = node0, no interleave

    mov     %ebx, %eax
    sub     $4, %eax                      # Now point to the Base register
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    mov     $0x00000003, %eax              # Set the read and write enable bits
    out     %eax, %dx                     # DRAM Base = 0x0000, R/W

    AMD_ENABLE_STACK_FAMILY_HOOK

    # Init CPU MSRs for our init routines
    mov     $MTRR_SYS_CFG, %ecx           # SYS_CFG
    _RDMSR
    bts     $MTRR_FIX_DRAM_MOD_EN, %eax   # Turn on modification enable bit
    _WRMSR

    mov     %esi, %eax
    bt      $FLAG_STACK_REENTRY, %eax     # Is this a 2nd entry?
    #.if (!carry?)                       #   On a re-entry, do not clear MTRRs or reset TOM; just reset the stack SS:ESP
    jc 0f
        bt      $FLAG_IS_PRIMARY, %eax    #   Is this core the primary in a compute unit?
        #.if (carry?)                    #     Families using shared groups do not need to clear the MTRRs since that is done at power-on reset
            #  Note: Relying on MSRs to be cleared to 0's at reset for families w/shared cores
            # Clear all variable and Fixed MTRRs for non-shared cores
        jnc 0f
        mov     $AMD_MTRR_VARIABLE_BASE0, %ecx
        xor     %eax, %eax
        xor     %edx, %edx
        #.while (cl != 10h)                  # Variable MTRRphysBase[n] and MTRRphysMask[n]
        jmp	1f
        2:
            _WRMSR
            inc     %cl
        #.endw
        1:
        cmp	$0x10, %cl
        jne	2b
        mov     $AMD_MTRR_FIX64k_00000, %cx   # MSR:0000_0250
        _WRMSR
        mov     $AMD_MTRR_FIX16k_80000, %cx   # MSR:0000_0258
        _WRMSR
        mov     $AMD_MTRR_FIX16k_A0000, %cx   # MSR:0000_0259
        _WRMSR
        mov     $AMD_MTRR_FIX4k_C0000, %cx    # Fixed 4Ks: MTRRfix4K_C0000 to MTRRfix4K_F8000
        #.while (cl != 70h)
        jmp 3f
        4:
            _WRMSR
            inc     %cl
        #.endw
        3:
        cmp $0x70, %cl
        jne  4b
        # Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
        #  - not strictly needed since the FixedMTRRs take presedence.
        mov     $(16 * 1024 * 1024), %eax
        mov     $TOP_MEM, %ecx            # MSR:C001_001A
        _WRMSR
        #.endif                          #   End Is_Primary
    #.endif                              # End Stack_ReEntry
    0:
    # Clear IORRs (C001_0016-19) and TOM2(C001_001D) for all cores
    xor     %eax, %eax
    xor     %edx, %edx
    mov     $IORR_BASE, %ecx              # MSR:C001_0016 - 0019
    #.while (cl != 1Ah)
    jmp  1f
    2:
        _WRMSR
        inc     %cl
    #.endw
    1:
    cmp $0x1A, %cl
    jne  2b
    mov     $TOP_MEM2, %ecx               # MSR:C001_001D
    _WRMSR

    # setup MTRRs for stacks
    #   A speculative read can be generated by a speculative fetch mis-aligned in a code zone
    #    or due to a data zone being interpreted as code. When a speculative read occurs outside a
    #    controlled region (intentionally used by software), it could cause an unwanted cache eviction.
    #   To prevent speculative reads from causing an eviction, the unused cache ranges are set
    #    to UC type. Only the actively used regions (stack, heap) are reflected in the MTRRs.
    #    Note: some core stack regions will share an MTRR since the control granularity is much
    #    larger than the allocated stack zone. The allocation algorithm must account for this 'extra'
    #    space covered by the MTRR when parseling out cache space for the various uses. In some cases
    #    this could reduce the amount of EXE cache available to a core. see cpuCacheInit.c
    #
    # Outcome of this block is that:   (Note the MTRR map at the top of the file)
    #   ebp - start address of stack block
    #   ebx - [31:16] - MTRR MSR address
    #       - [15:8]  - slot# in MTRR register
    #       - [7:0]   - block size in #4K blocks
    # review: ESI[31:24]=Flags; SI[15,8]= Node#; SI[7,0]= core# (relative to node)
    #

    mov     %si, %ax                      # Load node, core
    #.if (al == 0)                       # Is a core 0?
    or %al, %al
    jne 1f
        #.if (ah == 0)                   # Is Node 0? (BSP)
        or %ah, %ah
        jne 2f
            # Is BSP, assign a 64K stack
            mov     $((AMD_MTRR_FIX64k_00000 << 16) + (3 << 8) + (BSP_STACK_SIZE  >> 12)), %ebx
            mov     $BSP_STACK_BASE_ADDR, %ebp
            jmp     0f
        #.else   # node 1 to 7, core0
        2:
            # Is a Core0 of secondary node, assign 16K stacks
            mov     $AMD_MTRR_FIX16k_80000, %bx
            shl     $16, %ebx             #
            mov     %ah, %bh              # Node# is used as slot#
            mov     $(CORE0_STACK_SIZE >> 12), %bl
            mov     %ah, %al              # Base = (Node# * Size)#
            mul     %bl                  #
            movzx   %ax, %eax             #
            shl     $12, %eax             # Expand back to full byte count (* 4K)
            add     $CORE0_STACK_BASE_ADDR, %eax
            mov     %eax, %ebp
        #.endif
        jmp 0f
    #.else    #core 1 thru core 7
    1:
        # Is core 1-7 of any node, assign 4K stacks
        mov     $8, %al                   # CoreIndex = ( (Node# * 8) ...
        mul     %ah                      #
        mov     %si, %bx                  #
        add     %bl, %al                  #         ...  + Core#)#

        mov     $AMD_MTRR_FIX64k_00000, %bx
        shl     $16, %ebx                 #
        mov     %al, %bh                  # Slot# = (CoreIndex / 16) + 4#
        shr     $4, %bh                   #
        add     $4, %bh                   #
        mov     $(CORE1_STACK_SIZE >> 12), %bl

        mul     %bl                      # Base = ( (CoreIndex * Size) ...
        movzx   %ax, %eax                 #
        shl     $12, %eax                 # Expand back to full byte count (* 4K)
        add     $CORE1_STACK_BASE_ADDR, %eax #     ...   + Base_Addr)#
        mov     %eax, %ebp
    #.endif
    0:

    # Now set the MTRR. Add this to already existing settings (don't clear any MTRR)
    mov     $WB_DRAM_TYPE, %edi           # Load Cache type in 1st slot
    mov     %bh, %cl                      # ShiftCount =  ((slot#   ...
    and     $0x03, %cl                     #   ...  % 4)             ...
    shl     $0x03, %cl                       #   ...  * 8)#
    shl     %cl, %edi                     # Cache type is now in correct position
    ror     $16, %ebx                     # Get the MTRR address
    movzx   %bx, %ecx                     #
    rol     $16, %ebx                     # Put slot# & size back in BX
    _RDMSR                              # Read-modify-write the MSR
    #.if (bh < 4)                        # Is value in lower or upper half of MSR?
    cmp $4, %bh
    jae 1f
        or      %edi, %eax                #
        jmp     0f
    #.else
    1:                               #
        or      %edi, %edx                #
    #.endif                              #
    0:
    _WRMSR                              #

    # Enable MTRR defaults as UC type
    mov     $AMD_MTRR_DEFTYPE, %ecx       # MSR:0000_02FF
    _RDMSR                              # Read-modify-write the MSR
    bts     $MTRR_DEF_TYPE_EN, %eax       # MtrrDefTypeEn
    bts     $MTRR_DEF_TYPE_FIX_EN, %eax   # MtrrDefTypeFixEn
    _WRMSR

    # Close the modification window on the Fixed MTRRs
    mov     $MTRR_SYS_CFG, %ecx           # MSR:0C001_0010
    _RDMSR
    bts     $MTRR_FIX_DRAM_EN, %eax       # MtrrFixDramEn
    bts     $MTRR_VAR_DRAM_EN, %eax       # variable MTRR enable bit
    btr     $MTRR_FIX_DRAM_MOD_EN, %eax   # Turn off modification enable bit
    _WRMSR

    # Enable caching in CR0
    mov     %cr0, %eax                    # Enable WT/WB cache
    btr     $CR0_PG, %eax                     # Make sure paging is disabled
    btr     $CR0_CD, %eax                     # Clear CR0 NW and CD
    btr     $CR0_NW, %eax
    mov     %eax, %cr0

    # Use the Stack Base & size to calculate SS and ESP values
    # review:
    #       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    #       ebp - start address of stack block
    #       ebx - [31:16] - MTRR MSR address
    #           - [15:8]  - slot# in MTRR register
    #           - [7:0]   - block size in #4K blocks
    #
    mov     %ebp, %esp                    # Initialize the stack pointer
    mov     %esp, %edi                    # Copy the stack start to edi
    movzx   %bl, %bx
    movzx   %bx, %ebx                     # Clear upper ebx, don't need MSR addr anymore
    shl     $12, %ebx                     # Make size full byte count (* 4K)
    add     %ebx, %esp                    # Set the Stack Pointer as full linear address
    sub     $4, %esp
    #
    # review:
    #       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    #       edi - 32b start address of stack block
    #       ebx - size of stack block
    #       esp - 32b linear stack pointer
    #

    # Determine mode for SS base;
    mov     %cr0, %ecx                    # Check for 32-bit protect mode
    bt      $CR0_PE, %ecx                 #
    #.if (!carry?)                       # PE=0 means real mode
    jc      Protected32Mode\@
    mov     %cs, %cx                      # PE=1
    cmp     $0x0D000, %cx                  # Check for CS
    jb      Protected32Mode\@             # If CS < D000, it is a selector instead of a segment
            # alter SS:ESP for 16b Real Mode:
Real16bMode\@:
    mov     %edi, %eax
    shr     $4, %eax                      # Create a Real Mode segment for ss, ds, es
    mov     %ax, %ss
    mov     %ax, %ds
    mov     %ax, %es
    shl     $4, %eax
    sub     %eax, %edi                    # Adjust the clearing pointer for Seg:Offset mode
    mov     %ebx, %esp                    # Make SP an offset from SS
    sub     $4, %esp              #
    #    .endif                          # endif
    # #else
    #   Default is to use Protected 32b Mode
    #.endif
    ;
Protected32Mode\@:
    #
    # Clear The Stack
    #   Now that we have set the location and the MTRRs, initialize the cache by
    #   reading then writing to zero all of the stack area.
    # review:
    #       ss  - Stack base
    #       esp - stack pointer
    #       ebx - size of stack block
    #       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    #       edi -  address of start of stack block
    #

ClearTheStack\@:                          # Stack base is in SS, stack pointer is in ESP
    shr     $2, %ebx                      # ebx = stack block size in dwords
    mov     %bx, %cx                      #
    # Check our flags - Don't clear an existing stack
    #.if ( !(esi & 0FF000000h))          # Check our flags
    test    $(1 << FLAG_STACK_REENTRY), %esi
    jne 1f
        cld
        mov     %edi, %esi
        rep     lodsl (%esi)    # Pre-load the range
        xor     %eax, %eax
        mov     %bx, %cx
        mov     %edi, %esi                # Preserve base for push on stack
        rep     stosl (%edi)    # Clear the range
        movl     $0x0ABCDDCBA, (%esp) # Put marker in top stack dword
        shl     $2, %ebx                  # Put stack size and base
        push    %ebx                     #  in top of stack
        push    %esi

        mov     %ebx, %ecx                # Return size of stack in bytes
        xor     %eax, %eax                # eax = 0 : no error return code
        jmp     0f
    #.else
    1:
        movzx   %cx, %ecx
        shl     $2, %ecx                  # Return size of stack in bytes
        mov     %esi, %eax
        shr     $24, %eax                 # Keep the flags as part of the error report
        or      $0x40000000, %eax          # eax = AGESA_WARNING (Stack has already been set up)
    #.endif
    0:
.endm

/*****************************************************************************
* AMD_DISABLE_STACK:  Destroy the stack inside the cache. This routine
*                     should only be executed on the BSP
*
*   In:
*       none
*
*   Out:
*       none
*
*   Preserved:
*       ESP
*   Destroyed:
*       EAX, EBX, ECX, EDX, EDI, ESI
*****************************************************************************/
.macro  AMD_DISABLE_STACK

    # get node/core/flags of current executing core
    GET_NODE_ID_CORE_ID                 # Sets ESI[15,8]= Node#; ESI[7,0]= core# (relative to node)

    # Turn on modification enable bit
    mov     $MTRR_SYS_CFG, %ecx           # MSR:C001_0010
    _RDMSR
    bts     $MTRR_FIX_DRAM_MOD_EN, %eax   # Enable modifications
    _WRMSR

    # Set lower 640K MTRRs for Write-Back memory caching
    mov     $AMD_MTRR_FIX64k_00000, %ecx
    mov     $0x1E1E1E1E, %eax
    mov     %eax, %edx
    _WRMSR                              # 0 - 512K = WB Mem
    mov     $AMD_MTRR_FIX16k_80000, %ecx
    _WRMSR                              # 512K - 640K = WB Mem

    # Turn off modification enable bit
    mov     $MTRR_SYS_CFG, %ecx           # MSR:C001_0010
    _RDMSR
    btr     $MTRR_FIX_DRAM_MOD_EN, %eax   # Disable modification
    _WRMSR

    AMD_DISABLE_STACK_FAMILY_HOOK       # Re-Enable 'normal' cache operations

.endm
