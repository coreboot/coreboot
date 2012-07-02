;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including ap halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU
; * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
; */
;*****************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
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

  bt esi, FLAG_IS_PRIMARY
  .if (carry?)
    ; restore variable MTRR6 and MTRR7 to default states
    mov ecx,  AMD_MTRR_VARIABLE_MASK7    ; clear MTRRPhysBase6 MTRRPhysMask6
    xor eax,  eax                        ;  and MTRRPhysBase7 MTRRPhysMask7
    xor edx,  edx
    .while (cx >= AMD_MTRR_VARIABLE_BASE6)
      _WRMSR
      dec cx
    .endw
  .endif

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
;======================================================================
FarCallGetCs PROC FAR PRIVATE

  mov  ax, ss:[esp + 4]
  retf

FarCallGetCs ENDP

;======================================================================
; SetIdtr:
;
;   In:
;       @param[in]   IdtPtr Points to IDT table
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
;======================================================================
PUBLIC  SetIdtr
SetIdtr PROC NEAR C USES EBX, IdtPtr:PTR
  mov   ebx, IdtPtr
  lidt  fword ptr ss:[ebx]
  ret
SetIdtr ENDP

;======================================================================
; GetIdtr:
;
;   In:
;       @param[in]   IdtPtr Points to IDT table
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
;======================================================================
PUBLIC  GetIdtr
GetIdtr PROC NEAR C USES EBX, IdtPtr:PTR
  mov   ebx, IdtPtr
  sidt  fword ptr ss:[ebx]
  ret
GetIdtr ENDP

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
