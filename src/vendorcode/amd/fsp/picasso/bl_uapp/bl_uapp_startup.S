/*****************************************************************************
 *
 * Copyright (c) 2019, Advanced Micro Devices, Inc.
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
 ***************************************************************************/

#include <arch/asm.h>
#include <bl_uapp/bl_errorcodes_public.h>

	.global Main
	.global _psp_vs_start

	.global  PSP_VERSTAGE_STACK_BASE
	.global  PSP_VERSTAGE_STACK_END
	.global  PSP_VERSTAGE_TEMP_STACK_END

.arm
.text
.section "PSP_HEADER_DATA", "aw", %note

//==============================================================================
// First 256 bytes of the binary image contain the header.
// Executable code starts from offset 0x100.
//==============================================================================
#include "bl_uapp_header.inc"

//==============================================================================
// This is entry point to the binary which is called by main Boot Loader.
//==============================================================================

ENTRY(_psp_vs_start)
	// Map Debug Unlock stack to separate Virtual Address so that stack
	// overflow cause exception instead of data corruption.
	//

	ldr	sp, =PSP_VERSTAGE_TEMP_STACK_END // temporary stack pointer
	ldr	r2, =AllocateStack
	blx	r2

	// Return value contains Virtual Address of mapped stack
	//
	mov	sp, r0                  // set OEM PSP BL user app stack pointer
	ldr	lr, =ShouldNotBeReached // return address

	ldr	r2, =Main               // pass control to verstage main function
	blx	r2

// This point should not be reached. The Main() function should return
// to main BL using Svc_Exit().
//
ShouldNotBeReached:
	mov	r0, #BL_ERR_GENERIC     // Returned from Main
	svc	#0x0                    // SVC_EXIT

ENDPROC(_psp_vs_start)

//==============================================================================
// Code to allocate an address space for stack
//==============================================================================
.global AllocateStack

ENTRY(AllocateStack)
	push	{r3,lr}
	ldr	r0, =PSP_VERSTAGE_STACK_BASE
	mov	r2, sp
	ldr	r1, =PSP_VERSTAGE_STACK_END
	svc	#0x1                    // 0x1 is the SvcID for SVC_MAP_USER_STACK
	cmp	r0, #0
	bne	svcExit
	ldr	r0,[sp,#0]
	cmp	r0, #0                  // r0 contains the virtual stack address
	bne	ret

svcExit:
	mov	r0, #BL_ERR_GENERIC
	svc	#0x0                    // SVC_EXIT

ret:
	pop	{r3,pc}

ENDPROC(AllocateStack)
.end
