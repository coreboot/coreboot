;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  Workfile: cpcarmac.inc    $Revision:: 44324   $    $Date:: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
;*****************************************************************************

    .XLIST
    INCLUDE cpcar.inc
    .LIST
    .586P
    .mmx

;======================================================================
; AMD_ENABLE_STACK:  Setup a stack
;
;   In:
;       EBX  = Return address (preserved)
;
;   Out:
;       SS:ESP - Our new private stack location
;
;       EAX = AGESA_STATUS
;       EDX = Return status code if EAX contains a return code of higher
;             severity than AGESA_SUCCESS
;       ECX = Stack size in bytes
;
;   Requirements:
;       * This routine presently is limited to a max of 64 processor cores
;   Preserved:
;       ebx ebp
;   Destroyed:
;       eax, ecx, edx, edi, esi, ds, es, ss, esp
;       mmx0, mmx1
;
;   Description:
; Fixed MTRR address allocation to cores:
; The BSP gets 64K of stack, Core0 of each node gets 16K of stack, all other cores get 4K.
; There is a max of 1 BSP, 7 core0s and 56 other cores.
; Although each core has it's own cache storage, they share the address space. Each core must
; be assigned a private and unique address space for its stack. To support legacy systems,
; the stack needs to be within the legacy address space (1st 1Meg). Room must also be reserved
; for the other legacy elements (Interrupt vectors, BIOS ROM, video buffer, etc.)
;
; 80000h                                        40000h                                      00000h
;     +----------+----------+----------+----------+----------+----------+----------+----------+
; 64K |          |          |          |          |          |          |          |          |  64K  ea
;  ea +----------+----------+----------+----------+----------+----------+----------+----------+
;     |                             MTRR 0000_0250 MTRRfix64K_00000                           |
;     +----------+----------+----------+----------+----------+----------+----------+----------+
;     |  7 ,  6  |  5 ,  4  |  3 ,  2  |  1 ,  0  |     0    |          |          |          | <-node
;     |7..1,7..1 |7..1,7..1 |7..1,7..1 |7..1,7..1 |     0    |          |          |          | <-core
;     +----------+----------+----------+----------+----------+----------+----------+----------+
;
; C0000h                       B0000h                      A0000h                      90000h                      80000h
;     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
;16K  |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |
; ea  +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
;     |              MTRR 0259 MTRRfix16K_A0000               |             MTRR 0258 MTRRfix16K_80000                |
;     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
;     | > Dis|play B|uffer |   <  |      |      |      |      |   7  |  6   |  5   |  4   |  3   |  2   |  1   |      | <-node
;     | >   T| e  m |p o r |a r y |  B u |f f e |r   A |r e a<|   0  |  0   |  0   |  0   |  0   |  0   |  0   |      | <-core
;     +------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+------+
;
; E0000h                                            D0000h                                         C0000h
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
; 4K  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  4K  ea
;  ea +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;     |  026B MTRRfix4K_D8000 | 026A MTRRfix4K_D0000  | 0269 MTRRfix4K_C8000  | 0268 MTRRfix4K_C0000  |
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | >| V| I| D| E| O|  |B |I |O |S |  |A |r |e |a<|
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;
; 100000h                                           F0000h                                          E0000h
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  4K  ea
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;     |  026F MTRRfix4K_F8000 | 026E MTRRfix4K_F0000  | 026D MTRRfix4K_E8000  | 026C MTRRfix4K_E0000  |
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;     | >|MA|IN| B|IO|S |RA|NG|E |  |  |  |  |  |  |< | >|EX|TE|ND|ED| B|IO|S |ZO|NE|  |  |  |  |  |< |
;     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;======================================================================
AMD_ENABLE_STACK MACRO
  local AmdEnableStackExit

