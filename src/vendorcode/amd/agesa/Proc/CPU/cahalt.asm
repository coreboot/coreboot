;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including ap halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU
; * @e \$Revision: 35270 $   @e \$Date: 2010-07-20 00:02:41 +0800 (Tue, 20 Jul 2010) $
; */
;*****************************************************************************
; 
;  Copyright (c) 2011, Advanced Micro Devices, Inc.
;  All rights reserved.
;  
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
;      * Redistributions of source code must retain the above copyright
;        notice, this list of conditions and the following disclaimer.
;      * Redistributions in binary form must reproduce the above copyright
;        notice, this list of conditions and the following disclaimer in the
;        documentation and/or other materials provided with the distribution.
;      * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;        its contributors may be used to endorse or promote products derived 
;        from this software without specific prior written permission.
;  
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;  DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
;  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;  
;*****************************************************************************

    .XLIST
    INCLUDE agesa.inc
    INCLUDE cpcarmac.inc
    .LIST

    .586P

;===============================================
;===============================================
;==
;== M E M O R Y   A B S E N T   S E G M E N T
;==
;===============================================
;===============================================
    .MODEL flat
    .CODE
;======================================================================
; ExecuteFinalHltInstruction:  Disables the stack and performs
;                              a hlt instruction on an AP.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       eax, ebx, ecx, edx, esp
;
;======================================================================
PUBLIC ExecuteFinalHltInstruction
ExecuteFinalHltInstruction PROC NEAR C USES ESI EDI HaltFlags:DWORD, ApMtrrSettingList:PTR, StandardHeader:PTR

  mov esi,  StandardHeader        ; The code must reference all parameters to avoid a build warning
  mov esi,  HaltFlags
  mov edi,  ApMtrrSettingList
  ; Do these special steps in case if the core is part of a compute unit
  ; Note: The following bits are family specific flags, that gets set during build time,
  ;          and indicates things like "family cache control methodology", etc.
  ; esi bit0 = 0  -> not a Primary core
  ; esi bit0 = 1  -> Primary core
  ; esi bit1 = 0  -> Cache disable
  ; esi bit1 = 1  -> Cache enable
  .if (esi & 2h)
    ; Set CombineCr0Cd bit
    mov ecx,  CU_CFG3
    _RDMSR
    bts edx,  (COMBINE_CR0_CD - 32)
    _WRMSR
    ; Clear the CR0.CD bit
    mov eax,  CR0                 ; Make sure cache is enabled for all APs
    btr eax,  CR0_CD
    btr eax,  CR0_NW
    mov CR0,  eax                 ; Write back to CR0
  .else
    mov eax,  CR0                 ; Make sure cache is disabled for all APs
    bts eax,  CR0_CD              ; Disable cache
    bts eax,  CR0_NW
    mov CR0,  eax                 ; Write back to CR0
  .endif

  .if (esi & 1h)
    ; This core is a primary core and needs to do all the MTRRs, including shared MTRRs.
    mov esi,  edi                 ; Get ApMtrrSettingList

    ; Configure the MTRRs on the AP so
    ; when it runs remote code it will execute
    ; out of RAM instead of ROM.

    ; Disable MTRRs and turn on modification enable bit
    mov ecx,  MTRR_SYS_CFG
    _RDMSR
    btr eax,  MTRR_VAR_DRAM_EN     ; Disable
    bts eax,  MTRR_FIX_DRAM_MOD_EN  ;  Enable
    btr eax,  MTRR_FIX_DRAM_EN     ; Disable
    bts eax,  SYS_UC_LOCK_EN
    _WRMSR

    ; Setup default values for Fixed-Sized MTRRs
    ; Set 7FFFh-00000h as WB
    mov ecx,  AMD_AP_MTRR_FIX64k_00000
    mov eax,  1E1E1E1Eh
    mov edx,  eax
    _WRMSR

    ; Set 9FFFFh-80000h also as WB
    mov ecx,  AMD_AP_MTRR_FIX16k_80000
    _WRMSR

    ; Set BFFFFh-A0000h as Uncacheable Memory-mapped IO
    mov ecx,  AMD_AP_MTRR_FIX16k_A0000
    xor eax,  eax
    xor edx,  edx
    _WRMSR

    ; Set DFFFFh-C0000h as Uncacheable Memory-mapped IO
    xor eax,  eax
    xor edx,  edx
    mov ecx,  AMD_AP_MTRR_FIX4k_C0000

CDLoop:
    _WRMSR
    inc ecx
    cmp ecx,  AMD_AP_MTRR_FIX4k_D8000
    jbe CDLoop

    ; Set FFFFFh-E0000h as Uncacheable Memory
    mov eax,  18181818h
    mov edx,  eax

    mov ecx,  AMD_AP_MTRR_FIX4k_E0000

