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

AMD_MTRR_VARIABLE_BASE0  =      0x0200
AMD_MTRR_VARIABLE_BASE6  =      0x020C
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

LS_CFG                   =      0x0C0011020      /* Load-Store Configuration                                                                                                                             */
    DIS_SS               =     28                /* Family 10h,12h,15h:Disable Streng Store functionality */
    DIS_STREAM_ST        =     28                /* Family 14h:DisStreamSt - Disable Streaming Store functionality */

IC_CFG                   =      0x0C0011021      /* Instruction Cache Config Register  */
    IC_DIS_SPEC_TLB_RLD  =      9                /*   Disable speculative TLB reloads  */
    DIS_IND              =      14               /*   Family 10-14h:Disable Indirect Branch Predictor */
    DIS_I_CACHE          =      14               /*   Family 15h:DisICache - Disable Indirect Branch Predictor */

DC_CFG                   =      0x0C0011022      /* Data Cache Configuration                                                                                                                                   */
    DC_DIS_SPEC_TLB_RLD      =  4                /*   Disable speculative TLB reloads */
    DIS_CLR_WBTOL2_SMC_HIT   =  8                /*   self modifying code check buffer bit */
    DIS_HW_PF                =  13               /*   Hardware prefetches bit                                                                                                                                    */

DE_CFG                   =      0x0C0011029      /* Decode Configuration */
    CL_FLUSH_SERIALIZE   =      23               /*   Family 12h,15h: CL Flush Serialization */

BU_CFG2                  =      0x0C001102A      /* Family 10h: Bus Unit Configuration 2 */
CU_CFG2                  =      0x0C001102A      /* Family 15h: Combined Unit Configuration 2 */
    F10_CL_LINES_TO_NB_DIS  =   15               /*   ClLinesToNbDis - allows WP code to be cached in L2 */
    IC_DIS_SPEC_TLB_WR      =   35               /*   IcDisSpecTlbWr - ITLB speculative writes */

CU_CFG3                  =      0x0C001102B      /* Combined Unit Configuration 3 */
    COMBINE_CR0_CD       =      49               /*   Combine CR0.CD for both cores of a compute unit */


CR0_PE                  = 0           # Protection Enable
CR0_NW                  = 29          # Not Write-through
CR0_CD                  = 30          # Cache Disable
CR0_PG                  = 31          # Paging Enable

/* CPUID Functions */

CPUID_MODEL              =      1
AMD_CPUID_FMF            =      0x80000001       /* Family Model Features information */
AMD_CPUID_APIC           =      0x80000008       /* Long Mode and APIC info., core count */

NB_CFG                   =      0x0C001001F      /* Northbridge Configuration Register */
    INIT_APIC_ID_CPU_ID_LO    = 54               /*  InitApicIdCpuIdLo - is core# in high or low half of APIC ID? */

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

# Local use flags, in upper most byte if ESI
FLAG_UNKNOWN_FAMILY     = 24          # Signals that the family# of the installed processor is not recognized
FLAG_STACK_REENTRY      = 25          # Signals that the environment has made a re-entry (2nd) call to set up the stack
FLAG_IS_PRIMARY         = 26          # Signals that this core is the primary within the comoute unit

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

    AMD_ENABLE_STACK_FAMILY_HOOK_F10
    AMD_ENABLE_STACK_FAMILY_HOOK_F12
    AMD_ENABLE_STACK_FAMILY_HOOK_F14
    AMD_ENABLE_STACK_FAMILY_HOOK_F15
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

    AMD_DISABLE_STACK_FAMILY_HOOK_F10
    AMD_DISABLE_STACK_FAMILY_HOOK_F12
    AMD_DISABLE_STACK_FAMILY_HOOK_F14
    AMD_DISABLE_STACK_FAMILY_HOOK_F15

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
    GET_NODE_ID_CORE_ID_F10
    GET_NODE_ID_CORE_ID_F12
    GET_NODE_ID_CORE_ID_F14
    GET_NODE_ID_CORE_ID_F15
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

