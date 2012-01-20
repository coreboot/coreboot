;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
; $Workfile:: cpcar.inc
;
; Description: CPCAR.INC - AGESA cache-as-RAM setup Include File
;
;*****************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*****************************************************************************
    .LIST
    .mmx

BSP_STACK_BASE_ADDR     EQU 30000h      ; Base address for primary cores stack
BSP_STACK_SIZE_64K      EQU 10000h      ; 64KB for BSP core
BSP_STACK_SIZE_32K      EQU 8000h       ; 32KB for BSP core
CORE0_STACK_BASE_ADDR   EQU 80000h      ; Base address for primary cores stack
CORE0_STACK_SIZE        EQU 4000h       ; 16KB for primary cores
CORE1_STACK_BASE_ADDR   EQU 40000h      ; Base address for AP cores
CORE1_STACK_SIZE        EQU 1000h       ; 4KB for each AP cores

L3_CONTROL_REGISTER     EQU 8100C3B8h   ; Bus 0, Device 18h, Function 3, Offset 1B8h

APIC_BASE_ADDRESS       EQU 0000001Bh
    APIC_BSC                EQU 8       ; Boot Strap Core

APIC_MSG_REG            EQU 380h        ; Location of BSC message
    APIC_MSG            EQU 00DE00ADh   ; Message data
APIC_CMD_LO_REG         EQU 300h        ; APIC command low
APIC_CMD_HI_REG         EQU 310h        ; APIC command high
    CMD_REG_TO_READ_DATA EQU 00000338h  ; APIC command for remote read of APIC_MSG_REG
    REMOTE_READ_STS       EQU 00030000h ; Remote read status mask
    REMOTE_DELIVERY_PEND  EQU 00010000h ; Remote read is pending
    REMOTE_DELIVERY_DONE  EQU 00020000h ; Remote read is complete
    DELIVERY_STS_BIT    EQU 12          ; Delivery status valid bit
APIC_ID_REG             EQU 0020h       ; Local APIC ID offset
    APIC20_APICID       EQU 24
APIC_REMOTE_READ_REG    EQU 00C0h       ; Remote read offset

; Flags can only run from bits 31 to 24.  Bits 23:0 are in use.
AMD_CU_NEED_TO_WAIT     EQU 31
AMD_CU_SEND_INVD_MSG    EQU 30
AMD_CU_RESTORE_ES       EQU 29

AMD_MTRR_VARIABLE_BASE0 EQU 0200h
AMD_MTRR_VARIABLE_BASE6 EQU 020Ch
AMD_MTRR_VARIABLE_BASE7 EQU 020Eh
    VMTRR_VALID             EQU     11
    MTRR_TYPE_WB            EQU     06h
    MTRR_TYPE_WP            EQU     05h
    MTRR_TYPE_WT            EQU     04h
    MTRR_TYPE_UC            EQU     00h
AMD_MTRR_VARIABLE_MASK7 EQU 020Fh
AMD_MTRR_FIX64k_00000   EQU 0250h
AMD_MTRR_FIX16k_80000   EQU 0258h
AMD_MTRR_FIX16k_A0000   EQU 0259h
AMD_MTRR_FIX4k_C0000    EQU 0268h
AMD_MTRR_FIX4k_C8000    EQU 0269h
AMD_MTRR_FIX4k_D0000    EQU 026Ah
AMD_MTRR_FIX4k_D8000    EQU 026Bh
AMD_MTRR_FIX4k_E0000    EQU 026Ch
AMD_MTRR_FIX4k_E8000    EQU 026Dh
AMD_MTRR_FIX4k_F0000    EQU 026Eh
AMD_MTRR_FIX4k_F8000    EQU 026Fh

AMD_MTRR_DEFTYPE        EQU 02FFh
    WB_DRAM_TYPE            EQU 1Eh     ; MemType - memory type
    MTRR_DEF_TYPE_EN        EQU 11      ; MtrrDefTypeEn - variable and fixed MTRRs default enabled
    MTRR_DEF_TYPE_FIX_EN    EQU 10      ; MtrrDefTypeEn - fixed MTRRs default enabled

HWCR                    EQU 0C0010015h  ; Hardware Configuration
    INVD_WBINVD             EQU 4       ;   INVD to WBINVD conversion

IORR_BASE               EQU 0C0010016h  ; IO Range Regusters Base/Mask, 2 pairs
                                        ;   uses 16h - 19h
TOP_MEM                 EQU 0C001001Ah  ; Top of Memory
TOP_MEM2                EQU 0C001001Dh  ; Top of Memory2

LS_CFG                  EQU 0C0011020h  ; Load-Store Configuration
    DIS_SS                  EQU 28      ;   Family 10h,12h,15h:Disable Streaming Store functionality
    DIS_STREAM_ST           EQU 28      ;   Family 14h:DisStreamSt - Disable Streaming Store functionality

IC_CFG                  EQU 0C0011021h  ; Instruction Cache Config Register
    IC_DIS_SPEC_TLB_RLD     EQU 9       ;   Disable speculative TLB reloads
    DIS_IND                 EQU 14      ;   Family 10-14h:Disable Indirect Branch Predictor
    DIS_I_CACHE             EQU 14      ;   Family 15h:DisICache - Disable Indirect Branch Predictor

DC_CFG                  EQU 0C0011022h  ; Data Cache Configuration
    DC_DIS_SPEC_TLB_RLD     EQU 4       ;   Disable speculative TLB reloads
    DIS_CLR_WBTOL2_SMC_HIT  EQU 8       ;   self modifying code check buffer bit
    DIS_HW_PF               EQU 13      ;   Hardware prefetches bit

CU_CFG                  EQU 0C0011023h  ; Family 15h: Combined Unit Configuration
    L2_WAY_LOCK_EN          EQU 23      ;   L2WayLock - L2 way lock enable
    L2_FIRST_LOCKED_WAY     EQU 19      ;   L2FirstLockedWay - first L2 way lockedh
    L2_FIRST_LOCKED_WAY_OR_MASK  EQU 000780000h

DE_CFG                  EQU 0C0011029h  ; Decode Configuration
    CL_FLUSH_SERIALIZE      EQU 23      ;   Family 12h,15h: CL Flush Serialization

BU_CFG2                 EQU 0C001102Ah  ; Family 10h-14h: Bus Unit Configuration 2
CU_CFG2                 EQU 0C001102Ah  ; Family 15h: Combined Unit Configuration 2
    F10_CL_LINES_TO_NB_DIS  EQU 15      ;   ClLinesToNbDis - allows WP code to be cached in L2
    IC_DIS_SPEC_TLB_WR      EQU 35      ;   IcDisSpecTlbWr - ITLB speculative writes

