;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  Workfile: cpcarmac.inc    $Revision:: 44323   $    $Date:: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
;
; Description: Code to setup and break down cache-as-stack
;
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
    INCLUDE cpcar.inc
    .LIST
    .586P

;======================================================================
; AMD_ENABLE_STACK:  Setup a stack
;
;   In:
;       none
;
;   Out:
;       SS:ESP - Our new private stack location
;       4000:3FFC   - for BSP (16K Stack)
;       4000:7FFC   - for core0 of node1 (16K Stack)
;       4000:BFFC   - for core0 of node2 (16K Stack)
;       4000:FFFC   - for core0 of node3 (16K Stack)
;       5000:3FFC   - for core0 of node4 (16K Stack)
;       5000:7FFC   - for core0 of node5 (16K Stack)
;       5000:BFFC   - for core0 of node6 (16K Stack)
;       5000:FFFC   - for core0 of node7 (16K Stack)
;
;       6000:1FFC   - for core1 node0 (4k stack)
;       6000:2FFC   - for core2 node0 (4k stack)
;       ...
;       9000:8FFC   - for core7 of node7 (4k stack)  ......... max of 64 cores in system
;
;       EAX = AGESA_STATUS
;
;       ECX = Stack size in bytes
;
;   Requirements:
;       * This routine presently is limited to a max of 64 processors
;
;   Preserved:
;       ebx
;   Destroyed:
;       eax, ecx, edx, edi, esi, ds, es
;
;======================================================================
AMD_ENABLE_STACK MACRO

    local   SetupStack
    local   SetupDramMap
    local   get_SS_ESP
    local   r16bmode
    local   p32mode
    local   init_stack

;   Note that SS:ESP will be default stack.  Note that this stack
;   routine will not be used after memory has been initialized.  Because
;   of its limited lifetime, it will not conflict with typical PCI devices.

    mov     esp, ebx                    ; put return address in a safe place

    ; get node id and core id of current executing core
    mov si, -1
    GET_NODE_ID_CORE_ID
    movzx   edi, di

    ; determine if stack is already enabled.
    mov     eax, cr0
    test    eax, 60000000h
    jnz     SetupStack
    mov     ecx, AMD_MTRR_DEFTYPE
    _RDMSR
    test    ah, 0Ch
    jz      SetupStack
    or      edi, 0FFFF0000h             ; indicate stack has already been initialized
    jmp     get_SS_ESP

SetupStack:
    ; Enable routing tables on BSP (just in case the HT init code has not yet enabled them)
    mov     eax, 8000C06Ch
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    in      eax, dx
    btr     eax, 0
    out     dx, eax

    ; Setup temporary DRAM map for CAS on all nodes
    mov     eax, 8000C060h              ; Get NodeCnt from BSP
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    in      al, dx
    shr     ax, 4
    and     al, 7
    mov     cl, al

    mov     ebx, 8000C144h
SetupDramMap:
    mov     eax, ebx                    ; Set 0000_0000 to 00FF_FFFF as DRAM
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    mov     eax, 0
    out     dx, eax

    mov     eax, ebx
    sub     eax, 4
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    mov     eax, 3
    out     dx, eax

    add     bh, 8
    dec     cl
    jns     SetupDramMap

    ; Disable the self modifying code check buffer and Disable hardware prefetches
    mov     ecx, 0C0011022h
    _RDMSR
    bts     eax, DC_DIS_SPEC_TLB_RLD    ; turn on Disable speculative TLB reloads bit
    bts     eax, DIS_CLR_WBTOL2_SMC_HIT ; turn on Disable the self modifying code check buffer bit
    bts     eax, DIS_HW_PF              ; turn on Disable hardware prefetches bit
    _WRMSR
    dec     cx                          ; MSRC001_1021 Instruction Cache Configuration Register (IC_CFG)
    _RDMSR
    bts     eax, IC_DIS_SPEC_TLB_RLD    ; turn on Disable speculative TLB reloads bit
    _WRMSR

    AMD_ENABLE_STACK_FAMILY_HOOK        ; Disable L3 cache to accept clear lines

    ; Init CPU MSRs for our init routines
    mov     ecx, MTRR_SYS_CFG           ; SYS_CFG
    _RDMSR
    and     eax, 0FFE3FFFFh             ; turn off MTRR enable bits
    bts     eax, MtrrFixDramModEn       ; turn on modification enable bit
    _WRMSR

    ; clear all variable and Fixed MTRRs
    mov     ecx, 200h
    xor     eax, eax
    xor     edx, edx
    .while (cl != 10h)                  ; MTRRphysBasen and MTRRphysMaskn
        _WRMSR
        inc     cl
    .endw
    mov     cl, 50h                     ; MTRRfix64K_00000
    _WRMSR
    mov     cl, 58h                     ; MTRRfix16K_80000
    _WRMSR
    mov     cl, 59h                     ; MTRRfix16K_A0000
    _WRMSR
    mov     cl, 68h                     ; MTRRfix4K_C0000 to MTRRfix4K_F8000
    .while (cl != 70h)
        _WRMSR
        inc     cl
    .endw

    ; setup MTTR for stacks
    mov ebx, WB_DRAM_TYPE
    .if (di == 0)    ;core 0
        .if (si > 3)    ; node 0 to 3 located at 40000h, node 4 to 7 located at 50000h
            shl ebx, 8
        .endif
        mov ecx, AMD_MTRR_FIX64k_00000
        _RDMSR
        or edx, ebx
        _WRMSR
    .else    ;core 1 to core 7 start at 60000h
        .if (si < 4)    ; node 0 to 3 using AMD_MTRR_FIX64K_6000 and AMD_MTRR_FIX64K_7000 MTTR
            shl ebx, 16
            .if (si > 1)
                shl ebx, 8
            .endif
            mov ecx, AMD_MTRR_FIX64k_00000
            _RDMSR
            or edx, ebx
            _WRMSR
        .else           ; node 4 to 7 uses AMD_MTRR_FIX16K_80000 and AMD_MTRR_FIX16K_9000 MTTR
            mov ecx, AMD_MTRR_FIX16k_80000
            _RDMSR
            .if (si < 6)   ; node 4 and node 5
                .if (si == 4)     ; node 4
                    .if (di >= 4)
                        shl ebx, 8
                    .endif
                .else             ; node 5
                    shl ebx, 16
                    .if (di >= 4)
                        shl ebx, 8
                    .endif
                .endif
                or eax, ebx
                _WRMSR
            .else          ; node 6 and node 7
                .if (si == 6)     ; node 6
                    .if (di >= 4)
                        shl ebx, 8
                    .endif
                .else             ; node 7
                    shl ebx, 16
                    .if (di >= 4)
                        shl ebx, 8
                    .endif
                .endif
                or edx, ebx
                _WRMSR
            .endif
        .endif
    .endif

    ; Clear IORRs, TOP_MEM and TOM2
    xor     eax, eax
    xor     edx, edx

    mov     ecx, 0C0010016h             ;IORRBase0
    .while (cl != 1Bh)
        _WRMSR
        inc     cl
    .endw
    mov     cl, 1Dh
    _WRMSR

    ; Enable MTRRs
    mov     ecx, 02FFh                  ; MTRRdefType
    mov     ah, 0Ch                     ; MtrrDefTypeEn and MtrrDefTypeFixEn
    _WRMSR

    mov     ecx, MTRR_SYS_CFG           ; SYS_CFG
    _RDMSR
    bts     eax, MtrrFixDramEn          ; MtrrFixDramEn
    btr     eax, MtrrFixDramModEn       ; turn off modification enable bit
    _WRMSR

    ; Enable caching in CR0
    mov     eax, CR0                    ; Enable WT/WB cache
    btr     eax, 31                     ; make sure paging is disabled
    btr     eax, 30                     ; Clear CR0 NW and CD
    btr     eax, 29
    mov     CR0, eax