/****************************************************************************
##                      Family 10h MACROS
##***************************************************************************
#---------------------------------------------------
#
# AMD_ENABLE_STACK_FAMILY_HOOK_F10 Macro - Stackless
#
#   Set any family specific controls needed to enable the use of
#   cache as general storage before main memory is available.
#
# Inputs:
#       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
# Outputs:
#       none
#
# Family 10h requirements (BKDG section 2.3.3):
#   * Paging disabled
#   * MSRC001_0015[INVDWBINVD]=0
#   * MSRC001_1021[DIS_IND]=1
#   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
#   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
#   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=1
#   * MSRC001_1022[DIS_HW_PF]=1
#   * MSRC001_102A[IcDisSpecTlbWr]=1
#   * MSRC001_102A[ClLinesToNbDis]=1
#   * No INVD or WBINVD, no exceptions, page faults or interrupts
****************************************************************************/
.macro AMD_ENABLE_STACK_FAMILY_HOOK_F10

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x01, %al                      # Is this family 10h?
    jnz     fam10_enable_stack_hook_exit\@ # Br if no

    mov     $DC_CFG, %ecx                 # MSR:C001_1022
    _RDMSR
    bts     $DC_DIS_SPEC_TLB_RLD, %eax     # Turn on Disable speculative DTLB reloads bit
    bts     $DIS_CLR_WBTOL2_SMC_HIT, %eax  # Turn on Disable the self modifying code check buffer bit
    bts     $DIS_HW_PF, %eax               # Turn on Disable hardware prefetches bit
    _WRMSR

    dec     %cx                          # MSR:C001_1021
    _RDMSR
    bts     $IC_DIS_SPEC_TLB_RLD, %eax     # Turn on Disable speculative TLB reloads bit
    bts     $DIS_IND, %eax                 # Turn on Disable indirect branch predictor
    _WRMSR

    mov     $BU_CFG2, %ecx                # MSR C001_102A
    _RDMSR
    bts     $F10_CL_LINES_TO_NB_DIS, %eax   # Allow BIOS ROM to be cached in the IC
    bts     $(IC_DIS_SPEC_TLB_WR-32), %edx  #Disable speculative writes to the ITLB
    _WRMSR

    mov     $HWCR, %ecx                    # MSR C001_0015
    _RDMSR
    bt      $FLAG_STACK_REENTRY, %esi                 # Check if stack has already been set
    jc      fam10_skipClearingBit4
    btr     $INVD_WBINVD, %eax             # disable INVD -> WBINVD conversion
    _WRMSR

fam10_skipClearingBit4:
    mov %esi, %eax                         # load core#
    or %al, %al                       # If (BSP)
    jne  fam10_enable_stack_hook_exit\@
    mov     $PERF_COUNTER3, %ecx       #   Select performance counter three
                                        #   to count number of CAR evictions
    xor     %eax, %eax                #   Initialize the lower part of the counter to zero
    xor     %edx, %edx                #   Initializa the upper part of the counter to zero
    _WRMSR                          #   Save it
    mov     $PERF_CONTROL3, %ecx      #   Select the event control three
    _RDMSR                          #   Get the current setting
    and     $PERF_CONTROL3_RESERVE_L, %eax   # Preserve the reserved bits
    or      $CONFIG_EVENT_L, %eax      #   Set the lower part of event register to
                                        #   select CAR Corruption occurred by any cores
    and     $PERF_CONTROL3_RESERVE_H, %dx   # Preserve the reserved bits
    or      $CONFIG_EVENT_H, %dx       #   Set the upper part of event register
    _WRMSR                          #   Save it
    bts     $EVENT_ENABLE, %eax        #   Enable it
    _WRMSR                          #   Save it

fam10_enable_stack_hook_exit\@:
.endm

/****************************************************************************
*
* AMD_DISABLE_STACK_FAMILY_HOOK_F10 Macro - Stackless
*
*   Return any family specific controls to their 'standard'
*   settings for using cache with main memory.
*
* Inputs:
*       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
* Outputs:
*       none
*
* Family 10h requirements:
*   * INVD or WBINVD
*   * MSRC001_0015[INVD_WBINVD]=1
*   * MSRC001_1021[DIS_IND]=0
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=0
*   * MSRC001_1022[DIS_HW_PF]=0
*   * MSRC001_102A[IcDisSpecTlbWr]=0
*   * MSRC001_102A[ClLinesToNbDis]=0
*****************************************************************************/

