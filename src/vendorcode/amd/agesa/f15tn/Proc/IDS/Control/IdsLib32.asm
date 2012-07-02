;/**
; * @file
; *
; * Ids Assembly library 32bit
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  IDS
; * @e \$Revision: 14305 $   @e \$Date: 2009-05-24 02:20:55 +0800 (Sun, 24 May 2009) $
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

.586p
.model  flat
ASSUME FS:NOTHING
.code
public IdsDelay
IdsDelay  PROC  NEAR C USES EAX EDX
Local targetedx:dword, targeteax:dword
  rdtsc
;set target time
  add eax,1500000000
  adc edx,0
  mov targetedx,edx
  mov targeteax,eax

  rdtsc
;set "Si!=0" skip below loop
  .while(1)
    .if(si != 0)
      jmp delay_exit
    .endif
    .if(edx > targetedx)
      jmp delay_exit
    .elseif (edx == targetedx)
      .if(eax > targeteax)
        jmp delay_exit
      .endif
    .endif
    rdtsc
  .endw
delay_exit:
  ret
IdsDelay    ENDP
;/*++
;
;Routine Description:
;
;  IdsErrorStop -- Function for Assert
;
;Arguments:
;  Filecode
;
;Returns:
;
;  None
;
;--*/
public IdsErrorStop
IdsErrorStop PROC NEAR  C filecode:dword
local tmpebx:dword,tmpedx:dword
  pushad

  mov     si,0                      ; Si is used as control flag, "Si!=0" skip postcode loop
; send debug port 1st, then fire SimNow breakpoint
  mov     ax,  0deadh
  out     0e0h, ax
  mov     eax, filecode
  out     84h, eax
  mov     eax, 0BACCD00Bh         ; Backdoor in SimNow
  mov     ebx, 2                  ; Select breakpoint feature
  cpuid

  mov   ebx,0dead0000h
  mov   edx,filecode
  ror   edx,16
  mov   bx,dx
  mov   dx,0
;ebx:edx = deadxxxxyyyy0000 xxxx is the filecode yyyy is the line num
  mov   tmpebx,ebx
  mov   tmpedx,edx

  xor   eax,eax
  mov   cl,6

  .while((cl != 0) && (si == 0))
    .if(cl <= 2)
      shld  eax,edx,8
      shl   edx,8
    .else
      shld  eax,ebx,8
      shl   ebx,8
    .endif

    out   80h,eax
    call IdsDelay
    dec cl
    .if(cl == 0)
      mov   cl,6
      mov   ebx,tmpebx
      mov   edx,tmpedx
    .endif
  .endw

  popad
  xor eax,eax
  ret
IdsErrorStop  endp

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Stop CPU
; *
; *
; *
; */
IDS_STOP_HERE MACRO
@@:
  jmp short @b
ENDM

;======================================================================
; IdsExceptionHandler:  Simply performs a jmp $ and IRET.
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
PUBLIC _IdsExceptionHandler
PUBLIC _SizeIdtDescriptor
PUBLIC _SizeTotalIdtDescriptors

; Size of each exception MUST be the same
Exception00:
  push eax
  mov  al, 00h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception01:
  push eax
  mov  al, 01h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception02:
  push eax
  mov  al, 02h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception03:
  push eax
  mov  al, 03h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception04:
  push eax
  mov  al, 04h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception05:
  push eax
  mov  al, 05h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception06:
  push eax
  mov  al, 06h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception07:
  push eax
  mov  al, 07h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception08:
  push eax
  mov  al, 08h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception09:
  push eax
  mov  al, 09h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception10:
  push eax
  mov  al, 10h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception11:
  push eax
  mov  al, 11h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception12:
  push eax
  mov  al, 12h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception13:
  push eax
  mov  al, 13h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception14:
  push eax
  mov  al, 14h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception15:
  push eax
  mov  al, 15h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception16:
  push eax
  mov  al, 16h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception17:
  push eax
  mov  al, 17h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception18:
  push eax
  mov  al, 18h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception19:
  push eax
  mov  al, 19h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception20:
  push eax
  mov  al, 20h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception21:
  push eax
  mov  al, 21h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception22:
  push eax
  mov  al, 22h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception23:
  push eax
  mov  al, 23h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception24:
  push eax
  mov  al, 24h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception25:
  push eax
  mov  al, 25h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception26:
  push eax
  mov  al, 26
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception27:
  push eax
  mov  al, 27h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception28:
  push eax
  mov  al, 28h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception29:
  push eax
  mov  al, 29h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception30:
  push eax
  mov  al, 30h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception31:
  push eax
  mov  al, 31h
  jmp  near ptr CommonHandler
CommonHandler:
  out 80h, al
  pop eax
  IDS_STOP_HERE
  iretd

_IdsExceptionHandler dq offset Exception00
_SizeIdtDescriptor dd (offset Exception01 - offset Exception00)
_SizeTotalIdtDescriptors dd (offset CommonHandler - offset Exception00)

END
