/**

Copyright (C) 2013, Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/

#ifndef __UEFI_TYPES_H__
#define __UEFI_TYPES_H__

//
// Set the UEFI types and attributes
//
#define __APPLE__ 0
#include <stdlib.h>
#include <Uefi/UefiBaseType.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiHob.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/HobLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include <IndustryStandard/PeImage.h>

///
/// For GNU assembly code, .global or .globl can declare global symbols.
/// Define this macro to unify the usage.
///
#if defined(ASM_GLOBAL)
#undef ASM_GLOBAL
#endif
#define ASM_GLOBAL .global

//
// Define the ASSERT support
//
static inline void debug_dead_loop(void)
{
	for (;;)
		;
}

#define _ASSERT(expression)  debug_dead_loop()
#ifndef ASSERT
#define ASSERT(expression)			\
	do {					\
		if (!(expression)) {		\
			_ASSERT(expression);	\
		}				\
	} while (FALSE)
#endif

//
// Contents of the PEI_GRAPHICS_INFO_HOB
//
typedef struct {
	EFI_PHYSICAL_ADDRESS			FrameBufferBase;
	UINT32					FrameBufferSize;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION	GraphicsMode;
} EFI_PEI_GRAPHICS_INFO_HOB;

//
// Define the known GUIDs
//
#define EFI_PEI_GRAPHICS_INFO_HOB_GUID				\
{								\
	0x39f62cce, 0x6825, 0x4669,				\
	{ 0xbb, 0x56, 0x54, 0x1a, 0xba, 0x75, 0x3a, 0x07 }	\
}

#define FSP_BOOTLOADER_TEMP_MEMORY_HOB_GUID			\
{								\
	0xbbcff46c, 0xc8d3, 0x4113,				\
	{0x89, 0x85, 0xb9, 0xd4, 0xf3, 0xb3, 0xf6, 0x4e}	\
}

#define FSP_BOOTLOADER_TOLUM_HOB_GUID				\
{								\
	0x73ff4f56, 0xaa8e, 0x4451,				\
	{ 0xb3, 0x16, 0x36, 0x35, 0x36, 0x67, 0xad, 0x44 }	\
}

#define FSP_INFO_HEADER_GUID					\
{								\
	0x912740BE, 0x2284, 0x4734,				\
	{0xB9, 0x71, 0x84, 0xB0, 0x27, 0x35, 0x3F, 0x0C}	\
}

#define FSP_NON_VOLATILE_STORAGE_HOB_GUID			\
{								\
	0x721acf02, 0x4d77, 0x4c2a,				\
	{0xb3, 0xdc, 0x27, 0x0b, 0x7b, 0xa9, 0xe4, 0xb0}	\
}

#define FSP_RESERVED_MEMORY_RESOURCE_HOB_GUID			\
{								\
	0x69a79759, 0x1373, 0x4367,				\
	{ 0xa6, 0xc4, 0xc7, 0xf5, 0x9e, 0xfd, 0x98, 0x6e }	\
}

#define FSP_SMBIOS_MEMORY_INFO_GUID				\
{								\
	0x01a1108c, 0x9dee, 0x4984,				\
	{ 0x88, 0xc3, 0xee, 0xe8, 0xc4, 0x9e, 0xfb, 0x89 }	\
}

#endif	/* __UEFI_TYPES_H__*/
