;/**
; * @file
; *
; * Agesa library 32bit
; *
; * Contains AMD AGESA Library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Lib
; * @e \$Revision: 9201 $   @e \$Date: 2008-10-31 03:36:20 -0500 (Fri, 31 Oct 2008) $
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

.586p
.model  flat
ASSUME FS:NOTHING
.code
;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the right.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_aullshr  PROC NEAR C PUBLIC
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    eax, edx
                xor    edx, edx
            .endif
            shrd   eax, edx, cl
            shr    edx, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_aullshr ENDP

;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the left.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_allshl  PROC NEAR C PUBLIC USES CX
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    edx, eax
                xor    eax, eax
            .endif
            shld   edx, eax, cl
            shl    eax, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_allshl ENDP

END
