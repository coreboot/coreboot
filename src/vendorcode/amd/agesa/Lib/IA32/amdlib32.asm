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
; * @e \$Revision: 17071 $   @e \$Date: 2009-07-30 10:13:11 -0700 (Thu, 30 Jul 2009) $
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

.586p
.xmm
.model flat,C
ASSUME FS:NOTHING
.code

;---------------------------------------------------------------------------
;
; _mm_clflush_fs - execute clflush instruction for address fs:address32
;                  this lets clflush operate beyond 4GB in 32-bit mode
;
;                  void _mm_clflush_fs (void *address32);
;

_mm_clflush_fs proc public






    mov     eax, [esp+8]













































    clflush fs:[eax]



    ret
_mm_clflush_fs  ENDP


;---------------------------------------------------------------------------
;
; _mm_stream_si128_fs - execute movntdq instruction for address fs:address32
;                       this lets movntdq operate beyond 4GB in 32-bit mode
;
; void _mm_stream_si128_fs (void *dest, void *data)
;
_mm_stream_si128_fs proc public
    push        esi
    mov         esi, [esp+12]
    movdqa      xmm0, [esi]
    mov         esi, [esp+8]
    movntdq     fs:[esi], xmm0







    pop         esi
    ret




_mm_stream_si128_fs  ENDP


    

;---------------------------------------------------------------------------


END