.macro  AMD_DISABLE_STACK_FAMILY_HOOK_F10

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x01, %al                     # Is this family 10h?
    jnz     fam10_disable_stack_hook_exit # Br if no

    mov     $DC_CFG, %ecx                 # MSR:C001_1022
    _RDMSR
    btr     $DC_DIS_SPEC_TLB_RLD, %eax    # Enable speculative TLB reloads
    btr     $DIS_CLR_WBTOL2_SMC_HIT, %eax # Allow self modifying code check buffer
    btr     $DIS_HW_PF, %eax              # Allow hardware prefetches
    _WRMSR

    dec     %cx                          # MSR:C001_1021
    _RDMSR
    btr     $DIS_IND, %eax                # Turn on indirect branch predictor
    btr     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative TLB reloads
    _WRMSR

    mov     $BU_CFG2, %ecx                # MSR:C001_102A
    _RDMSR
    btr     $F10_CL_LINES_TO_NB_DIS, %eax  # Return L3 to normal mode
    btr     $(IC_DIS_SPEC_TLB_WR-32), %edx #Re-enable speculative writes to the ITLB
    _WRMSR

    #--------------------------------------------------------------------------
    # Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    mov     $HWCR, %ecx                    # MSR:0000_0015
    _RDMSR
    mov     %ax, %bx                      # Save INVD -> WBINVD bit
    btr    $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion for the invd instruction.
    _WRMSR
    invd                                # Clear the cache tag RAMs
    mov    %bx, %ax                      # Restore INVD -> WBINVD bit
    _WRMSR

    #--------------------------------------------------------------------------
    # End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    mov     $PERF_CONTROL3, %ecx          # Select the event control three
    _RDMSR                              # Retrieve the current value
    btc     $EVENT_ENABLE, %eax           # Is event enable, complement it as well
    jnc     fam10_disable_stack_hook_exit # No
    cmp     $CONFIG_EVENT_L, %ax          # Is the lower part of event set to capture the CAR Corruption
    jne     fam10_disable_stack_hook_exit # No
    cmp     $CONFIG_EVENT_H, %dl        # Is the upper part of event set to capture the CAR Corruption
    jne     fam10_disable_stack_hook_exit # No
    _WRMSR                              # Disable the event

fam10_disable_stack_hook_exit:
.endm

/****************************************************************************
*
* GET_NODE_ID_CORE_ID_F10 Macro - Stackless
*
*   Read family specific values to determine the node and core
*   numbers for the core executing this code.
*
* Inputs:
*     none
* Outputs:
*     SI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
*****************************************************************************/
.macro  GET_NODE_ID_CORE_ID_F10


    cmp     $-1, %si                      # Has node/core already been discovered?
    jnz     node_core_f10_exit\@          # Br if yes

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x01, %al                     # Is this family 10h?
    jnz     node_core_f10_exit\@          # Br if no

    xor     %esi, %esi                    # Assume BSC, clear flags
    mov     $APIC_BASE_ADDRESS, %ecx      # MSR:0000_001B
    _RDMSR
    bt      $APIC_BSC, %eax             # Is this the BSC?
    jnc      node_core_f10_AP\@            # Br if no

        # This is the BSP.
    # Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
    mov     $0x8000C06C, %eax             # PCI address for D18F0x6C Link Initialization Control Register
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    in      %dx, %eax
    btr     $0, %eax                      # Set LinkInitializationControl[RouteTblDis] = 0
    out     %eax, %dx
    jmp     1f          #

node_core_f10_AP\@:
    #
    # This is an AP. Routing tables have been enabled by the HT Init process.
    # Also, the MailBox register was set by the BSP during early init
    #   The Mailbox register content is formatted as follows:
    #         UINT32 Node:4#          // The node id of Core's node.
    #         UINT32 Socket:4#        // The socket of this Core's node.
    #         UINT32 Module:2#        // The internal module number for Core's node.
    #         UINT32 ModuleType:2#    // Single Module = 0, Multi-module = 1.
    #         UINT32 :20#             // Reserved
    #
    mov     $0x0C0000408, %ecx             # Read the family 10h mailbox
    _RDMSR                              #        MC4_MISC1[63:32]
    mov     %dx, %si                      # SI = raw mailbox contents (will extract node# from this)
    shr     $24, %ebx                     # BL = CPUID Fn0000_0001_EBX[LocalApicId]
    mov     %bx, %di                      # DI = Initial APIC ID (will extract core# from this)

    AMD_CPUID   $AMD_CPUID_APIC          #
    shr     $4, %ch                       # CH = ApicIdSize, #bits in APIC ID that show core#
    inc     %cl                          # CL = Number of enabled cores in the socket
    mov     %cx, %bx

    mov     $NB_CFG, %ecx                 # MSR:C001_001F
    _RDMSR                              # EDX has InitApicIdCpuIdLo bit

    mov     %bh, %cl                      # CL = APIC ID size
    mov     $1, %al                       # Convert APIC ID size to an AND mask
    shl     %cl, %al                      # AL = 2^APIC ID size
    dec     %al                          # AL = mask for relative core number
    xor     %ah, %ah                      # AX = mask for relative core number
    bt      $(INIT_APIC_ID_CPU_ID_LO-32), %edx # InitApicIdCpuIdLo == 1?
    #.if (!carry?)                       # Br if yes
    jc      0f
        mov     $8, %ch                   # Calculate core number shift count
        sub     %cl, %ch                  # CH = core shift count
        mov     %ch, %cl
        shr     %cl, %di                  # Right justify core number
    #.endif
   0:
    and     %ax, %di                      # DI = socket-relative core number

    mov     %si, %cx                      # CX = raw mailbox value
    shr     $10, %cx                      # CL[1:0] = ModuleType or #nodes per socket (0-SCM, 1-MCM)
    and     $3, %cl                       # Isolate ModuleType
    xor     %bh, %bh                      # BX = Number of enabled cores in the socket
    shr     %cl, %bx                      # BX = Number of enabled cores per node
    xor     %dx, %dx                      # Clear upper word for div
    mov     %di, %ax                      # AX = socket-relative core number
    div     %bx                          # DX = node-relative core number
    movzx   %si, %eax                     # prepare return value, [23:16]=shared Core# (=0, not shared)
    and     $0x000F, %ax                   # AX = node number
    shl     $8, %ax                       # [15:8]=node#
    mov     %dl, %al                      # [7:0]=core# (relative to node)
    mov     %eax, %esi                    # ESI = return value