;   Note that SS:ESP will be default stack.  Note that this stack
;   routine will not be used after memory has been initialized.  Because
;   of its limited lifetime, it will not conflict with typical PCI devices.
    movd    mm0, ebx                    ; Put return address in a safe place
    movd    mm1, ebp                    ; Save some other user registers

    ; get node id and core id of current executing core
    GET_NODE_ID_CORE_ID                 ; Sets ESI[15,8]= Node#; ESI[7,0]= core# (relative to node)
    ; Note: ESI[31:24] are used for flags:  Unrecognized Family,  Is_Primary core,  Stack already established

    ; If we detected an unknown processor family or core combination, return AGESA_FATAL.
    .if (esi & (1 SHL FLAG_UNKNOWN_FAMILY))
      mov edx, CPU_EVENT_UNKNOWN_PROCESSOR_FAMILY
      mov eax, AGESA_FATAL
      jmp AmdEnableStackExit
    .elseif (esi & (1 SHL FLAG_CORE_NOT_IDENTIFIED))
      mov edx, CPU_EVENT_CORE_NOT_IDENTIFIED
      mov eax, AGESA_FATAL
      jmp AmdEnableStackExit
    .endif

    ; determine if stack is already enabled. We are using the DefType MSR for this determination.
    ; It is =0 after reset; CAR setup sets it to enable the MTRRs
    mov     eax, cr0                    ; Is cache enabled? (CD or NW bit set)
    CR0_MASK    TEXTEQU %((1 SHL CR0_CD) OR (1 SHL CR0_NW))
    .if (!(eax & CR0_MASK))
        mov     ecx, AMD_MTRR_DEFTYPE   ; MSR:0000_02FF
        _RDMSR                          ; Are either of the default types enabled? (MTRR_DEF_TYPE_EN + MTRR_DEF_TYPE_FIX_EN)
        MSR_MASK    TEXTEQU %((1 SHL MTRR_DEF_TYPE_EN)+(1 SHL MTRR_DEF_TYPE_FIX_EN))
        .if (eax & MSR_MASK)
            bts     esi, FLAG_STACK_REENTRY     ; indicate stack has already been initialized
        .endif
    .endif

    ; Set node to map the first 16MB to node 0; 0000_0000 to 00FF_FFFF as DRAM
    mov     ebx, esi                    ; Get my Node/Core info
    xor     bl, bl
    shl     bh, 3                       ; Isolate my node#, match alignment for PCI Dev#
    mov     eax, 8000C144h              ; D18F1x44:DRAM Base/Limit; N is Base, N+4 is Limit
    add     ah, bh
    mov     ebx, eax                    ; Save PCI address for Base/Limit pair

    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    xor     eax, eax                    ; Least Significant bit is AD24 so 0 sets mask of 00FF_FFFF (16MB)
    out     dx, eax                     ; DRAM Limit = node0, no interleave

    mov     eax, ebx
    sub     eax, 4                      ; Now point to the Base register
    mov     dx, 0CF8h
    out     dx, eax
    add     dx, 4
    mov     eax, 00000003h              ; Set the read and write enable bits
    out     dx, eax                     ; DRAM Base = 0x0000, R/W

    AMD_ENABLE_STACK_FAMILY_HOOK

    ; Init CPU MSRs for our init routines
    mov     ecx, MTRR_SYS_CFG           ; SYS_CFG
    _RDMSR
    bts     eax, MTRR_FIX_DRAM_MOD_EN   ; Turn on modification enable bit
    _WRMSR

    mov     eax, esi
    bt      eax, FLAG_STACK_REENTRY     ; Is this a 2nd entry?
    .if (!carry?)                       ;   On a re-entry, do not clear MTRRs or reset TOM; just reset the stack SS:ESP
        bt      eax, FLAG_IS_PRIMARY    ;   Is this core the primary in a compute unit?
        .if (carry?)                    ;     Families using shared groups do not need to clear the MTRRs since that is done at power-on reset
            ;  Note: Relying on MSRs to be cleared to 0's at reset for families w/shared cores
            ; Clear all variable and Fixed MTRRs for non-shared cores
            mov     ecx, AMD_MTRR_VARIABLE_BASE0
            xor     eax, eax
            xor     edx, edx
            .while (cl != 10h)                  ; Variable MTRRphysBase[n] and MTRRphysMask[n]
                _WRMSR
                inc     cl
            .endw
            mov     cx, AMD_MTRR_FIX64k_00000   ; MSR:0000_0250
            _WRMSR
            mov     cx, AMD_MTRR_FIX16k_80000   ; MSR:0000_0258
            _WRMSR
            mov     cx, AMD_MTRR_FIX16k_A0000   ; MSR:0000_0259
            _WRMSR
            mov     cx, AMD_MTRR_FIX4k_C0000    ; Fixed 4Ks: MTRRfix4K_C0000 to MTRRfix4K_F8000
            .while (cl != 70h)
                _WRMSR
                inc     cl
            .endw

            ; Set TOP_MEM (C001_001A) for non-shared cores to 16M. This will be increased at heap init.
            ;  - not strictly needed since the FixedMTRRs take presedence.
            mov     eax, (16 * 1024 * 1024)
            mov     ecx, TOP_MEM                ; MSR:C001_001A
            _WRMSR
        .endif                          ;   End Is_Primary
    .endif                              ; End Stack_ReEntry

    ; Clear IORRs (C001_0016-19) and TOM2(C001_001D) for all cores
    xor     eax, eax
    xor     edx, edx
    mov     ecx, IORR_BASE              ; MSR:C001_0016 - 0019
    .while (cl != 1Ah)
        _WRMSR
        inc     cl
    .endw
    mov     ecx, TOP_MEM2               ; MSR:C001_001D
    _WRMSR

    ; setup MTTRs for stacks
    ;   A speculative read can be generated by a speculative fetch mis-aligned in a code zone
    ;    or due to a data zone being interpreted as code. When a speculative read occurs outside a
    ;    controlled region (intentionally used by software), it could cause an unwanted cache eviction.
    ;   To prevent speculative reads from causing an eviction, the unused cache ranges are set
    ;    to UC type. Only the actively used regions (stack, heap) are reflected in the MTRRs.
    ;    Note: some core stack regions will share an MTRR since the control granularity is much
    ;    larger than the allocated stack zone. The allocation algorithm must account for this 'extra'
    ;    space covered by the MTRR when parseling out cache space for the various uses. In some cases
    ;    this could reduce the amount of EXE cache available to a core. see cpuCacheInit.c
    ;
    ; Outcome of this block is that:   (Note the MTRR map at the top of the file)
    ;   ebp - start address of stack block
    ;   ebx - [31:16] - MTRR MSR address
    ;       - [15:8]  - slot# in MTRR register
    ;       - [7:0]   - block size in #4K blocks
    ; review: ESI[31:24]=Flags; SI[15,8]= Node#; SI[7,0]= core# (relative to node)
    ;

    mov     ax, si                      ; Load node, core
    .if (al == 0)                       ; Is a core 0?
        .if (ah == 0)                   ; Is Node 0? (BSP)
            ; Is BSP, assign a 64K stack
            mov     ebx, ((AMD_MTRR_FIX64k_00000 SHL 16) + (3 SHL 8) + (BSP_STACK_SIZE  / 1000h))
            mov     ebp, BSP_STACK_BASE_ADDR
        .else   ; node 1 to 7, core0
            ; Is a Core0 of secondary node, assign 16K stacks
            mov     bx, AMD_MTRR_FIX16k_80000
            shl     ebx, 16             ;
            mov     bh, ah              ; Node# is used as slot#
            mov     bl, (CORE0_STACK_SIZE / 1000h)
            mov     al, ah              ; Base = (Node# * Size);
            mul     bl                  ;
            movzx   eax, ax             ;
            shl     eax, 12             ; Expand back to full byte count (* 4K)
            add     eax, CORE0_STACK_BASE_ADDR
            mov     ebp, eax
        .endif
    .else    ;core 1 thru core 7
        ; Is core 1-7 of any node, assign 4K stacks
        mov     al, 8                   ; CoreIndex = ( (Node# * 8) ...
        mul     ah                      ;
        mov     bx, si                  ;
        add     al, bl                  ;         ...  + Core#);

        mov     bx, AMD_MTRR_FIX64k_00000
        shl     ebx, 16                 ;
        mov     bh, al                  ; Slot# = (CoreIndex / 16) + 4;
        shr     bh, 4                   ;
        add     bh, 4                   ;
        mov     bl, (CORE1_STACK_SIZE / 1000h)

        mul     bl                      ; Base = ( (CoreIndex * Size) ...
        movzx   eax, ax                 ;
        shl     eax, 12                 ; Expand back to full byte count (* 4K)
        add     eax, CORE1_STACK_BASE_ADDR ;     ...   + Base_Addr);
        mov     ebp, eax
    .endif

    ; Now set the MTRR. Add this to already existing settings (don't clear any MTRR)
    mov     edi, WB_DRAM_TYPE           ; Load Cache type in 1st slot
    mov     cl, bh                      ; ShiftCount =  ((slot#   ...
    and     cl, 03h                     ;   ...  % 4)             ...
    shl     cl, 3                       ;   ...  * 8);
    shl     edi, cl                     ; Cache type is now in correct position
    ror     ebx, 16                     ; Get the MTRR address
    movzx   ecx, bx                     ;
    rol     ebx, 16                     ; Put slot# & size back in BX
    _RDMSR                              ; Read-modify-write the MSR
    .if (bh < 4)                        ; Is value in lower or upper half of MSR?
        or      eax, edi                ;
    .else                               ;
        or      edx, edi                ;
    .endif                              ;
    _WRMSR                              ;

    ; Enable MTRR defaults as UC type
    mov     ecx, AMD_MTRR_DEFTYPE       ; MSR:0000_02FF
    _RDMSR                              ; Read-modify-write the MSR
    bts     eax, MTRR_DEF_TYPE_EN       ; MtrrDefTypeEn
    bts     eax, MTRR_DEF_TYPE_FIX_EN   ; MtrrDefTypeFixEn
    _WRMSR

    ; Close the modification window on the Fixed MTRRs
    mov     ecx, MTRR_SYS_CFG           ; MSR:0C001_0010
    _RDMSR
    bts     eax, MTRR_FIX_DRAM_EN       ; MtrrFixDramEn
    bts     eax, MTRR_VAR_DRAM_EN       ; variable MTRR enable bit
    btr     eax, MTRR_FIX_DRAM_MOD_EN   ; Turn off modification enable bit
    _WRMSR

    ; Enable caching in CR0
    mov     eax, CR0                    ; Enable WT/WB cache
    btr     eax, CR0_PG                 ; Make sure paging is disabled
    btr     eax, CR0_CD                 ; Clear CR0 NW and CD
    btr     eax, CR0_NW
    mov     CR0, eax

    ; Use the Stack Base & size to calculate SS and ESP values
    ; review:
    ;       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    ;       ebp - start address of stack block
    ;       ebx - [31:16] - MTRR MSR address
    ;           - [15:8]  - slot# in MTRR register
    ;           - [7:0]   - block size in #4K blocks
    ;
    mov     esp, ebp                    ; Initialize the stack pointer
    mov     edi, esp                    ; Copy the stack start to edi
    movzx   bx, bl
    movzx   ebx, bx                     ; Clear upper ebx, don't need MSR addr anymore
    shl     ebx, 12                     ; Make size full byte count (* 4K)
    add     esp, ebx                    ; Set the Stack Pointer as full linear address
    sub     esp, 4
    ;
    ; review:
    ;       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    ;       edi - 32b start address of stack block
    ;       ebx - size of stack block
    ;       esp - 32b linear stack pointer
    ;

    ; Determine mode for SS base;
    mov     ecx, CR0                    ; Check for 32-bit protect mode
    bt      ecx, CR0_PE                 ;
    .if (!carry?)                       ; PE=0 means real mode
        mov     cx, cs                  ;
        .if (cx >= 0D000h)              ; If CS >= D000, it's a real mode segment. PM selector would be 08-> 1000
            ; alter SS:ESP for 16b Real Mode:
            mov     eax, edi            ;
            shr     eax, 4              ;   Create a Real Mode segment for ss, ds, es
            mov     ss, ax              ;
            mov     ds, ax              ;
            mov     es, ax              ;
            shl     eax, 4              ;
            sub     edi, eax            ;   Adjust the clearing pointer for Seg:Offset mode
            mov     esp, ebx            ;   Make SP an offset from SS
            sub     esp, 4              ;
        .endif                          ; endif
    ; else
    ;   Default is to use Protected 32b Mode
    .endif
    ;
    ; Clear The Stack
    ;   Now that we have set the location and the MTRRs, initialize the cache by
    ;   reading then writing to zero all of the stack area.
    ; review:
    ;       ss  - Stack base
    ;       esp - stack pointer
    ;       ebx - size of stack block
    ;       esi[31:24]=Flags; esi[15,8]= Node#; esi[7,0]= core# (relative to node)
    ;       edi -  address of start of stack block
    ;
    shr     ebx, 2                      ;
    mov     cx, bx                      ; set cx for size count of DWORDS
    ; Check our flags - Don't clear an existing stack
    .if ( !(esi & (1 SHL FLAG_STACK_REENTRY)) )
        cld
        mov     esi, edi
        rep     lods DWORD PTR [esi]    ; Pre-load the range
        xor     eax, eax
        mov     cx, bx
        mov     esi, edi                ; Preserve base for push on stack
        rep     stos DWORD PTR [edi]    ; Clear the range
        mov     DWORD PTR [esp], 0ABCDDCBAh ; Put marker in top stack dword
        shl     ebx, 2                  ; Put stack size and base
        push    ebx                     ;  in top of stack
        push    esi

        mov     ecx, ebx                ; Return size of stack in bytes
        mov     eax, AGESA_SUCCESS      ; eax = AGESA_SUCCESS : no error return code
    .else
        movzx   ecx, cx
        shl     ecx, 2                  ; Return size of stack, in bytes
        mov     edx, CPU_EVENT_STACK_REENTRY
        mov     eax, AGESA_WARNING      ; eax = AGESA_WARNING (Stack has already been set up)
    .endif

AmdEnableStackExit:
    movd        ebx, mm0                ; Restore return address
    movd        ebp, mm1
ENDM

;======================================================================
; AMD_DISABLE_STACK:  Destroy the stack inside the cache. This routine
;                     should only be executed on the BSP
;
;   In:
;       none
;
;   Out:
;       EAX = AGESA_SUCCESS
;
;   Preserved:
;       ebx
;   Destroyed:
;       eax, ecx, edx, esp
;======================================================================
AMD_DISABLE_STACK MACRO

    mov     esp, ebx                    ; Save return address

    ; get node/core/flags of current executing core
    GET_NODE_ID_CORE_ID                 ; Sets ESI[15,8]= Node#; ESI[7,0]= core# (relative to node)

    ; Turn on modification enable bit
    mov     ecx, MTRR_SYS_CFG           ; MSR:C001_0010
    _RDMSR
    bts     eax, MTRR_FIX_DRAM_MOD_EN   ; Enable modifications
    _WRMSR

    ; Set lower 640K MTRRs for Write-Back memory caching
    mov     ecx, AMD_MTRR_FIX64k_00000
    mov     eax, 1E1E1E1Eh
    mov     edx, eax
    _WRMSR                              ; 0 - 512K = WB Mem
    mov     ecx, AMD_MTRR_FIX16k_80000
    _WRMSR                              ; 512K - 640K = WB Mem

    ; Turn off modification enable bit
    mov     ecx, MTRR_SYS_CFG           ; MSR:C001_0010
    _RDMSR
    btr     eax, MTRR_FIX_DRAM_MOD_EN   ; Disable modification
    _WRMSR

    AMD_DISABLE_STACK_FAMILY_HOOK       ; Re-Enable 'normal' cache operations

    mov     ebx, esp                    ; restore return address (ebx)
    xor     eax, eax

ENDM
