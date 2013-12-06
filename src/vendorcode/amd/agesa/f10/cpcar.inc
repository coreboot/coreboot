;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
; $Workfile:: cpcar.inc
;
; Description: CPCAR.INC - AGESA cache-as-RAM setup Include File
;
;*****************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
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

CORE0_STACK_BASE_ADDR   EQU 40000h    ;base address for primary cores stack
CORE1_STACK_BASE_ADDR   EQU 60000h    ;base address for AP cores
CORE0_STACK_SIZE        EQU 4000h     ;16KB for primary cores
CORE1_STACK_SIZE        EQU 1000h     ;4KB for each AP cores

AMD_MTRR_FIX64k_00000    EQU 250h
AMD_MTRR_FIX16k_80000    EQU 258h
AMD_MTRR_FIX16k_A0000    EQU 259h
AMD_MTRR_FIX4k_C0000     EQU 268h
AMD_MTRR_FIX4k_C8000     EQU 269h
AMD_MTRR_FIX4k_D0000     EQU 26Ah
AMD_MTRR_FIX4k_D8000     EQU 26Bh
AMD_MTRR_FIX4k_E0000     EQU 26Ch
AMD_MTRR_FIX4k_E8000     EQU 26Dh
AMD_MTRR_FIX4k_F0000     EQU 26Eh
AMD_MTRR_FIX4k_F8000     EQU 26Fh
AMD_MTRR_DEFTYPE        EQU 2FFh

AMD_MTRR_VARIABLE_BASE6 EQU 020Ch

TOP_MEM                 EQU 0C001001Ah

MtrrFixDramEn      EQU 18
MtrrFixDramModEn   EQU 19
MtrrVarDramEn      EQU 20

WB_DRAM_TYPE       EQU 1Eh

HWCR                   EQU 0C0010015h
    INVD_WBINVD        EQU 4

LS_CFG                 EQU 0C0011020h
    DisStreamSt        EQU 28

IC_CFG                 EQU 0C0011021h
    DIS_SPEC_TLB_RLD   EQU 9
    DIS_IND            EQU 14

BU_CFG2                EQU 0C001102Ah
    ClLinesToNbDis     EQU 15

DC_DIS_SPEC_TLB_RLD    EQU  4       ; disable speculative TLB reloads
DIS_CLR_WBTOL2_SMC_HIT EQU  8       ; self modifying code check buffer bit
DIS_HW_PF              EQU 13       ; hardware prefetches bit
IC_DIS_SPEC_TLB_RLD    EQU  9       ; disable speculative TLB reloads

CR0_CD    EQU 40000000h      ; CR0[30] = Cache Disable
CR0_NW    EQU 20000000h      ; CR0[29] = Not Writethrough

; CPUID Functions

CPUID_MODEL             EQU     1
AMD_CPUID_FMF           EQU     80000001h   ; Family Model Features information

NB_CFG                  EQU     0C001001Fh
    bEnableCF8ExtCfg    EQU     00004000h   ; [46]
    bInitAPICCPUIDLo    EQU     00400000h   ; [54]

MTRR_SYS_CFG        EQU 0C0010010h
  ChxToDirtyDis     EQU 16
  SysUcLockEn       EQU 17
  MtrrFixDramEn     EQU 18
  MtrrFixDramModEn  EQU 19
  MtrrVarDramEn     EQU 20
  MtrrTom2En        EQU 21

PERF_COUNTER3            EQU 0C0010007h ; Performance event counter three
PERF_CONTROL3            EQU 0C0010003h ; Performance event control three
PERF_CONTROL3_RESERVE_L  EQU 00200000h  ; Preserve the reserved bits
PERF_CONTROL3_RESERVE_H  EQU 0FCF0h     ; Preserve the reserved bits
CONFIG_EVENT_L           EQU 0F0E2h     ; All cores with level detection
CONFIG_EVENT_H           EQU 4          ; Increment count by number of event
                                        ; occured in clock cycle
EVENT_ENABLE             EQU 22         ; Enable the event

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

;----------------------------------------------
;
; AMD_DISABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;----------------------------------------------
AMD_DISABLE_STACK_FAMILY_HOOK MACRO

    AMD_DISABLE_STACK_FAMILY_HOOK_F10
    AMD_DISABLE_STACK_FAMILY_HOOK_F14

ENDM