CU_CFG3                 EQU 0C001102Bh  ; Combined Unit Configuration 3
    COMBINE_CR0_CD          EQU 49      ;   Combine CR0.CD for both cores of a compute unit


CR0_PE                  EQU 0           ; Protection Enable
CR0_NW                  EQU 29          ; Not Write-through
CR0_CD                  EQU 30          ; Cache Disable
CR0_PG                  EQU 31          ; Paging Enable

; CPUID Functions

CPUID_MODEL             EQU 1
AMD_CPUID_FMF           EQU 80000001h   ; Family Model Features information
AMD_CPUID_L2Cache       EQU 80000006h   ; L2/L3 cache info
AMD_CPUID_APIC          EQU 80000008h   ; Long Mode and APIC info., core count
    APIC_ID_CORE_ID_SIZE     EQU 12     ; ApicIdCoreIdSize bit position

NB_CFG                  EQU 0C001001Fh  ; Northbridge Configuration Register
    INIT_APIC_ID_CPU_ID_LO   EQU 54     ;   InitApicIdCpuIdLo - is core# in high or low half of APIC ID?
    ENABLE_CF8_EXT_CFG       EQU 46     ;   EnableCf8ExtCfg - enable CF8 extended configuration cycles

MTRR_SYS_CFG            EQU 0C0010010h  ; System Configuration Register
  CHX_TO_DIRTY_DIS          EQU 16      ;   ChxToDirtyDis    Change to dirty disable
  SYS_UC_LOCK_EN            EQU 17      ;   SysUcLockEn      System lock command enable
  MTRR_FIX_DRAM_EN          EQU 18      ;   MtrrFixDramEn    MTRR fixed RdDram and WrDram attributes enable
  MTRR_FIX_DRAM_MOD_EN      EQU 19      ;   MtrrFixDramModEn MTRR fixed RdDram and WrDram modification enable
  MTRR_VAR_DRAM_EN          EQU 20      ;   MtrrVarDramEn    MTRR variable DRAM enable
  MTRR_TOM2_EN              EQU 21      ;   MtrrTom2En       MTRR top of memory 2 enable

PERF_CONTROL3           EQU 0C0010003h  ; Performance event control three
    PERF_CONTROL3_RESERVE_L EQU 00200000h ; Preserve the reserved bits
    PERF_CONTROL3_RESERVE_H EQU 0FCF0h  ; Preserve the reserved bits
    CONFIG_EVENT_L          EQU 0F0E2h  ; All cores with level detection
    CONFIG_EVENT_H          EQU 4       ; Increment count by number of event
                                        ; occured in clock cycle
    EVENT_ENABLE            EQU 22      ; Enable the event
PERF_COUNTER3           EQU 0C0010007h  ; Performance event counter three

; Local use flags, in upper most byte of ESI
FLAG_UNKNOWN_FAMILY               EQU 24    ; Signals that the family# of the installed processor is not recognized
FLAG_STACK_REENTRY                EQU 25    ; Signals that the environment has made a re-entry (2nd) call to set up the stack
FLAG_IS_PRIMARY                   EQU 26    ; Signals that this core is the primary within the compute unit
FLAG_CORE_NOT_IDENTIFIED          EQU 27    ; Signals that the cores/compute units of the installed processor is not recognized
FLAG_FORCE_32K_STACK              EQU 28    ; Signals that to force 32KB stack size for BSP core

; Error code returned in EDX by AMD_ENABLE_STACK
IFNDEF CPU_EVENT_UNKNOWN_PROCESSOR_FAMILY
       CPU_EVENT_UNKNOWN_PROCESSOR_FAMILY     EQU   008010500h
ENDIF
IFNDEF CPU_EVENT_STACK_REENTRY
       CPU_EVENT_STACK_REENTRY                EQU   008020500h
ENDIF
IFNDEF CPU_EVENT_CORE_NOT_IDENTIFIED
       CPU_EVENT_CORE_NOT_IDENTIFIED          EQU   008030500h
ENDIF

; AGESA_STATUS values
IFNDEF AGESA_SUCCESS
       AGESA_SUCCESS  EQU 0
ENDIF
IFNDEF AGESA_WARNING
       AGESA_WARNING  EQU 4
ENDIF
IFNDEF AGESA_FATAL
       AGESA_FATAL    EQU 7
ENDIF
;;***************************************************************************
;;
;;                      CPU MACROS - PUBLIC
;;
;;***************************************************************************
_WRMSR macro
  db  0Fh, 30h
  endm

_RDMSR macro
  db  0Fh, 32h
  endm

AMD_CPUID MACRO arg0
  IFB <arg0>
    mov   eax, 1
    db    0Fh, 0A2h                     ; Execute instruction
    bswap eax
    xchg  al, ah                        ; Ext model in al now
    rol   eax, 8                        ; Ext model in ah, model in al
    and   ax, 0FFCFh                    ; Keep 23:16, 7:6, 3:0
  ELSE
    mov   eax, arg0
    db    0Fh, 0A2h
  ENDIF
ENDM

;---------------------------------------------------
; LoadTableAddress
;       Due to the various assembly methodologies used by BIOS vendors, this macro is needed to abstract the
;       loading of the address of a table. The default is the standard LEA instruction with table address.
;       The IBV that needs to use an alternative method can define their version of the macro prior to including
;       this file into their source.
;       An alternative example:
;         LoadTableAddress  MACRO MyTable
;           LEA   eax, -(LAST_ADDRESS - MyTable)
;---------------------------------------------------
IFNDEF LoadTableAddress
  LoadTableAddress        Macro TargetTable
       LEA   eax, TargetTable
  ENDM
ENDIF


;;***************************************************************************
;;
;;                      CPU STRUCTURES - PUBLIC
;;
;;***************************************************************************
CPU_FAMILY_INFO     STRUC
    L2_MIN_SIZE         WORD    ?       ; Minimum size of the L2 cache for this family, in K
    NUM_SHARED_CORES    BYTE    ?       ; Number of cores sharing an L2 cache
    L2_ALLOC_MEM        BYTE    ?       ; L2 space reserved for memory training, in K
    L2_ALLOC_EXE        WORD    ?       ; L2 space reserved for EXE CACHE, in K. 0 means unlimited.
    SIZE_ADDRESS_BUS    BYTE    ?       ; Number of address bits supported by this family
    FAMILY_RESERVED     BYTE    ?       ; reserved, pad to DWORD size
CPU_FAMILY_INFO     ENDS


