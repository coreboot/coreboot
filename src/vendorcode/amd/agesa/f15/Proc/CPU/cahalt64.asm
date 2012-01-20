;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including ap halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU
; * @e \$Revision: 10071 $   @e \$Date: 2008-12-16 18:03:04 -0600 (Tue, 16 Dec 2008) $
; */
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

 text  SEGMENT


;======================================================================
; ExecuteFinalHltInstruction:  Performs a hlt instruction.
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
ExecuteFinalHltInstruction PROC PUBLIC
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
ExecuteHltInstruction PROC PUBLIC
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
NmiHandler PROC PUBLIC
    iretq
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
GetCsSelector PROC PUBLIC
    ; This stub function is here to avoid compilation errors.
    ; At this time, there is no need to provide a 64 bit function.
    ret
GetCsSelector ENDP

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
SetIdtr PROC PUBLIC
    ; This stub function is here to avoid compilation errors.
    ; At this time, there is no need to provide a 64 bit function.
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
GetIdtr PROC PUBLIC
    ; This stub function is here to avoid compilation errors.
    ; At this time, there is no need to provide a 64 bit function.
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
ExecuteWbinvdInstruction PROC PUBLIC
    wbinvd                        ; Write back the cache tag RAMs
    ret
ExecuteWbinvdInstruction ENDP

END