1:
 bts     $FLAG_IS_PRIMARY, %esi        # all Family 10h cores are primary
node_core_f10_exit\@:
.endm


/*****************************************************************************
**                      Family 12h MACROS
*****************************************************************************/
/*****************************************************************************
*
* AMD_ENABLE_STACK_FAMILY_HOOK_F12 Macro - Stackless
*
*   Set any family specific controls needed to enable the use of
*   cache as general storage before main memory is available.
*
* Inputs:
*       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
* Outputs:
*       none
*
* Family 12h requirements (BKDG section 2.3.3):
*   The following requirements must be satisfied prior to using the cache as general storage:
*   * Paging must be disabled.
*   * MSRC001_0015[INVD_WBINVD]=0
*   * MSRC001_1020[DIS_SS]=1
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
*   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
*   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=1
*   * MSRC001_1022[DIS_HW_PF]=1
*   * MSRC001_1029[ClflushSerialize]=1
*   * No INVD or WBINVD, no exceptions, page faults or interrupts
*****************************************************************************/
.macro  AMD_ENABLE_STACK_FAMILY_HOOK_F12

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x03, %al                     # Is this family 12h?
    jnz     fam12_enable_stack_hook_exit\@ # Br if no

    mov     $DC_CFG, %ecx                 # MSR:C001_1022
    _RDMSR
    bts     $DC_DIS_SPEC_TLB_RLD, %eax    # Disable speculative DC-TLB reloads
    bts     $DIS_CLR_WBTOL2_SMC_HIT, %eax # Disable self modifying code check buffer
    bts     $DIS_HW_PF, %eax              # Disable hardware prefetches
    _WRMSR

    dec     %cx   #IC_CFG                # MSR:C001_1021
    _RDMSR
    bts     $IC_DIS_SPEC_TLB_RLD, %eax    # Disable speculative IC-TLB reloads
    _WRMSR

    dec     %cx   #LS_CFG                # MSR:C001_1020
    _RDMSR
    bts     $DIS_SS, %eax                 # Disabled Streaming store functionality
    _WRMSR

    mov     $HWCR, %ecx                   # MSR C001_0015
    _RDMSR
    bt      $FLAG_STACK_REENTRY , %esi                 # Check if stack has already been set
    jc      fam12_skipClearingBit4
    btr     $INVD_WBINVD, %eax            # disable INVD -> WBINVD conversion
    _WRMSR

fam12_skipClearingBit4:
    mov     $DE_CFG, %ecx                 # MSR:C001_1029
    _RDMSR
    bts     $CL_FLUSH_SERIALIZE, %eax     # Serialize all CL Flush actions
    _WRMSR

fam12_enable_stack_hook_exit\@:
.endm

