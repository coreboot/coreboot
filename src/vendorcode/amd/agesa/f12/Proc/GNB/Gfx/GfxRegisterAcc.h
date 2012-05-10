/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics controller access service routines.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/
#ifndef _GFXREGISTERACC_H_
#define _GFXREGISTERACC_H_

/// GMM Register Entry
typedef struct {
  UINT16  GmmReg;             ///< Register
  UINT32  GmmData;            ///< Data
} GMM_REG_ENTRY;

/// Register to Register copy
typedef struct {
  UINT32  CpuReg;             ///< CPU Register
  UINT16  GmmReg;             ///< GMM Register
  UINT8   CpuOffset;          ///< CPU register field start bit
  UINT8   CpuWidth;           ///< CPU register field width
  UINT8   GmmOffset;          ///< GMM register field start bit
  UINT8   GmmWidth;           ///< GMM register field width
} REGISTER_COPY_ENTRY;


/// Table length and table pointer
typedef struct {
  UINT32  TableLength;        ///< Table Length
  VOID*   TablePtr;           ///< Table Pointer
} TABLE_INDIRECT_PTR;

VOID
GmmRegisterWrite (
  IN      UINT16                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

UINT32
GmmRegisterRead (
  IN      UINT16                Address,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GmmRegisterWriteField (
  IN      UINT16                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );


VOID
GmmRegisterTableWrite (
  IN      GMM_REG_ENTRY         Table[],
  IN      UINTN                 TableLength,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxLibCopyMemToFb (
  IN     VOID                   *Source,
  IN     UINT32                  FbOffset,
  IN     UINT32                  Length,
  IN     GFX_PLATFORM_CONFIG    *Gfx
  );

#endif
