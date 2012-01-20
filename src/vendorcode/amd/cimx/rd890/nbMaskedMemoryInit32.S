/*
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 */

/******************************************************************************
* CIMX Northbridge
*
* Contains CIMX 32 bit library code
*
* Description: nbMaskedMemoryInit32.S - nb Masked Memory Init 32bit ASM code
*
******************************************************************************/

.altmacro

#include <cpu/x86/post_code.h>

        .section ".text"
        .code32
        .globl NbInitMaskedMemory, NbInitMaskedMemoryLength

/*
* PciAddress   = 8(%ebp)
* AlinkAddress = 12(%ebp)
*/
.type NbInitMaskedMemory, @function

NbInitMaskedMemory:
        push    %ebp
        movl     %esp, %ebp
        pusha
        cmp $0, 12(%ebp)
        jz	0f
        mov 12(%ebp), %dx
        mov $0x80000004, %eax
        out %eax, %dx
        add $4, %dx
        in %dx, %eax
        btr $2, %eax
        out %eax, %dx
       0:
        mov 8(%ebp), %eax
        shr $4, %eax
        or  $0x80000060, %eax
        mov %eax, %ebx
        mov $0x0cf8, %dx
        out %eax, %dx
        mov $0x0cfc, %dx
        mov $(0x80 | 0x1B), %eax      # NB_MISC_REG1B
        out %eax, %dx
        mov %ebx, %eax
        mov $0x64, %al
        mov $0x0cf8, %dx
        out %eax, %dx
        mov $0x0cfc, %dx
        in  %dx, %eax
        and $(~(0x01ff << 21)), %eax
        bts $15, %eax
        xor %ecx, %ecx
        out %eax, %dx
StartInit:
        and $(~(0xff << 22)), %eax
        shl $22, %ecx
        or  %ecx, %eax
        shr $22, %ecx
        out %eax, %dx
        bts $21, %eax
        out %eax, %dx
        btr $21, %eax
        out %eax, %dx
        cmp $0x0ff, %cl
        je  DoneInit
        inc %ecx
        jmp StartInit
DoneInit:
        btr $15, %eax
        out %eax, %dx
        cmp $0, 12(%ebp)
        jz	1f
        mov 12(%ebp), %dx
        mov $0x80000004, %eax
        out %eax, %dx
        add $4, %dx
        in  %dx, %eax
        bts $2, %eax
        out %eax, %dx
        1:
        popa
	movl %ebp, %esp
        pop     %ebp
        ret
NbInitMaskedMemoryLength = ( . - NbInitMaskedMemory)