/*****************************************************************************
*
* AMD_DISABLE_STACK_FAMILY_HOOK_F12 Macro - Stackless
*
*   Return any family specific controls to their 'standard'
*   settings for using cache with main memory.
*
* Inputs:
*       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
* Outputs:
*       none
*
* Family 12h requirements:
*   * INVD or WBINVD
*   * MSRC001_0015[INVD_WBINVD]=1
*   * MSRC001_1020[DIS_SS]=0
*   * MSRC001_1021[IC_DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DC_DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=0
*   * MSRC001_1022[DIS_HW_PF]=0
*   * MSRC001_1029[ClflushSerialize]=0
*****************************************************************************/
.macro  AMD_DISABLE_STACK_FAMILY_HOOK_F12

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x03, %al                     # Is this family 12h?
    jnz     fam12_disable_stack_hook_exit\@ # Br if no

    mov     $DC_CFG, %ecx                 # MSR:C001_1022
    _RDMSR
    btr     $DC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative DC-TLB reloads
    btr     $DIS_CLR_WBTOL2_SMC_HIT, %eax # Enable self modifying code check buffer
    btr     $DIS_HW_PF, %eax              # Enable Hardware prefetches
    _WRMSR

    dec     %cx   #IC_CFG                # MSR:C001_1021
    _RDMSR
    btr     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative IC-TLB reloads
    _WRMSR

    dec     %cx   #LS_CFG                # MSR:C001_1020
    _RDMSR
    btr     $DIS_SS, %eax                 # Turn on Streaming store functionality
    _WRMSR

    mov     $DE_CFG, %ecx                 # MSR:C001_1029
    _RDMSR
    btr     $CL_FLUSH_SERIALIZE, %eax
    _WRMSR

    #--------------------------------------------------------------------------
    # Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    mov     $HWCR, %ecx                    # MSR:0000_0015h
    _RDMSR
    mov     %ax, %bx                      # Save INVD -> WBINVD bit
    btr     $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                # Clear the cache tag RAMs
    mov     %bx, %ax                      # Restore INVD -> WBINVD bit
    _WRMSR

    #--------------------------------------------------------------------------
    # End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

fam12_disable_stack_hook_exit\@:
.endm

/*****************************************************************************
*
* GET_NODE_ID_CORE_ID_F12 Macro - Stackless
*
*   Read family specific values to determine the node and core
*   numbers for the core executing this code.
*
* Inputs:
*     none
* Outputs:
*     SI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
*****************************************************************************/
.macro  GET_NODE_ID_CORE_ID_F12


    cmp     $-1, %si                      # Has node/core already been discovered?
    jnz     node_core_f12_exit\@          # Br if yes

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x03, %al                     # Is this family 12h?
    jnz     node_core_f12_exit\@          # Br if no

    shr     $24, %ebx                     # CPUID_0000_0001_EBX[31:24]: initial local APIC physical ID
    bts     $FLAG_IS_PRIMARY, %ebx        # all family 12h cores are primary
    mov     %ebx, %esi                    # ESI = Node#=0, core number
node_core_f12_exit\@:
.endm

/*****************************************************************************
**                      Family 14h MACROS
*****************************************************************************/
/*****************************************************************************
*
* AMD_ENABLE_STACK_FAMILY_HOOK_F14 Macro - Stackless
*
*   Set any family specific controls needed to enable the use of
*   cache as general storage before main memory is available.
*
* Inputs:
*       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
* Outputs:
*       none
*
* Family 14h requirements (BKDG section 2.3.3):
*   * Paging must be disabled.
*   * MSRC001_0015[INVD_WBINVD]=0.
*   * MSRC001_1020[DisStreamSt]=1.
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1. Disable speculative ITLB reloads.
*   * MSRC001_1022[DIS_HW_PF]=1.
*   * No INVD or WBINVD, no exceptions, page faults or interrupts
*****************************************************************************/
.macro  AMD_ENABLE_STACK_FAMILY_HOOK_F14

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x05, %al                     # Is this family 14h?
    jnz     fam14_enable_stack_hook_exit\@ # Br if no

    mov     $DC_CFG, %ecx                 # MSR:C001_1022
    _RDMSR
    bts     $DIS_HW_PF, %eax              # Disable hardware prefetches
    _WRMSR

    dec     %cx  #IC_CFG                 # MSR:C001_1021
    _RDMSR
    bts     $IC_DIS_SPEC_TLB_RLD, %eax    # Disable speculative TLB reloads
    _WRMSR

    dec     %cx  #LS_CFG                 # MSR:C001_1020
    _RDMSR
    bts     $DIS_STREAM_ST, %eax          # Disabled Streaming store functionality
    _WRMSR

    mov     $HWCR, %ecx                   # MSR C001_0015
    _RDMSR
    bt      $FLAG_STACK_REENTRY, %esi     # Check if stack has already been set
    jc      fam14_skipClearingBit4
    btr     $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion
    _WRMSR
fam14_skipClearingBit4:                 # Keeping this label

fam14_enable_stack_hook_exit\@:
.endm