AMD_DISABLE_STACK_FAMILY_HOOK_F10 MACRO

    local   cl_line_to_nb_enable_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 01h            ; Is this family 10h?
    jnz     cl_line_to_nb_enable_exit ; Br if no

    ; For Family 10h, clear BU_CFG2[15] ClLinesToNbDis bit to Re-Enable L3 cache
    ; Warning: icache for ROM (IO space) is disabled
    mov     ecx, 0C001102Ah             ; BU_CFG2
    _RDMSR
    btr     eax, 15
    btr     edx, 3
    _WRMSR

    ; * MSRC001_1021[DIS_IND]=0. Disable indirect branch predictor.
    mov     ecx, IC_CFG         ; IC_CFG
    _RDMSR
    btr     eax, DIS_IND        ; turn off Disable indirect branch predictor
    _WRMSR

    ;--------------------------------------------------------------------------
    ; Begin critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

    ; Disable INVD_WBINVD across the invd instruction.
    mov     cx, 0015h           ; HWCR
    _RDMSR
    mov     bx, ax              ; save INVD -> WBINVD bit
    and     al, 0EFh            ; disable INVD -> WBINVD conversion
    _WRMSR

    invd                        ; Clear the cache tag RAMs

    ; Restore INVD_WBINVD
    mov     ax, bx              ; restore INVD -> WBINVD bit
    _WRMSR

    ;--------------------------------------------------------------------------
    ; End critical sequence in which EAX, BX, ECX, and EDX must be preserved.
    ;--------------------------------------------------------------------------

    mov ecx, PERF_CONTROL3        ; Select the event control three
    _RDMSR                        ; Retrieve the current value
    btc eax, EVENT_ENABLE         ; Is event enable, complement it as well
    jnc cl_line_to_nb_enable_exit ; No
    cmp ax, CONFIG_EVENT_L        ; Is the lower part of event set to capture the CAR Corruption
    jne cl_line_to_nb_enable_exit ; No
    cmp  dl,  CONFIG_EVENT_H      ; Is the upper part of event set to capture the CAR Corruption
    jne cl_line_to_nb_enable_exit ; No
    _WRMSR                        ; Disable the event

cl_line_to_nb_enable_exit:
ENDM

AMD_DISABLE_STACK_FAMILY_HOOK_F14 MACRO

    local   fam14_enable_stack_hook_exit

    ;
    ; For Family 14h
    ;
    ; Restore the following configuration state:
    ; * MSRC001_0015[INVD_WBINVD]=1.
    ; * MSRC001_1020[DisStreamSt]=0.
    ; * MSRC001_1021[DIS_SPEC_TLB_RLD]=0.
    ; * MSRC001_1022[DIS_HW_PF]=0.
    ;
    ; When BIOS is done executing from WP-IO the following steps are followed:
    ; * MSRC001_102A[ClLinesToNbDis]=1.

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 05h            ; Is this family 10h?
    jnz     fam14_enable_stack_hook_exit ; Br if no

    ; * MSRC001_0015[INVD_WBINVD]=0.
    mov     ecx, HWCR
    _RDMSR
    bts     eax, INVD_WBINVD            ; turn off Convert INVD to WBINVD bit
    _WRMSR
    ; * MSRC001_1020[DisStreamSt]=1.
    mov     ecx, LS_CFG
    _RDMSR
    btr     eax, DisStreamSt            ; turn on Streaming store functionality disabled bit
    _WRMSR
    ; * MSRC001_1021[DIS_SPEC_TLB_RLD]=1. Disable speculative ITLB reloads.
    inc     ecx                         ; mov     ecx, IC_CFG
    _RDMSR
    btr     eax, DIS_SPEC_TLB_RLD       ; turn on Disable speculative TLB reloads bit
    _WRMSR
    ; * MSRC001_1022[DIS_HW_PF]=1.
    inc     ecx
    _RDMSR
    btr     eax, DIS_HW_PF              ; turn on Disable hardware prefetches bit
    _WRMSR

fam14_enable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; AMD_ENABLE_STACK_FAMILY_HOOK Macro - Stackless
;
;---------------------------------------------------
AMD_ENABLE_STACK_FAMILY_HOOK MACRO

    AMD_ENABLE_STACK_FAMILY_HOOK_F10
    AMD_ENABLE_STACK_FAMILY_HOOK_F14

ENDM