EFLoop:
    _WRMSR
    inc ecx
    cmp ecx,  AMD_AP_MTRR_FIX4k_F8000
    jbe EFLoop

    ; If IBV provided settings for Fixed-Sized MTRRs,
    ; overwrite the default settings.
    .if ((esi != 0) && (esi != 0FFFFFFFFh))
      mov ecx, (AP_MTRR_SETTINGS ptr [esi]).MsrAddr
      ; While we are not at the end of the list
      .while (ecx != CPU_LIST_TERMINAL)
        ; Ensure that the MSR address is valid for Fixed-Sized MTRRs
        .if ( ((ecx >= AMD_AP_MTRR_FIX4k_C0000) && (ecx <= AMD_AP_MTRR_FIX4k_F8000)) || \
               (ecx == AMD_AP_MTRR_FIX64k_00000) || (ecx == AMD_AP_MTRR_FIX16k_80000 ) || (ecx == AMD_AP_MTRR_FIX16k_A0000))
          mov eax,  dword ptr (AP_MTRR_SETTINGS ptr [esi]).MsrData
          mov edx,  dword ptr (AP_MTRR_SETTINGS ptr [esi+4]).MsrData
          _WRMSR
        .endif
        add esi,  sizeof (AP_MTRR_SETTINGS)
        mov ecx,  (AP_MTRR_SETTINGS ptr [esi]).MsrAddr
      .endw
    .endif

    ; restore variable MTTR6 and MTTR7 to default states
    mov ecx,  AMD_MTRR_VARIABLE_BASE6     ; clear MTRRPhysBase6 MTRRPhysMask6
    xor eax,  eax                         ;  and MTRRPhysBase7 MTRRPhysMask7
    xor edx,  edx
    .while (cl < 010h)
      _WRMSR
      inc cl
    .endw

    ; Enable fixed-range and variable-range MTRRs
    mov ecx,  AMD_MTRR_DEFTYPE
    _RDMSR
    bts eax, MTRR_DEF_TYPE_EN      ; MtrrDefTypeEn
    bts eax, MTRR_DEF_TYPE_FIX_EN  ; MtrrDefTypeFixEn
    _WRMSR

    ; Enable Top-of-Memory setting
    ; Enable use of RdMem/WrMem bits attributes
    mov ecx,  MTRR_SYS_CFG
    _RDMSR
    bts eax,  MTRR_VAR_DRAM_EN     ; Enable
    btr eax,  MTRR_FIX_DRAM_MOD_EN ; Disable
    bts eax,  MTRR_FIX_DRAM_EN     ;  Enable
    _WRMSR

    mov esi, (1 SHL FLAG_IS_PRIMARY)
  .else                                 ; end if primary core
    xor esi, esi
  .endif
  ; Make sure not to touch any Shared MSR from this point on

  AMD_DISABLE_STACK_FAMILY_HOOK

  xor   eax,  eax

@@:
  cli
  hlt
  jmp @B  ;ExecuteHltInstruction
  ret
ExecuteFinalHltInstruction ENDP

;======================================================================
; ExecuteHltInstruction:  Performs a hlt instruction.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       eax, ebx, ecx, edx, esp
;
;======================================================================
PUBLIC  ExecuteHltInstruction
ExecuteHltInstruction PROC NEAR C
  cli
  hlt
  ret
ExecuteHltInstruction ENDP

;======================================================================
; NmiHandler:  Simply performs an IRET.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC  NmiHandler
NmiHandler PROC NEAR C
  iretd
NmiHandler ENDP

;======================================================================
; GetCsSelector:  Returns the current protected mode CS selector.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC  GetCsSelector
GetCsSelector PROC NEAR C, CsSelector:PTR
  push ax
  push ebx

  call FarCallGetCs
  mov  ebx, CsSelector
  mov  [ebx], ax
  pop  ebx
  pop  ax
  ret
GetCsSelector ENDP

;======================================================================
; FarCallGetCs:
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
; WARNING: This routine has a mirror routine in the PREMEM segment.
;   These two routines MUST be sync'd for content.
;======================================================================
FarCallGetCs PROC FAR PRIVATE

  mov  ax, ss:[esp + 4]
  retf

FarCallGetCs ENDP

;======================================================================
; SetIdtr:
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
; WARNING: This routine has a mirror routine in the PREMEM segment.
;   These two routines MUST be sync'd for content.
;======================================================================
PUBLIC  SetIdtr
SetIdtr PROC NEAR C USES EBX, IdtPtr:PTR
  mov   ebx, IdtPtr
  lidt  fword ptr ss:[ebx]
  ret
SetIdtr ENDP

;======================================================================
; ExecuteWbinvdInstruction:  Performs a wbinvd instruction.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC  ExecuteWbinvdInstruction
ExecuteWbinvdInstruction PROC NEAR C
  wbinvd                        ; Write back the cache tag RAMs
  ret
ExecuteWbinvdInstruction ENDP

END