/*****************************************************************************
*
* AMD_DISABLE_STACK_FAMILY_HOOK_F14 Macro - Stackless
*
*   Return any family specific controls to their 'standard'
*   settings for using cache with main memory.
*
* Inputs:
*       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
* Outputs:
*       none
*
* Family 14h requirements:
*   * INVD or WBINVD
*   * MSRC001_0015[INVD_WBINVD]=1.
*   * MSRC001_1020[DisStreamSt]=0.
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0.
*   * MSRC001_1022[DIS_HW_PF]=0.
*****************************************************************************/
.macro  AMD_DISABLE_STACK_FAMILY_HOOK_F14

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x05, %al                     # Is this family 14h?
    jnz     fam14_disable_stack_hook_exit\@ # Br if no

    mov     $LS_CFG, %ecx                 # MSR:C001_1020
    _RDMSR
    btr     $DIS_STREAM_ST, %eax          # Turn on Streaming store functionality
    _WRMSR

    inc     %cx  #IC_CFG                 # MSR:C001_1021
    _RDMSR
    btr     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative DC-TLB reloads
    _WRMSR

    inc     %cx  #DC_CFG                 # MSR:C001_1022
    _RDMSR
    btr     $DIS_HW_PF, %eax              # Turn on hardware prefetches
    _WRMSR

    #--------------------------------------------------------------------------
    # Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    mov     $HWCR, %ecx                    # MSR:C001_0015h
    _RDMSR
    btr     $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                # Clear the cache tag RAMs
    bts     $INVD_WBINVD, %eax            # Turn on Conversion of INVD to WBINVD
    _WRMSR

    #--------------------------------------------------------------------------
    # End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

fam14_disable_stack_hook_exit\@:
.endm

/*****************************************************************************
*
* GET_NODE_ID_CORE_ID_F14 Macro - Stackless
*
*   Read family specific values to determine the node and core
*   numbers for the core executing this code.
*
* Inputs:
*     none
* Outputs:
*     SI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
*****************************************************************************/
.macro  GET_NODE_ID_CORE_ID_F14


    cmp     $-1, %si                    # Has node/core already been discovered?
    jnz     node_core_f14_exit\@          # Br if yes

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x05, %al                     # Is this family 14h?
    jnz     node_core_f14_exit\@          # Br if no

    xor     %esi, %esi                    # Node must be 0
    bts     $FLAG_IS_PRIMARY, %esi        # all family 14h cores are primary
    mov     $APIC_BASE_ADDRESS, %ecx      # MSR:0000_001B
    _RDMSR
    bt      $APIC_BSC, %eax               # Is this the BSC?
    jc      node_core_f14_exit\@          # Br if yes
    inc     %si                          # Set core to 1
node_core_f14_exit\@:
.endm



/*****************************************************************************
**                      Family 15h MACROS
*****************************************************************************/
/*****************************************************************************
*
* AMD_ENABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
*
*   Set any family specific controls needed to enable the use of
*   cache as general storage before main memory is available.
*
* Inputs:
*       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
* Outputs:
*       none
*
* Family 15h requirements (BKDG #42301 section 2.3.3):
*   * Paging must be disabled.
*   * MSRC001_0015[INVD_WBINVD]=0
*   * MSRC001_1020[DisSS]=1
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
*   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
*   * MSRC001_1022[DisHwPf]=1
*   * No INVD or WBINVD, no exceptions, page faults or interrupts
*****************************************************************************/
.macro  AMD_ENABLE_STACK_FAMILY_HOOK_F15

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x06, %al                     # Is this family 15h?
    jnz     fam15_enable_stack_hook_exit\@ # Br if no

    bt      $FLAG_STACK_REENTRY , %esi                 # Check if stack has already been set
    jc      fam15_skipClearingBit4
    mov     $HWCR, %ecx                   # MSR C001_0015
    _RDMSR
    btr     $INVD_WBINVD, %eax            # disable INVD -> WBINVD conversion
    _WRMSR

fam15_skipClearingBit4:
    mov     $LS_CFG, %ecx                 # MSR:C001_1020
    _RDMSR
    bts     $DIS_SS, %eax                 # Turn on Streaming store functionality disabled bit
    _WRMSR

    inc     %ecx  #IC_CFG                # MSR:C001_1021
    _RDMSR
    bts     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on Disable speculative IC-TLB reloads bit
    _WRMSR

    inc     %ecx  #DC_CFG                # MSR:C001_1022
    _RDMSR
    bts     $DC_DIS_SPEC_TLB_RLD, %eax    # Turn on Disable speculative DC-TLB reloads bit
    bts     $DIS_HW_PF, %eax              # Turn on Disable hardware prefetches bit
    _WRMSR

    mov     $CU_CFG3, %ecx                # MSR:C001_102B
    _RDMSR
    btr     $(COMBINE_CR0_CD - 32), %edx         # Clear CombineCr0Cd bit
    _WRMSR

fam15_enable_stack_hook_exit\@:
.endm