AMD_ENABLE_STACK_FAMILY_HOOK_F10 MACRO

    local   cl_line_to_nb_disable_exit

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 01h            ; Is this family 10h?
    jnz     cl_line_to_nb_disable_exit ; Br if no

    ; For Family 10h, set BU_CFG2[15] ClLinesToNbDis bit to disable L3 cache
    ; Allow BIOS ROM to be cached in the IC
    mov     ecx, 0C001102Ah         ; BU_CFG2
    _RDMSR
    bts     eax, 15
    bts     edx, 3
    _WRMSR

    mov     cx, 0015h           ; HWCR
    _RDMSR
    and     al, 0EFh            ; disable INVD -> WBINVD conversion
    _WRMSR

    ; * MSRC001_1021[DIS_IND]=1. Disable indirect branch predictor.
    mov     ecx, IC_CFG         ; IC_CFG
    _RDMSR
    bts     eax, DIS_IND        ; turn on Disable indirect branch predictor
    _WRMSR

    .if (di == 0)    ;core 0
       mov ecx, PERF_COUNTER3            ; Select performance counter three
                                         ; to count number of CAR Corruption
       xor eax, eax                      ; Initialize the lower part of the counter to zero
       xor edx, edx                      ; Initializa the upper part of the counter to zero
       _WRMSR                            ; Save it
       mov ecx, PERF_CONTROL3            ; Select the event control three
       _RDMSR                            ; Get the current setting
       and eax, PERF_CONTROL3_RESERVE_L  ; Preserve the reserved bits
       or eax, CONFIG_EVENT_L            ; Set the lower part of event register to
                                         ; select CAR Corruption occurred by any cores
       and dx, PERF_CONTROL3_RESERVE_H   ; Preserve the reserved bits
       or  dx, CONFIG_EVENT_H            ; Set the upper part of event register
       _WRMSR                            ; Save it
       bts eax, EVENT_ENABLE             ; Enable it
       _WRMSR                            ; Save it
    .endif

cl_line_to_nb_disable_exit:
ENDM

AMD_ENABLE_STACK_FAMILY_HOOK_F14 MACRO

    local   fam14_enable_stack_hook_exit

    ;
    ; For Family 14h
    ;
    ; The following requirements must be satisfied prior to using the cache as general storage:
    ; * Paging must be disabled.
    ; * MSRC001_0015[INVD_WBINVD]=0.
    ; * MSRC001_1020[DisStreamSt]=1.
    ; * MSRC001_1021[DIS_SPEC_TLB_RLD]=1. Disable speculative ITLB reloads.
    ; * MSRC001_1022[DIS_HW_PF]=1.
    ; * MSRC001_102A[ClLinesToNbDis]=1.

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 05h            ; Is this family 10h?
    jnz     fam14_enable_stack_hook_exit ; Br if no

    ; * MSRC001_0015[INVD_WBINVD]=0.
    mov     ecx, HWCR
    _RDMSR
    btr     eax, INVD_WBINVD            ; turn off Convert INVD to WBINVD bit
    _WRMSR
    ; * MSRC001_1020[DisStreamSt]=1.
    mov     ecx, LS_CFG
    _RDMSR
    bts     eax, DisStreamSt            ; turn on Streaming store functionality disabled bit
    _WRMSR
    ; * MSRC001_1021[DIS_SPEC_TLB_RLD]=1. Disable speculative ITLB reloads.
    inc     ecx                         ; mov     ecx, IC_CFG
    _RDMSR
    bts     eax, DIS_SPEC_TLB_RLD       ; turn on Disable speculative TLB reloads bit
    _WRMSR
    ; * MSRC001_1022[DIS_HW_PF]=1.
    inc     ecx
    _RDMSR
    bts     eax, DIS_HW_PF              ; turn on Disable hardware prefetches bit
    _WRMSR
    ; * MSRC001_102A[ClLinesToNbDis]=1.
    mov     ecx, BU_CFG2
    _RDMSR
    bts     eax, ClLinesToNbDis         ; turn on DC and IC caches WT/WP-IO, but L2 does not.
    _WRMSR

fam14_enable_stack_hook_exit:
ENDM

;---------------------------------------------------
;
; GET_NODE_ID_CORE_ID Macro - Stackless
;
; Macro returns:
;  SI= NODE ID
;  DI= CORE ID
;---------------------------------------------------
GET_NODE_ID_CORE_ID MACRO

    GET_NODE_ID_CORE_ID_F10
    GET_NODE_ID_CORE_ID_F12
    GET_NODE_ID_CORE_ID_F14

