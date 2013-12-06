;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including ap halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU
; * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
; */
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
ExecuteFinalHltInstruction PROC NEAR C PUBLIC

  pop esi   ; StdHeader
  pop esi   ; pointer to ApMtrrSettingsList, set through build configuration

  mov     eax, CR0                 ; Make sure cache is disabled for all APs
  or      eax, CR0_CD OR CR0_NW    ; Disable cache
  mov     cr0, eax                 ; Write back to CR0

  ; Configure the MTRRs on the AP so
  ; when it runs remote code it will execute
  ; out of RAM instead of ROM.

  ; Disable MTRRs and turn on modification enable bit
  mov ecx, MTRR_SYS_CFG
  _RDMSR
  btr eax, MtrrVarDramEn    ; Disable
  bts eax, MtrrFixDramModEn ; Enable
  btr eax, MtrrFixDramEn    ; Disable
  bts eax, SysUcLockEn
  _WRMSR

  ; Setup default values for Fixed-Sized MTRRs
  ; Set 7FFFh-00000h as WB
  mov ecx, AMD_AP_MTRR_FIX64k_00000
  mov eax, 1E1E1E1Eh
  mov edx, eax
  _WRMSR

  ; Set 9FFFFh-80000h also as WB
  mov ecx, AMD_AP_MTRR_FIX16k_80000
  _WRMSR

  ; Set BFFFFh-A0000h as Uncacheable Memory-mapped IO
  mov ecx, AMD_AP_MTRR_FIX16k_A0000
  xor eax, eax
  xor edx, edx
  _WRMSR

  ; Set DFFFFh-C0000h as Uncacheable Memory-mapped IO
  xor eax, eax
  xor edx, edx
  mov ecx, AMD_AP_MTRR_FIX4k_C0000

CDLoop:
  _WRMSR
  inc ecx
  cmp ecx, AMD_AP_MTRR_FIX4k_D8000
  jbe CDLoop

  ; Set FFFFFh-E0000h as Uncacheable Memory
  mov eax, 18181818h
  mov edx, eax

  mov ecx, AMD_AP_MTRR_FIX4k_E0000

EFLoop:
  _WRMSR
  inc ecx
  cmp ecx, AMD_AP_MTRR_FIX4k_F8000
  jbe EFLoop

  ; If IBV provided settings for Fixed-Sized MTRRs,
  ; overwrite the default settings.
  .if (esi != 0)
    mov ecx, (AP_MTRR_SETTINGS ptr [esi]).MsrAddr
    ; While we are not at the end of the list
    .while (ecx != CPU_LIST_TERMINAL)
      ; Ensure that the MSR address is valid for Fixed-Sized MTRRs
      .if ( ((ecx >= AMD_AP_MTRR_FIX4k_C0000) && (ecx <= AMD_AP_MTRR_FIX4k_F8000)) || \
             (ecx == AMD_AP_MTRR_FIX64k_00000) || (ecx == AMD_AP_MTRR_FIX16k_80000 ) || (ecx == AMD_AP_MTRR_FIX16k_A0000))
        mov eax, dword ptr (AP_MTRR_SETTINGS ptr [esi]).MsrData
        mov edx, dword ptr (AP_MTRR_SETTINGS ptr [esi+4]).MsrData
        _WRMSR
      .endif
      add esi, sizeof (AP_MTRR_SETTINGS)
      mov ecx, (AP_MTRR_SETTINGS ptr [esi]).MsrAddr
    .endw
  .endif

  ; restore variable MTTR6 and MTTR7 to default states
  mov ecx, AMD_MTRR_VARIABLE_BASE6    ; clear MTRRPhysBase6 MTRRPhysMask6
  xor eax, eax                        ; and MTRRPhysBase7 MTRRPhysMask7
  xor edx, edx
  .while (cl < 010h)
    _WRMSR
    inc cl
  .endw

  ; Enable fixed-range and variable-range MTRRs
  mov ecx, AMD_MTRR_DEFTYPE
  _RDMSR
  or  ax, 0C00h       ; Set Fixed-Range Enable (FE) and MTRR Enable (E) bits
  _WRMSR

  ; Enable Top-of-Memory setting
  ; Enable use of RdMem/WrMem bits attributes
  mov ecx, MTRR_SYS_CFG
  _RDMSR
  bts eax, MtrrVarDramEn    ; Enable
  btr eax, MtrrFixDramModEn ; Disable
  bts eax, MtrrFixDramEn    ; Enable
  _WRMSR

  ; Enable the self modifying code check buffer and Enable hardware prefetches
  mov     ecx, 0C0011022h
  _RDMSR
  btr     eax, DC_DIS_SPEC_TLB_RLD    ; Disable speculative TLB reloads bit
  btr     eax, DIS_CLR_WBTOL2_SMC_HIT ; Disable the self modifying code check buffer bit
  btr     eax, DIS_HW_PF              ; Disable hardware prefetches bit
  _WRMSR

  dec     cx                          ; MSRC001_1021 Instruction Cache Configuration Register (IC_CFG)
  _RDMSR
  btr     eax, IC_DIS_SPEC_TLB_RLD    ; turn on Disable speculative TLB reloads bit
  _WRMSR

  AMD_DISABLE_STACK_FAMILY_HOOK   ; Re-Enable L3 cache to accept clear lines

  xor     eax, eax

@@:
  cli
  hlt
  jmp @B ;ExecuteHltInstruction
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
ExecuteHltInstruction PROC NEAR C PUBLIC
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
NmiHandler PROC NEAR C PUBLIC
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
GetCsSelector PROC NEAR C PUBLIC, CsSelector:PTR
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
SetIdtr PROC NEAR C PUBLIC USES EBX, IdtPtr:PTR
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
ExecuteWbinvdInstruction PROC NEAR C PUBLIC
  wbinvd                        ; Write back the cache tag RAMs
  ret
ExecuteWbinvdInstruction ENDP

END