/*****************************************************************************
*
* AMD_DISABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
*
*   Return any family specific controls to their 'standard'
*   settings for using cache with main memory.
*
* Inputs:
*       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
* Outputs:
*       none
*
* Family 15h requirements:
*   * INVD or WBINVD
*   * MSRC001_0015[INVD_WBINVD]=1
*   * MSRC001_1020[DisSS]=0
*   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
*   * MSRC001_1022[DIS_HW_PF]=0
*****************************************************************************/
.macro  AMD_DISABLE_STACK_FAMILY_HOOK_F15

    AMD_CPUID   $CPUID_MODEL
    mov     %eax, %ebx                    # Save revision info to EBX
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $0x06, %al                     # Is this family 15h?
    jnz     fam15_disable_stack_hook_exit\@ # Br if no

    mov     $LS_CFG, %ecx                 # MSR:C001_1020
    #.if (ebx != 00600F00h)              ; Is this rev A0?
    cmp	$0x00600F00, %ebx
    jz	0f
    _RDMSR
    btr     $DIS_SS, %eax                 # Turn on Streaming store functionality
    _WRMSR
    #.endif
    0:                              # End workaround for errata 495 and 496

    inc     %ecx  #IC_CFG                # MSR:C001_1021
    _RDMSR
    btr     $IC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative TLB reloads
    _WRMSR

    inc     %ecx  #DC_CFG                # MSR:C001_1022
    _RDMSR
    btr     $DC_DIS_SPEC_TLB_RLD, %eax    # Turn on speculative TLB reloads
    #.if (ebx != 00600F00h)              # Is this rev A0?
    cmp	$0x00600F00, %ebx
    jz	0f
    btr     $DIS_HW_PF, %eax              # Turn on hardware prefetches
    #.endif                              # End workaround for erratum 498
    0:
    _WRMSR
    #--------------------------------------------------------------------------
    # Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    bt      $FLAG_IS_PRIMARY, %esi
    #.if (carry?)                        # Only clear cache from primary core
    jnc 0f
    mov     $HWCR, %ecx                    # MSR:C001_0015h
    _RDMSR
    btr     $INVD_WBINVD, %eax            # Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                # Clear the cache tag RAMs
    bts     $INVD_WBINVD, %eax            # Turn on Conversion of INVD to WBINVD
    _WRMSR
    #.endif                              # end
    0:

    #--------------------------------------------------------------------------
    # End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    #--------------------------------------------------------------------------

    mov     $CU_CFG3, %ecx                # MSR:C001_102B
    _RDMSR
    bts     $(COMBINE_CR0_CD - 32), %edx         # Set CombineCr0Cd bit
    _WRMSR

fam15_disable_stack_hook_exit\@:
.endm


/*****************************************************************************
*
* GET_NODE_ID_CORE_ID_F15 Macro - Stackless
*
*   Read family specific values to determine the node and core
*   numbers for the core executing this code.
*
* Inputs:
*     none
* Outputs:
*     SI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
*****************************************************************************/
.macro  GET_NODE_ID_CORE_ID_F15


    cmp     $-1, %si                      # Has node/core already been discovered?
    jnz     node_core_f15_exit\@          # Br if yes

    AMD_CPUID   $CPUID_MODEL
    shr     $20, %eax                     # AL = cpu extended family
    cmp     $06, %al                     # Is this family 15h?
    jnz     node_core_f15_exit\@          # Br if no

    xor     %esi, %esi                    # Assume BSC, clear local flags
    mov     $APIC_BASE_ADDRESS, %ecx      # MSR:0000_001B
    _RDMSR
    bt      $APIC_BSC, %eax               # Is this the BSC?
    jnc      node_core_f15_AP\@            # Br if no

    # This is the BSP.
    # Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
    mov     $0x8000C06C, %eax              # PCI address for D18F0x6C Link Initialization Control Register
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    in      %dx, %eax
    btr     $0, %eax                      # Set LinkInitializationControl[RouteTblDis] = 0
    out     %eax, %dx
    jmp     node_core_f15_shared\@        #