ENDM

GET_NODE_ID_CORE_ID_F10 MACRO

    local   node_core_f10_exit

    cmp     si, -1             ; Has node/core already been discovered?
    jnz     node_core_f10_exit ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 01h            ; Is this family 10h?
    jnz     node_core_f10_exit ; Br if no

    mov     ecx, 1Bh           ; APIC Base MSR
    _RDMSR
    xor     si, si             ; Assume BSC
    xor     di, di
    test    ah, 1              ; Is this the BSC?
    jnz     node_core_f10_exit ; Br if yes
    mov     ecx, 0C0000408h    ; Read the family 10h mailbox
    _RDMSR
    mov     si, dx             ; SI = raw mailbox contents
    shr     ebx, 24            ; BL = Initial APIC ID
    mov     di, bx             ; DI = Initial APIC ID

    AMD_CPUID   80000008h
    shr     ch, 4              ; CH = ApicIdSize
    inc     cl                 ; CL = Number of enabled cores in the socket
    mov     bx, cx

    mov     ecx, NB_CFG
    _RDMSR                     ; EDX bit 54-32=26 is InitApicIdCpuIdLo bit

    mov     cl, bh             ; CL = APIC ID size
    mov     al, 1              ; Convert APIC ID size to an AND mask
    shl     al, cl             ; AL = 2^APIC ID size
    dec     al                 ; AL = mask for relative core number
    xor     ah, ah             ; AX = mask for relative core number
    bt      edx, 54-32         ; InitApicId == 1?
    .if (!carry?)              ; Br if yes
        mov     ch, 8          ; Calculate core number shift count
        sub     ch, cl         ; CH = core shift count
        mov     cl, ch
        shr     di, cl         ; Right justify core number
    .endif
    and     di, ax             ; DI = socket-relative core number

    mov     cx, si             ; CX = raw mailbox value
    shr     cx, 10             ; CL[1:0] = ModuleType
    and     cl, 3              ; Isolate ModuleType
    xor     bh, bh             ; BX = Number of enabled cores in the socket
    shr     bx, cl             ; BX = Number of enabled cores per node
    xor     dx, dx             ; Clear upper word for div
    mov     ax, di             ; AX = socket-relative core number
    div     bx                 ; DX = node-relative core number
    mov     di, dx             ; DI = node-relative core number
    and     si, 000Fh          ; SI = node number
node_core_f10_exit:
ENDM

GET_NODE_ID_CORE_ID_F12 MACRO

    local   node_core_f12_exit

    cmp     si, -1             ; Has node/core already been discovered?
    jnz     node_core_f12_exit ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 03h            ; Is this family 10h?
    jnz     node_core_f12_exit ; Br if no

    xor     si, si             ; Node must be 0
    shr     ebx, 24            ; CPUID_0000_0001_EBX[31:24]: initial local APIC physical ID
    mov     di, bx             ; DI = core number
node_core_f12_exit:
ENDM

GET_NODE_ID_CORE_ID_F14 MACRO

    local   node_core_f14_exit

    cmp     si, -1             ; Has node/core already been discovered?
    jnz     node_core_f14_exit ; Br if yes

    AMD_CPUID   CPUID_MODEL
    shr     eax, 20            ; AL = cpu extended family
    cmp     al, 05h            ; Is this family 14h?
    jnz     node_core_f14_exit ; Br if no

    xor     si, si             ; Node must be 0
    mov     ecx, 1Bh           ; APIC Base MSR
    _RDMSR
    xor     di, di             ; Assume BSC
    test    ah, 1              ; Is this the BSC?
    jnz     node_core_f14_exit ; Br if yes
    inc     di                 ; Set core to 1
node_core_f14_exit:
ENDM


AMD_CPUID MACRO arg0
  IFB <arg0>
    mov   eax, 1
    db    0Fh, 0A2h   ; Execute instruction
    bswap eax
    xchg  al, ah      ; Ext model in al now
    rol   eax, 8      ; Ext model in ah, model in al
    and   ax, 0FFCFh  ; Keep 23:16, 7:6, 3:0
  ELSE
    mov   eax, arg0
    db    0Fh, 0A2h
  ENDIF
ENDM

