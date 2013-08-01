;/**
; * @file
; *
; * Ids Assembly library 64bit
; *
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  IDS
; * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
; */
;*****************************************************************************
;
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

.code
IdsDelay  PROC
  push    rax
  push    rdx
  push    rbx
  xor     rax,rax
  xor     rdx,rdx
  rdtsc
;set target time
  add     eax,1500000000
  adc     edx,0
  shl     rdx,32
  add     rdx,rax
  mov     rbx,rdx
;rbx store the target
;set "Si!=0" skip below loop
__loop:
  cmp     si,0
  jnz     __loopexit
  rdtsc
  shl     rdx,32
  add     rdx,rax
  cmp     rdx,rbx
  jae     __loopexit
  jmp     __loop
__loopexit:
  pop     rbx
  pop     rdx
  pop     rax
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
IdsErrorStop PROC
;As x64 calling convention RCX is used as input parameters
  push    rcx
  push    rbx
  push    si
  push    dx
  push    rbx

  mov     si,0                      ; Si is used as control flag, "Si!=0" skip postcode loop
; send debug port 1st, then fire SimNow breakpoint
  mov     ax,  0deadh
  out     0e0h, ax
  mov     eax, ecx
  out     84h, eax
  mov     eax, 0BACCD00Bh         ; Backdoor in SimNow
  mov     ebx, 2                  ; Select breakpoint feature
  cpuid

  mov     rax,0dead00000000h
  or      rcx,rax
;rcx= 0dead__FILECODE
  shl     rcx,16
;rcx= 0dead__FILECODE__0000
  mov     rbx,rcx

  xor     rax,rax
  mov     dl,6

IdsErrorStopLoop:
  cmp     dl,0
  jz      IdsErrorStopExit
  cmp     si,0
  jnz     IdsErrorStopExit

  shld    rax,rcx,8
  shl     rcx,8
  out     80h,eax
  call    IdsDelay

  dec     dl
  cmp     dl,0
  jnz     _nextloop
  mov     dl,6
  mov     rcx,rbx
_nextloop:
  jmp     IdsErrorStopLoop
IdsErrorStopExit:
  pop     rbx
  pop     dx
  pop     si
  pop     rbx
  pop     rcx
  xor     rax,rax
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
PUBLIC IdsExceptionHandler
PUBLIC SizeIdtDescriptor
PUBLIC SizeTotalIdtDescriptors

; Size of each exception MUST be the same
Exception00:
  push rax
  mov  al, 00h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception01:
  push rax
  mov  al, 01h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception02:
  push rax
  mov  al, 02h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception03:
  push rax
  mov  al, 03h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception04:
  push rax
  mov  al, 04h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception05:
  push rax
  mov  al, 05h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception06:
  push rax
  mov  al, 06h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception07:
  push rax
  mov  al, 07h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception08:
  push rax
  mov  al, 08h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception09:
  push rax
  mov  al, 09h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception10:
  push rax
  mov  al, 10h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception11:
  push rax
  mov  al, 11h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception12:
  push rax
  mov  al, 12h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception13:
  push rax
  mov  al, 13h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception14:
  push rax
  mov  al, 14h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception15:
  push rax
  mov  al, 15h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception16:
  push rax
  mov  al, 16h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception17:
  push rax
  mov  al, 17h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception18:
  push rax
  mov  al, 18h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception19:
  push rax
  mov  al, 19h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception20:
  push rax
  mov  al, 20h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception21:
  push rax
  mov  al, 21h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception22:
  push rax
  mov  al, 22h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception23:
  push rax
  mov  al, 23h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception24:
  push rax
  mov  al, 24h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception25:
  push rax
  mov  al, 25h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception26:
  push rax
  mov  al, 26
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception27:
  push rax
  mov  al, 27h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception28:
  push rax
  mov  al, 28h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception29:
  push rax
  mov  al, 29h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception30:
  push rax
  mov  al, 30h
  jmp  near ptr CommonHandler
; Size of each exception MUST be the same
Exception31:
  push rax
  mov  al, 31h
  jmp  near ptr CommonHandler
CommonHandler:
  out 80h, al
  pop rax
  IDS_STOP_HERE
  iretq

IdsExceptionHandler dq offset Exception00
SizeIdtDescriptor dd (offset Exception01 - offset Exception00)
SizeTotalIdtDescriptors dd (offset CommonHandler - offset Exception00)

END

