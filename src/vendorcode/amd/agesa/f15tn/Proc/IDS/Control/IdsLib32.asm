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
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