;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK MACRO

    AMD_ENABLE_STACK_FAMILY_HOOK_F10
    AMD_ENABLE_STACK_FAMILY_HOOK_F12
    AMD_ENABLE_STACK_FAMILY_HOOK_F14
    AMD_ENABLE_STACK_FAMILY_HOOK_F15

ENDM

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;----------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK MACRO

    AMD_DISABLE_STACK_FAMILY_HOOK_F10
    AMD_DISABLE_STACK_FAMILY_HOOK_F12
    AMD_DISABLE_STACK_FAMILY_HOOK_F14
    AMD_DISABLE_STACK_FAMILY_HOOK_F15

ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     SI[7:0] = Core# (0..N, relative to node)
;     SI[15:8]= Node# (0..N)
;     SI[23:16]= reserved
;     SI[24]=   flag: 1=Family Unrecognized
;     SI[25]=   flag: 1=Interface re-entry call
;     SI[26]=   flag: 1=Core is primary of compute unit
;     SI[31:27]= reserved, =0
;
; Destroyed:
;       eax, ebx, ecx, edx, esi
;---------------------------------------------------
GET_NODE_ID_CORE_ID MACRO

    mov     si, -1
    GET_NODE_ID_CORE_ID_F10
    GET_NODE_ID_CORE_ID_F12
    GET_NODE_ID_CORE_ID_F14
    GET_NODE_ID_CORE_ID_F15
      ;
      ; Check for unrecognized Family
      ;
    .if (si == -1)                      ; Has family (node/core) been discovered?
        mov     esi, ( (1 SHL FLAG_UNKNOWN_FAMILY)+(1 SHL FLAG_IS_PRIMARY) ) ; No, Set error code, Only let BSP continue
        mov     ecx, APIC_BASE_ADDRESS  ; MSR:0000_001B
        _RDMSR
        bt      eax, APIC_BSC           ;   Is this the BSC?
        .if (!carry?)
            ; No, this is an AP
            hlt                         ;       Kill APs
        .endif
    .endif
ENDM




;;***************************************************************************
;;                      Family 10h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F10 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 10h requirements (BKDG section 2.3.3):
;   * Paging disabled
;   * MSRC001_0015[INVDWBINVD]=0
;   * MSRC001_1021[DIS_IND]=1
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=1
;   * MSRC001_1022[DIS_HW_PF]=1
;   * MSRC001_102A[IcDisSpecTlbWr]=1
;   * MSRC001_102A[ClLinesToNbDis]=1
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK_F10 MACRO
    local   fam10_enable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 01h                     ; Is this family 10h?
    jnz     fam10_enable_stack_hook_exit ; Br if no

    ; Errata #385
    ; F3x1B8[23] = 1 before enabling L3 cache through CR0[30](CD)
    mov     eax, ebx                    ; Restore revision info to EAX
    .if (al >= 80h)                     ; Is this Revision D and later?
        mov     ecx, NB_CFG             ; MSR:C001_001F
        _RDMSR                          ; EDX has EnableCf8ExtCfg bit
        bts     edx, (ENABLE_CF8_EXT_CFG - 32)
        _WRMSR

        mov     eax, esi                ; Get node# from esi[15:8]
        and     eax, 0000FF00h
        shl     eax, (11 - 8)           ; Device#
        add     eax, L3_CONTROL_REGISTER
        mov     dx, 0CF8h               ; PCI Read
        out     dx, eax
        mov     dx, 0CFCh
        in      eax, dx

        or      eax, (1 shl 23)         ; F3x1B8[23] = 1

        out     dx, eax                 ; PCI Write
    .endif

    mov     ecx, DC_CFG                 ; MSR:C001_1022
    _RDMSR
    bts     eax, DC_DIS_SPEC_TLB_RLD    ; Turn on Disable speculative DTLB reloads bit
    bts     eax, DIS_CLR_WBTOL2_SMC_HIT ; Turn on Disable the self modifying code check buffer bit
    bts     eax, DIS_HW_PF              ; Turn on Disable hardware prefetches bit
    _WRMSR

    dec     cx                          ; MSR:C001_1021
    _RDMSR
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on Disable speculative TLB reloads bit
    bts     eax, DIS_IND                ; Turn on Disable indirect branch predictor
    _WRMSR

    mov     ecx, BU_CFG2                ; MSR C001_102A
    _RDMSR
    bts     eax, F10_CL_LINES_TO_NB_DIS  ; Allow BIOS ROM to be cached in the IC
    bts     edx, (IC_DIS_SPEC_TLB_WR-32) ;Disable speculative writes to the ITLB
    _WRMSR

    mov     ecx, HWCR                   ; MSR C001_0015
    _RDMSR

    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    .if (!carry?)
        btr     eax, INVD_WBINVD        ; disable INVD -> WBINVD conversion
        _WRMSR
    .endif

    mov eax, esi                        ; load core#
    .if (al == 0)                       ; If (BSP)
        mov     ecx, PERF_COUNTER3      ;   Select performance counter three
                                        ;   to count number of CAR evictions
        xor     eax, eax                ;   Initialize the lower part of the counter to zero
        xor     edx, edx                ;   Initializa the upper part of the counter to zero
        _WRMSR                          ;   Save it
        mov     ecx, PERF_CONTROL3      ;   Select the event control three
        _RDMSR                          ;   Get the current setting
        and     eax, PERF_CONTROL3_RESERVE_L  ; Preserve the reserved bits
        or      eax, CONFIG_EVENT_L     ;   Set the lower part of event register to
                                        ;   select CAR Corruption occurred by any cores
        and     dx, PERF_CONTROL3_RESERVE_H  ; Preserve the reserved bits
        or      dx, CONFIG_EVENT_H      ;   Set the upper part of event register
        _WRMSR                          ;   Save it
        bts     eax, EVENT_ENABLE       ;   Enable it
        _WRMSR                          ;   Save it
    .endif                              ; endif