node_core_f15_AP\@:
    #
    # This is an AP. Routing tables have been enabled by the HT Init process.
    # Also, the MailBox register was set by the BSP during early init
    #   The Mailbox register content is formatted as follows:
    #         UINT32 Node:4;          // The node id of Core's node.
    #         UINT32 Socket:4;        // The socket of this Core's node.
    #         UINT32 Module:2;        // The internal module number for Core's node.
    #         UINT32 ModuleType:2;    // Single Module = 0, Multi-module = 1.
    #         UINT32 :20;             // Reserved
    #
    mov     $0x0C0000408, %ecx             # Read the family 15h mailbox
    _RDMSR                              #      MC4_MISC1[63:32]
    mov     %dx, %si                      # SI = raw mailbox contents (will extract node# from this)
    shr     $24, %ebx                     # BL = CPUID Fn0000_0001_EBX[LocalApicId]
    mov     %bx, %di                      # DI = Initial APIC ID (will extract core# from this)

    AMD_CPUID   $AMD_CPUID_APIC          #
    shr     $4, %ch                       # CH = ApicIdSize, #bits in APIC ID that show core#
    inc     %cl                          # CL = Number of enabled cores in the socket
    mov     %cx, %bx

    mov     $NB_CFG, %ecx
    _RDMSR                              # EDX has InitApicIdCpuIdLo bit

    mov     %bh, %cl                      # CL = APIC ID size
    mov     $1, %al                       # Convert APIC ID size to an AND mask
    shl     %cl, %al                      # AL = 2^APIC ID size
    dec     %al                          # AL = mask for relative core number
    xor     %ah, %ah                      # AX = mask for relative core number
    bt      $(INIT_APIC_ID_CPU_ID_LO-32), %edx # InitApicIdCpuIdLo == 1?
    #.if (!carry?)                       # Br if yes
    jc      0f
        mov     $8, %ch                   # Calculate core number shift count
        sub     %cl, %ch                  # CH = core shift count
        mov     %ch, %cl
        shr     %cl, %di                  # Right justify core number
    #.endif
    0:
    and     %ax, %di                      # DI = socket-relative core number

    mov     %si, %cx                      # CX = raw mailbox value
    shr     $10, %cx                      # CL[1:0] = ModuleType or #nodes per socket (0-SCM, 1-MCM)
    and     $3, %cl                       # Isolate ModuleType
    xor     %bh, %bh                      # BX = Number of enabled cores in the socket
    shr     %cl, %bx                      # BX = Number of enabled cores per node
    xor     %dx, %dx                      # Clear upper word for div
    mov     %di, %ax                      # AX = socket-relative core number
    div     %bx                          # DX = node-relative core number
    movzx   %si, %eax                     # Prepare return value
    and     $0x000F, %ax                   # AX = node number
    shl     $8,%ax                       # [15:8]=node#
    mov     %dl, %al                      # [7:0]=core# (relative to node)
    mov     %eax, %esi                    # ESI = node-relative core number

      #
      #   determine if this core shares MTRRs
      #
node_core_f15_shared\@:
    mov     $0x8000C580, %eax              # Compute Unit Status
    mov     %si, %bx
    shl     $3, %bh                       # Move node# to PCI Dev# field
    add     %bh, %ah                      # Adjust for node number
    mov     $0x0CF8, %dx
    out     %eax, %dx
    add     $4, %dx
    in      %dx, %eax                     # [3:0]=Enabled# [19:16]=DualCore

                                        # BL is MyCore#
    mov     $0x06, %cx                     # Use CH as 'first of pair' core#
    #.while (cl > 0)
    jmp  0f
    8:
        bt      $0, %eax                  # Is pair enabled?
        #.if (carry?)                    #
        jnc     1f
            mov     $0x01, %bh             #   flag core as primary
            bt      $16, %eax             # Is there a 2nd in the pair?
            #.if (carry?)                #
            jnc 4f
                #.break .if (ch == bl)   # Does 1st match MyCore#?
                cmp     %bl, %ch
                je      9f
                inc     %ch
                xor     %bh, %bh          #     flag core as NOT primary
                #.break .if (ch == bl)   # Does 2nd match MyCore#?
                cmp     %bl, %ch
                je      9f
                jmp     2f
            #.else                       # No 2nd core
            4:
                #.break .if (ch == bl)   # Does 1st match MyCore#?
                cmp     %bl, %ch
                je      9f
            #.endif
            2:
            inc     %ch
        #.endif
        1:
        shr     $1, %eax
        dec     %cl
    #.endw
    0:
    #.if (cl == 0)
    cmp $0x0, %cl
    ja	8b
    9:
    or	%cl, %cl
    jne 1f
        #Error - core# didn't match Compute Unit Status content
        bts     $FLAG_UNKNOWN_FAMILY, %esi
        bts     $FLAG_IS_PRIMARY, %esi    #   Set Is_Primary for unknowns
    #.endif
    1:
    #.if (bh != 0)                       # Check state of primary for the matched core
    or %bh, %bh
    je 2f
        bts     $FLAG_IS_PRIMARY, %esi    #   Set shared flag into return value
    #.endif
    2:

node_core_f15_exit\@:

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
