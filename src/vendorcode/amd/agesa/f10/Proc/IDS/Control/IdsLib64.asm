;/**
; * @file
; *
; * Ids Assembly library 64bit
; *
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  IDS
; * @e \$Revision: 14126 $   @e \$Date: 2009-05-21 23:02:32 +0800 (Thu, 21 May 2009) $
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
END