fam10_enable_stack_hook_exit:
ENDM

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F10 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 10h requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_1021[DIS_IND]=0
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=0
;   * MSRC001_1022[DIS_HW_PF]=0
;   * MSRC001_102A[IcDisSpecTlbWr]=0
;   * MSRC001_102A[ClLinesToNbDis]=0
;----------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK_F10 MACRO
    local   fam10_disable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 01h                     ; Is this family 10h?
    jnz     fam10_disable_stack_hook_exit ; Br if no

    mov     ecx, DC_CFG                 ; MSR:C001_1022
    _RDMSR
    btr     eax, DC_DIS_SPEC_TLB_RLD    ; Enable speculative TLB reloads
    btr     eax, DIS_CLR_WBTOL2_SMC_HIT ; Allow self modifying code check buffer
    btr     eax, DIS_HW_PF              ; Allow hardware prefetches
    _WRMSR

    dec     cx                          ; MSR:C001_1021
    _RDMSR
    btr     eax, DIS_IND                ; Turn on indirect branch predictor
    btr     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on speculative TLB reloads
    _WRMSR

    mov     ecx, BU_CFG2                ; MSR:C001_102A
    _RDMSR
    btr     eax, F10_CL_LINES_TO_NB_DIS  ; Return L3 to normal mode
    btr     edx, (IC_DIS_SPEC_TLB_WR-32) ;Re-enable speculative writes to the ITLB
    _WRMSR

    ;--------------------------------------------------------------------------
    ; Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------
    mov     ecx, HWCR                   ; MSR:0000_0015
    _RDMSR
    mov     bx, ax                      ; Save INVD -> WBINVD bit
    btr     eax, INVD_WBINVD            ; Disable INVD -> WBINVD conversion for the invd instruction.
    _WRMSR
    invd                                ; Clear the cache tag RAMs
    mov     ax, bx                      ; Restore INVD -> WBINVD bit
    _WRMSR

    ;--------------------------------------------------------------------------
    ; End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

    mov     ecx, PERF_CONTROL3          ; Select the event control three
    _RDMSR                              ; Retrieve the current value
    btc     eax, EVENT_ENABLE           ; Is event enable, complement it as well
    jnc     fam10_disable_stack_hook_exit ; No
    cmp     ax, CONFIG_EVENT_L          ; Is the lower part of event set to capture the CAR Corruption
    jne     fam10_disable_stack_hook_exit ; No
    cmp     dl,  CONFIG_EVENT_H         ; Is the upper part of event set to capture the CAR Corruption
    jne     fam10_disable_stack_hook_exit ; No
    _WRMSR                              ; Disable the event

fam10_disable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F10 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
GET_NODE_ID_CORE_ID_F10 MACRO

    local   node_core_f10_exit
    local   end_of_f10h_data

    IFNDEF FAM10H_INFO_STRUCT
        jmp     end_of_f10h_data
        ; Family 10h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        FAM10H_INFO_STRUCT  CPU_FAMILY_INFO {  512,         1,          16,     384,      48,         0}