get_SS_ESP:
    ; allocate space for stacks
    xor cx, cx
    xor edx, edx
    .if (di == 0)    ;core 0
        mov eax, CORE0_STACK_BASE_ADDR
        .while (cx <= si)
            add eax, 4000h
            inc cx
        .endw
        mov edx, eax
        sub eax, 4000h
        and eax, 0F0000h
        sub edx, 4
        and edx, 0FFFFh
        mov bx, CORE0_STACK_SIZE / 4
    .else    ;core 1 to core 7 start at 60000h
        mov eax, CORE1_STACK_BASE_ADDR        ; core 1 stack starts at 60000h
        .while (cx <= si)
            add eax, 8000h       ; 32k for each node
            inc cx
        .endw
        sub eax, 8000h
        mov dx, ax
        and eax, 0F0000h
        xor cx, cx
        .while (cx <= di)
            add edx, 1000h      ; 4k for APs
            inc cx
        .endw
        sub edx, 4
        mov bx, CORE1_STACK_SIZE / 4
    .endif

    ; Allocate stack and set ESP
    mov     ecx, CR0                 ; check for 32-bit protect mode
    test    ecx, 1                   ; PE bit
    jz      r16bmode                 ; PE=0, real mode
    mov     cx, cs                   ; PE=1
    cmp     cx, 0D000h               ; check for CS
    jb      p32mode                  ; if CS < D000, it is a selector instead of a segment
r16bmode:
    shr     eax, 4                   ; ax = ss, ds, es
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    jmp     init_stack
p32mode:
    add     edx, eax                 ; linear address of the stack
init_stack:

    .if ( !(edi & 0FFFF0000h))
        std
        xor ecx, ecx
        mov cx, bx
        mov esi, edx
        rep lods DWORD PTR [esi]
        xor eax, eax
        mov cx, bx
        mov edi, edx
        rep stos DWORD PTR [edi]
        cld
        xor eax, eax  ; eax = 0 : no error
    .else
        mov eax, 40000001h  ; eax = AGESA_WARNING (Stack has already been set up)
    .endif

        mov cx, bx    ; ecx = size in dwords
        shl ecx, 2    ; ecx = size in bytes

    mov     ebx, esp
    mov     esp, edx


ENDM

;======================================================================
; AMD_DISABLE_STACK:  Destroy the stack inside the cache. This routine
;                     should only be executed on the BSP
;
;   In:
;       none
;
;   Out:
;       EAX = AGESA_STATUS
;
;   Preserved:
;       ebx
;   Destroyed:
;       eax, ecx, edx, esp
;======================================================================
AMD_DISABLE_STACK MACRO

    mov     esp, ebx            ; save return address

    ; Turn on modification enable bit
    mov ecx, MTRR_SYS_CFG
    _RDMSR
    bts eax, MtrrFixDramModEn ; Enable
    _WRMSR

    ; Disable MTRRs and turn on modification enable bit
    mov ecx,AMD_MTRR_FIX64k_00000
    mov eax,1E1E1E1Eh
    mov edx,eax
    _WRMSR                          ; 0 - 512K = WB Mem
    mov cl,58h
    _WRMSR                          ; 512K - 640K = WB Mem

    ; Turn off modification enable bit
    mov ecx, MTRR_SYS_CFG
    _RDMSR
    btr eax, MtrrFixDramModEn ; Disable
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

    mov     ebx, esp
    xor     eax, eax

ENDM