end_of_f10h_data:
    ENDIF

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     node_core_f10_exit          ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 01h                     ; Is this family 10h?
    jnz     node_core_f10_exit          ; Br if no

    LoadTableAddress(FAM10H_INFO_STRUCT)
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    _RDMSR
    bt      eax, APIC_BSC               ; Is this the BSC?
    .if (carry?)
        ; This is the BSP.
        ; Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
        mov     eax, 8000C06Ch          ;   PCI address for D18F0x6C Link Initialization Control Register
        mov     dx, 0CF8h
        out     dx, eax
        add     dx, 4
        in      eax, dx
        btr     eax, 0                  ;   Set LinkInitializationControl[RouteTblDis] = 0
        out     dx, eax
    .else
        ; This is an AP. Routing tables have been enabled by the HT Init process.
        ; Also, the MailBox register was set by the BSP during early init
        ;   The Mailbox register content is formatted as follows:
        ;         UINT32 Node:4;        // The node id of Core's node.
        ;         UINT32 Socket:4;      // The socket of this Core's node.
        ;         UINT32 Module:2;      // The internal module number for Core's node.
        ;         UINT32 ModuleType:2;  // Single Module = 0, Multi-module = 1.
        ;         UINT32 :20;           // Reserved
        ;
        mov     ecx, 0C0000408h         ; Read the family 10h mailbox
        _RDMSR                          ;          MC4_MISC1[63:32]
        mov     si, dx                  ;   SI = raw mailbox contents (will extract node# from this)
        shr     ebx, 24                 ;   BL = CPUID Fn0000_0001_EBX[LocalApicId]
        mov     di, bx                  ;   DI = Initial APIC ID (will extract core# from this)

        AMD_CPUID   AMD_CPUID_APIC      ;
        shr     ch, 4                   ;   CH = ApicIdSize, #bits in APIC ID that show core#
        inc     cl                      ;   CL = Number of enabled cores in the socket
        mov     bx, cx

        mov     ecx, NB_CFG             ;   MSR:C001_001F
        _RDMSR                          ;   EDX has InitApicIdCpuIdLo bit

        mov     cl, bh                  ;   CL = APIC ID size
        mov     al, 1                   ;   Convert APIC ID size to an AND mask
        shl     al, cl                  ;   AL = 2^APIC ID size
        dec     al                      ;   AL = mask for relative core number
        xor     ah, ah                  ;   AX = mask for relative core number
        bt      edx, (INIT_APIC_ID_CPU_ID_LO-32) ; InitApicIdCpuIdLo == 1?
        .if (!carry?)                   ;   Br if yes
            mov     ch, 8               ;   Calculate core number shift count
            sub     ch, cl              ;   CH = core shift count
            mov     cl, ch
            shr     di, cl              ;   Right justify core number
        .endif
        and     di, ax                  ;   DI = socket-relative core number

        mov     cx, si                  ;   CX = raw mailbox value
        shr     cx, 10                  ;   CL[1:0] = ModuleType or #nodes per socket (0-SCM, 1-MCM)
        and     cl, 3                   ;   Isolate ModuleType
        xor     bh, bh                  ;   BX = Number of enabled cores in the socket
        shr     bx, cl                  ;   BX = Number of enabled cores per node
        xor     dx, dx                  ;   Clear upper word for div
        mov     ax, di                  ;   AX = socket-relative core number
        div     bx                      ;   DX = node-relative core number
        movzx   eax, si                 ;   prepare return value (clears flags)
        and     ax, 000Fh               ;   AX = node number
        shl     ax, 8                   ;   [15:8]=node#
        mov     al, dl                  ;   [7:0]=core# (relative to node)
        mov     esi, eax                ;   ESI = return value
    .endif                              ; end: Is_AP
    bts     esi, FLAG_IS_PRIMARY        ; all Family 10h cores are primary
    bts     esi, FLAG_FORCE_32K_STACK   ; all Family 10h to force 32KB stack size for BSP core

node_core_f10_exit:
ENDM


;;***************************************************************************
;;                      Family 12h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F12 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 12h requirements (BKDG section 2.3.3):
;   The following requirements must be satisfied prior to using the cache as general storage:
;   * Paging must be disabled.
;   * MSRC001_0015[INVD_WBINVD]=0
;   * MSRC001_1020[DIS_SS]=1
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=1
;   * MSRC001_1022[DIS_HW_PF]=1
;   * MSRC001_1029[ClflushSerialize]=1
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK_F12 MACRO
    local   fam12_enable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 03h                     ; Is this family 12h?
    jnz     fam12_enable_stack_hook_exit ; Br if no

    mov     ecx, DC_CFG                 ; MSR:C001_1022
    _RDMSR
    bts     eax, DC_DIS_SPEC_TLB_RLD    ; Disable speculative DC-TLB reloads
    bts     eax, DIS_CLR_WBTOL2_SMC_HIT ; Disable self modifying code check buffer
    bts     eax, DIS_HW_PF              ; Disable hardware prefetches
    _WRMSR

    dec     cx   ;IC_CFG                ; MSR:C001_1021
    _RDMSR
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; Disable speculative IC-TLB reloads
    _WRMSR

    dec     cx   ;LS_CFG                ; MSR:C001_1020
    _RDMSR
    bts     eax, DIS_SS                 ; Disabled Streaming store functionality
    _WRMSR

    mov     ecx, HWCR                   ; MSR C001_0015
    _RDMSR
    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    .if (!carry?)
        btr     eax, INVD_WBINVD        ;   disable INVD -> WBINVD conversion
        _WRMSR
    .endif

    mov     ecx, DE_CFG                 ; MSR:C001_1029
    _RDMSR
    bts     eax, CL_FLUSH_SERIALIZE     ; Serialize all CL Flush actions
    _WRMSR

fam12_enable_stack_hook_exit:
ENDM

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F12 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 12h requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_1020[DIS_SS]=0
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_CLR_WBTOL2_SMC_HIT]=0
;   * MSRC001_1022[DIS_HW_PF]=0
;   * MSRC001_1029[ClflushSerialize]=0
;---------------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK_F12 MACRO
    local   fam12_disable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 03h                     ; Is this family 12h?
    jnz     fam12_disable_stack_hook_exit ; Br if no

    mov     ecx, DC_CFG                 ; MSR:C001_1022
    _RDMSR
    btr     eax, DC_DIS_SPEC_TLB_RLD    ; Turn on speculative DC-TLB reloads
    btr     eax, DIS_CLR_WBTOL2_SMC_HIT ; Enable self modifying code check buffer
    btr     eax, DIS_HW_PF              ; Enable Hardware prefetches
    _WRMSR

    dec     cx   ;IC_CFG                ; MSR:C001_1021
    _RDMSR
    btr     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on speculative IC-TLB reloads
    _WRMSR

    dec     cx   ;LS_CFG                ; MSR:C001_1020
    _RDMSR
    btr     eax, DIS_SS                 ; Turn on Streaming store functionality
    _WRMSR

    mov     ecx, DE_CFG                 ; MSR:C001_1029
    _RDMSR
    btr     eax, CL_FLUSH_SERIALIZE
    _WRMSR

    ;--------------------------------------------------------------------------
    ; Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

    mov     ecx, HWCR                    ; MSR:0000_0015h
    _RDMSR
    mov     bx, ax                      ; Save INVD -> WBINVD bit
    btr     eax, INVD_WBINVD            ; Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                ; Clear the cache tag RAMs
    mov     ax, bx                      ; Restore INVD -> WBINVD bit
    _WRMSR

    ;--------------------------------------------------------------------------
    ; End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

fam12_disable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F12 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;       none
; Outputs:
;       ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;       MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
GET_NODE_ID_CORE_ID_F12 MACRO

    local   node_core_f12_exit
    local   end_of_f12h_data

    IFNDEF FAM12H_INFO_STRUCT
        jmp     end_of_f12h_data
        ; Family 12h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        FAM12H_INFO_STRUCT  CPU_FAMILY_INFO {  512,         1,          0,     384,      40,         0 }
end_of_f12h_data:
    ENDIF

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     node_core_f12_exit          ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 03h                     ; Is this family 12h?
    jnz     node_core_f12_exit          ; Br if no

    LoadTableAddress(FAM12H_INFO_STRUCT)
    movd    mm5, eax                    ; load pointer to Family Info Struc

    shr     ebx, 24                     ; CPUID_0000_0001_EBX[31:24]: initial local APIC physical ID
    bts     ebx, FLAG_IS_PRIMARY        ; all family 12h cores are primary
    mov     esi, ebx                    ; ESI = Node#=0, core number
    bts     esi, FLAG_FORCE_32K_STACK   ; all Family 12h to force 32KB stack size for BSP core
node_core_f12_exit:
ENDM


;;***************************************************************************
;;                      Family 14h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F14 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 14h requirements (BKDG section 2.3.3):
;   * Paging must be disabled.
;   * MSRC001_0015[INVD_WBINVD]=0.
;   * MSRC001_1020[DisStreamSt]=1.
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1. Disable speculative ITLB reloads.
;   * MSRC001_1022[DIS_HW_PF]=1.
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK_F14 MACRO
    local   fam14_enable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 05h                     ; Is this family 14h?
    jnz     fam14_enable_stack_hook_exit ; Br if no

    mov     ecx, DC_CFG                 ; MSR:C001_1022
    _RDMSR
    bts     eax, DIS_HW_PF              ; Disable hardware prefetches
    _WRMSR

    dec     cx  ;IC_CFG                 ; MSR:C001_1021
    _RDMSR
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; Disable speculative TLB reloads
    _WRMSR

    dec     cx  ;LS_CFG                 ; MSR:C001_1020
    _RDMSR
    bts     eax, DIS_STREAM_ST          ; Disabled Streaming store functionality
    _WRMSR

    mov     ecx, HWCR                   ; MSR C001_0015
    _RDMSR
    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    .if (!carry?)
        btr     eax, INVD_WBINVD        ; disable INVD -> WBINVD conversion
        _WRMSR
    .endif

fam14_enable_stack_hook_exit:
ENDM

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F14 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 14h requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1.
;   * MSRC001_1020[DisStreamSt]=0.
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0.
;   * MSRC001_1022[DIS_HW_PF]=0.
;---------------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK_F14 MACRO
    local   fam14_disable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 05h                     ; Is this family 14h?
    jnz     fam14_disable_stack_hook_exit ; Br if no

    mov     ecx, LS_CFG                 ; MSR:C001_1020
    _RDMSR
    btr     eax, DIS_STREAM_ST          ; Turn on Streaming store functionality
    _WRMSR

    inc     cx  ;IC_CFG                 ; MSR:C001_1021
    _RDMSR
    btr     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on speculative DC-TLB reloads
    _WRMSR

    inc     cx  ;DC_CFG                 ; MSR:C001_1022
    _RDMSR
    btr     eax, DIS_HW_PF              ; Turn on hardware prefetches
    _WRMSR

    ;--------------------------------------------------------------------------
    ; Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

    mov     ecx, HWCR                    ; MSR:C001_0015h
    _RDMSR
    btr     eax, INVD_WBINVD            ; Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                ; Clear the cache tag RAMs
    bts     eax, INVD_WBINVD            ; Turn on Conversion of INVD to WBINVD
    _WRMSR

    ;--------------------------------------------------------------------------
    ; End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

fam14_disable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F14 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
GET_NODE_ID_CORE_ID_F14 MACRO

    local   node_core_f14_exit
    local   end_of_f14h_data

    IFNDEF FAM14H_INFO_STRUCT
        jmp     end_of_f14h_data
        ; Family 14h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        FAM14H_INFO_STRUCT  CPU_FAMILY_INFO {  256,         1,          0,     0,        36,         0}
end_of_f14h_data:
    ENDIF

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     node_core_f14_exit          ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 05h                     ; Is this family 14h?
    jnz     node_core_f14_exit          ; Br if no

    LoadTableAddress(FAM14H_INFO_STRUCT)
    movd    mm5, eax                    ; load pointer to Family Info Struc

    shr     ebx, 24                     ; CPUID_0000_0001_EBX[31:24]: initial local APIC physical ID
    bts     ebx, FLAG_IS_PRIMARY        ; all family 14h cores are primary
    mov     esi, ebx                    ; ESI = Node#=0, core number
node_core_f14_exit:
ENDM



;;***************************************************************************
;;                      Family 15h MACROS
;;***************************************************************************
;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
;
;   Set any family specific controls needed to enable the use of
;   cache as general storage before main memory is available.
;
; Inputs:
;       ESI - node#, core#, flags from GET_NODE_ID_CORE_ID
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h requirements (BKDG #42301 section 2.3.3):
;   * Paging must be disabled.
;   * MSRC001_0015[INVD_WBINVD]=0
;   * MSRC001_1020[DisSS]=1
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=1
;   * MSRC001_1022[DisHwPf]=1
;   * No INVD or WBINVD, no exceptions, page faults or interrupts
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK_F15 MACRO
    local   fam15_enable_stack_hook_exit

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 06h                     ; Is this family 15h?
    jnz     fam15_enable_stack_hook_exit ; Br if no

    bt      esi, FLAG_STACK_REENTRY     ; Check if stack has already been set
    .if (!carry?)
        mov     ecx, HWCR               ; MSR C001_0015
        _RDMSR
        btr     eax, INVD_WBINVD        ; disable INVD -> WBINVD conversion
        _WRMSR
    .endif

    mov     ecx, LS_CFG                 ; MSR:C001_1020
    _RDMSR
    bts     eax, DIS_SS                 ; Turn on Streaming store functionality disabled bit
    _WRMSR

    inc     ecx  ;IC_CFG                ; MSR:C001_1021
    _RDMSR
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on Disable speculative IC-TLB reloads bit
    _WRMSR

    inc     ecx  ;DC_CFG                ; MSR:C001_1022
    _RDMSR
    bts     eax, DC_DIS_SPEC_TLB_RLD    ; Turn on Disable speculative DC-TLB reloads bit
    bts     eax, DIS_HW_PF              ; Turn on Disable hardware prefetches bit
    _WRMSR

    mov     eax, ebx                    ; Restore revision info to EAX
    shr     eax, 16
    and     al, 0Fh                     ; AL = cpu extended model
    .if (al == 01h)                     ; Is this TN?
        ; Do TN enable stack special
        mov     ecx, CU_CFG             ; MSR:C001_1023
        _RDMSR
        bt      eax, L2_WAY_LOCK_EN     ; Check if way 15 of the L2 needs to be reserved
        .if (!carry?)
            bts     eax, L2_WAY_LOCK_EN
            or      eax, L2_FIRST_LOCKED_WAY_OR_MASK
            _WRMSR
        .endif
    .endif
    ; Do Standard Family 15 work
    mov     ecx, CU_CFG3                ; MSR:C001_102B
    _RDMSR
    btr     edx, (COMBINE_CR0_CD - 32)  ; Clear CombineCr0Cd bit
    _WRMSR

fam15_enable_stack_hook_exit:
ENDM


;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK_F15 Macro - Stackless
;
;   Return any family specific controls to their 'standard'
;   settings for using cache with main memory.
;
; Inputs:
;       ESI - [31:24] flags; [15,8]= Node#; [7,0]= core#
; Outputs:
;       none
; Destroyed:
;       eax, ebx, ecx, edx
;
; Family 15h requirements:
;   * INVD or WBINVD
;   * MSRC001_0015[INVD_WBINVD]=1
;   * MSRC001_1020[DisSS]=0
;   * MSRC001_1021[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_SPEC_TLB_RLD]=0
;   * MSRC001_1022[DIS_HW_PF]=0
;---------------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK_F15 MACRO
    local   fam15_disable_stack_hook_exit
    local   fam15_disable_stack_remote_read_exit

    AMD_CPUID   CPUID_MODEL
    mov     ebx, eax                    ; Save revision info to EBX
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 06h                     ; Is this family 15h?
    jnz     fam15_disable_stack_hook_exit ; Br if no

    mov     edi, ebx                    ; Save revision info to EDI
    AMD_CPUID   AMD_CPUID_APIC
    mov     al, cl                      ; AL = number of cores - 1
    shr     cx, APIC_ID_CORE_ID_SIZE    ; CL = ApicIdCoreIdSize
    mov     bx, 1
    shl     bl, cl                      ; BL = theoretical number of cores on socket
    dec     bx                          ; BL = core number on socket mask
    mov     ah, bl                      ; AH = core number on socket mask
    mov     ebx, edi                    ; Restore revision info to EBX
    mov     di, ax                      ; DI[15:8] = core number mask, DI[7:0] = number of cores - 1

    and     ebx, 0F00FFh
    mov     eax, ebx
    shr     eax, 8
    or      bx, ax                      ; Save Extended Model, Model and Stepping to BX
                                        ; [11:8] = Extended Model, [7:4] = Model, [3:0] = Stepping

    ;; A handshake is required to ensure that all cores on a node invalidate in sync.
    mov     ecx, APIC_BASE_ADDRESS
    _RDMSR
    mov     dx, bx                      ; Save Extended Model, Model and Stepping to DX
    shl     edx, 16                     ; EDX[31:16] = Extended Model, Model and Stepping
    mov     ebx, eax                    ; EBX = LAPIC base
    xor     ecx, ecx                    ; Zero out CU flags
    bts     ecx, AMD_CU_NEED_TO_WAIT    ; Default to waiting
    bts     ecx, AMD_CU_SEND_INVD_MSG   ; Default to signaling
    mov     eax, CR0
    bt      ax, CR0_PE                  ; Are we in protected mode?
    .if (!carry?)
        bts     ecx, AMD_CU_RESTORE_ES  ; Indicate ES restore is required
        mov     cx, es                  ; Save ES segment register to CX
        xor     ax, ax
        mov     es, ax                  ; Set ES to big real mode selector for 4GB access
    .endif

    and     bx, 0F000h                  ; EBX = LAPIC base, offset 0
    or      bl, APIC_ID_REG
    mov     eax, es:[ebx]               ; EAX[31:24] = APIC ID
    shr     eax, APIC20_APICID          ; AL = APIC ID
    mov     ah, al                      ; AH = APIC ID
    mov     dx, di                      ; DH = core mask
    and     ah, dh                      ; AH = core number
    .if (zero?)
        ;; Core 0 of a socket
        btr     ecx, AMD_CU_SEND_INVD_MSG ; No need to signal after INVD
        .if (dl != 0)
            ;; This socket has multiple cores
            and     bx, 0F000h          ; EBX = LAPIC base, offset 0
            or      bx, APIC_MSG_REG
            mov     edi, APIC_MSG
            mov     es:[ebx], edi       ; Signal for non core 0s to complete CAR breakdown
        .else
            btr     ecx, AMD_CU_NEED_TO_WAIT ; No need to wait on a single core CPU
        .endif
    .endif

    bt     ecx, AMD_CU_NEED_TO_WAIT
    .if (carry?)
        .if (ah == dl)
            ;; This is the highest numbered core on this socket -- wait on core 0
            not     dh                  ; Flip the mask to determine local core 0's APIC ID
            and     al, dh              ; AL = target APIC ID
        .else
            ;; All other cores (including core 0) wait on the next highest core.
            ;; In this way, cores will halt in a cascading fashion down to 0.
            inc     al
        .endif

        shl     eax, APIC20_APICID
        and     bx, 0F000h
        or      bx, APIC_CMD_HI_REG
        mov     es:[ebx], eax           ; Set target APIC ID

        ;; Use bits 23:16 as a timeout for unresponsive cores
        ror     ecx, 8
        mov     ch, 0FFh
        stc
        .while (carry?)
            and     bx, 0F000h          ; EBX = LAPIC base, offset 0
            or      bx, APIC_CMD_LO_REG
            mov     eax, CMD_REG_TO_READ_DATA
            mov     es:[ebx], eax       ; Fire remote read IPI
            inc     ch                  ; Pre increment the timeout
            stc
            .while (carry?)
                dec     ch              ; Check the timeout
                jz      fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                mov     eax, es:[ebx]
                bt      eax, DELIVERY_STS_BIT
            .endw
            stc
            .while (carry?)
                mov     eax, es:[ebx]
                and     eax, REMOTE_READ_STS
                .if (eax == REMOTE_DELIVERY_PEND)
                    dec     ch          ; Check the timeout
                    jz      fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                    stc
                .else
                    clc
                .endif
            .endw
            .if (eax == REMOTE_DELIVERY_DONE)
                and     bx, 0F000h      ; EBX = LAPIC base, offset 0
                or      bl, APIC_REMOTE_READ_REG
                mov     eax, es:[ebx]
                .if (eax == APIC_MSG)
                    clc
                .else
                    stc
                .endif
            .else
                dec     ch              ; Check the timeout
                jz      fam15_disable_stack_remote_read_exit   ; Branch if there is an unresponsive core
                stc
            .endif
        .endw

fam15_disable_stack_remote_read_exit:
        rol     ecx, 8                  ; Restore ECX

    .endif
    bt      ecx, AMD_CU_RESTORE_ES
    .if (carry?)
        mov     es, cx
    .endif
    mov     edi, ecx                    ; EDI = CU flags
    shr     edx, 16
    mov     bx, dx                      ; Restore Extended Model, Model and Stepping

    ;; Handshaking complete.  Continue tearing down CAR.
    mov     ecx, LS_CFG                 ; MSR:C001_1020
    .if (bx != 0)                       ; Is this OR A0?
        _RDMSR
        btr     eax, DIS_SS             ; Turn on Streaming store functionality
        _WRMSR
    .endif                              ; End workaround for errata 495 and 496

    inc     ecx  ;IC_CFG                ; MSR:C001_1021
    _RDMSR
    btr     eax, IC_DIS_SPEC_TLB_RLD    ; Turn on speculative TLB reloads
    _WRMSR

    inc     ecx  ;DC_CFG                ; MSR:C001_1022
    _RDMSR
    btr     eax, DC_DIS_SPEC_TLB_RLD    ; Turn on speculative TLB reloads
    .if (bx != 0)                       ; Is this OR A0?
        btr     eax, DIS_HW_PF          ; Turn on hardware prefetches
    .endif                              ; End workaround for erratum 498
    _WRMSR

    mov     ecx, HWCR                   ; MSR:C001_0015h
    _RDMSR
    btr     eax, INVD_WBINVD            ; Disable INVD -> WBINVD conversion
    _WRMSR
    invd                                ; Clear the cache tag RAMs

    .if (bh == 01h)                     ; Is this TN?
        ; Do TN enable stack special
        mov     ecx, CU_CFG             ; MSR:C001_1023
        _RDMSR
        shr      eax, L2_FIRST_LOCKED_WAY
        and      eax, 01Fh
        .if (eax == 01Fh)               ; Check if way 15 of the L2 needs to be reserved
            mov     ecx, CU_CFG         ; MSR:C001_1023
            _RDMSR
            btr     eax, L2_WAY_LOCK_EN
            _WRMSR
        .endif
    .endif
    ; Do Standard Family 15 work
    mov     ecx, HWCR                   ; MSR:C001_0015h
    _RDMSR
    bts     eax, INVD_WBINVD            ; Turn on INVD -> WBINVD conversion
    _WRMSR

    mov     ecx, CU_CFG3                ; MSR:C001_102B
    _RDMSR
    bts     edx, (COMBINE_CR0_CD - 32)  ; Set CombineCr0Cd bit
    _WRMSR

    bt      edi, AMD_CU_SEND_INVD_MSG
    .if (carry?)
        ;; Non core zero needs to signal to core 0 to proceed
        mov     ecx, APIC_BASE_ADDRESS
        _RDMSR
        mov     ebx, eax                ; EBX = LAPIC base
        and     bx, 0F000h              ; EBX = LAPIC base, offset 0
        or      bx, APIC_MSG_REG
        mov     eax, APIC_MSG
        mov     es:[ebx], eax           ; Signal for core 0 to complete CAR breakdown
    .endif

fam15_disable_stack_hook_exit:
ENDM


;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID_F15 Macro - Stackless
;
;   Read family specific values to determine the node and core
;   numbers for the core executing this code.
;
; Inputs:
;     none
; Outputs:
;     ESI = core#, node# & flags (see GET_NODE_ID_CORE_ID macro above)
;     MM5 = 32b pointer to family info structure
; Destroyed:
;       eax, ebx, ecx, edx, esi, mm5
;---------------------------------------------------
GET_NODE_ID_CORE_ID_F15 MACRO

    local   node_core_f15_exit
    local   end_of_f15h_data

    IFNDEF FAM15H_INFO_STRUCT
        jmp     end_of_f15h_data
        ; Family 15h Info Structure:          L2Size,  #SharedCores, AllocMem, AllocExe, SzAddrBus, pad
        FAM15H_INFO_STRUCT  CPU_FAMILY_INFO {  512,         2,          0,     0,         48,         0    }
end_of_f15h_data:
    ENDIF

    cmp     si, -1                      ; Has node/core already been discovered?
    jnz     node_core_f15_exit          ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20                     ; AL = cpu extended family
    cmp     al, 06h                     ; Is this family 15h?
    jnz     node_core_f15_exit          ; Br if no

    LoadTableAddress(FAM15H_INFO_STRUCT)
    movd    mm5, eax                    ; load pointer to Family Info Struc

    xor     esi, esi                    ; Assume BSC, clear local flags
    mov     ecx, APIC_BASE_ADDRESS      ; MSR:0000_001B
    _RDMSR
    bt      eax, APIC_BSC               ; Is this the BSC?
    .if (carry?)
        ; This is the BSP.
        ; Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
        mov     eax, 8000C06Ch          ;   PCI address for D18F0x6C Link Initialization Control Register
        mov     dx, 0CF8h
        out     dx, eax
        add     dx, 4
        in      eax, dx
        btr     eax, 0                  ;   Set LinkInitializationControl[RouteTblDis] = 0
        out     dx, eax
    .else                               ;
        ; This is an AP. Routing tables have been enabled by the HT Init process.
        ; Also, the MailBox register was set by the BSP during early init
        ;   The Mailbox register content is formatted as follows:
        ;         UINT32 Node:4;        // The node id of Core's node.
        ;         UINT32 Socket:4;      // The socket of this Core's node.
        ;         UINT32 Module:2;      // The internal module number for Core's node.
        ;         UINT32 ModuleType:2;  // Single Module = 0, Multi-module = 1.
        ;         UINT32 :20;           // Reserved
        ;
        mov     ecx, 0C0000408h         ;   Read the family 15h mailbox
        _RDMSR                          ;      MC4_MISC1[63:32]
        mov     si, dx                  ;   SI = raw mailbox contents (will extract node# from this)
        shr     ebx, 24                 ;   BL = CPUID Fn0000_0001_EBX[LocalApicId]
        mov     di, bx                  ;   DI = Initial APIC ID (will extract core# from this)

        AMD_CPUID   AMD_CPUID_APIC      ;
        shr     ch, 4                   ;   CH = ApicIdSize, #bits in APIC ID that show core#
        inc     cl                      ;   CL = Number of enabled cores in the socket
        mov     bx, cx

        mov     ecx, NB_CFG
        _RDMSR                          ;   EDX has InitApicIdCpuIdLo bit

        mov     cl, bh                  ;   CL = APIC ID size
        mov     al, 1                   ;   Convert APIC ID size to an AND mask
        shl     al, cl                  ;   AL = 2^APIC ID size
        dec     al                      ;   AL = mask for relative core number
        xor     ah, ah                  ;   AX = mask for relative core number
        bt      edx, (INIT_APIC_ID_CPU_ID_LO-32) ; InitApicIdCpuIdLo == 1?
        .if (!carry?)                   ;   Br if yes
            mov     ch, 8               ;     Calculate core number shift count
            sub     ch, cl              ;     CH = core shift count
            mov     cl, ch              ;
            shr     di, cl              ;     Right justify core number
        .endif                          ;
        and     di, ax                  ;   DI = socket-relative core number

        mov     cx, si                  ;   CX = raw mailbox value
        shr     cx, 10                  ;   CL[1:0] = ModuleType or #nodes per socket (0-SCM, 1-MCM)
        and     cl, 3                   ;   Isolate ModuleType
        xor     bh, bh                  ;   BX = Number of enabled cores in the socket
        shr     bx, cl                  ;   BX = Number of enabled cores per node
        xor     dx, dx                  ;   Clear upper word for div
        mov     ax, di                  ;   AX = socket-relative core number
        div     bx                      ;   DX = node-relative core number
        movzx   eax, si                 ;   Prepare return value
        and     ax, 000Fh               ;   AX = node number
        shl     ax, 8                   ;   [15:8]=node#
        mov     al, dl                  ;   [7:0]=core# (relative to node)
        mov     esi, eax                ;   ESI = node-relative core number
    .endif                              ; end

    ;
    ;   determine if this core shares MTRRs
    ;
    mov     eax, 8000C580h              ; Compute Unit Status
    mov     bx, si                      ; load node#(bh), core#(bl)
    shl     bh, 3                       ; Move node# to PCI Dev# field
    add     ah, bh                      ; Adjust PCI adress for node number
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    in      eax, dx                     ; [3:0]=Enabled; [19:16]=DualCore

                                        ; BL is MyCore#  , BH is primary flag
    mov     cx, 06h                     ; Use CH as 'first of pair' core#
    .while (cl > 0)
        bt      eax, 0                  ;   Is pair enabled?
        .if (carry?)                    ;
            mov     bh, 01h             ;   flag core as primary
            bt      eax, 16             ;   Is there a 2nd in the pair?
            .if (carry?)                ;
                .break .if (ch == bl)   ;     Does 1st match MyCore#?
                inc     ch
                xor     bh, bh          ;     flag core as NOT primary
                .break .if (ch == bl)   ;     Does 2nd match MyCore#?
            .else                       ;   No 2nd core
                .break .if (ch == bl)   ;     Does 1st match MyCore#?
            .endif
            inc     ch
        .endif
        shr     eax, 1
        dec     cl
    .endw
    .if (cl == 0)
        ;Error - core# didn't match Compute Unit Status content
        bts     esi, FLAG_CORE_NOT_IDENTIFIED
        bts     esi, FLAG_IS_PRIMARY    ;   Set Is_Primary for unknowns
    .endif
    .if (bh != 0)                       ; Check state of primary for the matched core
        bts     esi, FLAG_IS_PRIMARY    ;   Set shared flag into return value
    .endif
node_core_f15_exit:
ENDM



