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
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
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
 * 
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxRegisterAcc.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXREGISTERACC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Write GMM register
 *
 *
 * @param[in] Address         GMM register address
 * @param[in] Value           Value
 * @param[in] S3Save          Save for S3 resume
 * @param[in] Gfx             Pointer to global GFX configuration
 */

VOID
GmmRegisterWrite (
  IN      UINT16                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  ASSERT (Gfx->GmmBase != 0);
  GnbLibMemWrite (Gfx->GmmBase + Address, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Value, GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read GMM register
 *
 *
 * @param[in] Address         GMM register address
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval                    Value of GMM register
 */

UINT32
GmmRegisterRead (
  IN      UINT16                Address,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32  Value;
  ASSERT (Gfx->GmmBase != 0);
  GnbLibMemRead (Gfx->GmmBase + Address, AccessWidth32, &Value, GnbLibGetHeader (Gfx));
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write GMM register field
 *
 *
 * @param[in] Address         GMM register address
 * @param[in] FieldOffset     Register field offset
 * @param[in] FieldWidth      Register field width
 * @param[in] Value           Field value
 * @param[in] S3Save          Save for S3 resume
 * @param[in] Gfx             Pointer to global GFX configuration
 */

VOID
GmmRegisterWriteField (
  IN      UINT16                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32  Data;
  UINT32  Mask;
  Data = GmmRegisterRead (Address, Gfx);
  Mask = (1 << FieldWidth) - 1;
  Value &= Mask;
  Data &= (~(Mask << FieldOffset));
  GmmRegisterWrite (Address, Data | (Value << FieldOffset), S3Save, Gfx);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write GMM registers table
 *
 *
 * @param[in] Table           Pointer to table
 * @param[in] TableLength     Number of entries in table
 * @param[in] S3Save          Save for S3 resume
 * @param[in] Gfx             Pointer to global GFX configuration
 */


VOID
GmmRegisterTableWrite (
  IN      GMM_REG_ENTRY         Table[],
  IN      UINTN                 TableLength,
  IN      BOOLEAN               S3Save,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINTN Index;
  for (Index = 0; Index < TableLength; Index++) {
    GmmRegisterWrite (Table[Index].GmmReg, Table[Index].GmmData, S3Save, Gfx);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy memory content to FB
 *
 *
 * @param[in] Source       Pointer to source
 * @param[in] FbOffset     FB offset
 * @param[in] Length       The length to copy
 * @param[in] Gfx          Pointer to global GFX configuration
 *
 */
VOID
GfxLibCopyMemToFb (
  IN     VOID                   *Source,
  IN     UINT32                  FbOffset,
  IN     UINT32                  Length,
  IN     GFX_PLATFORM_CONFIG    *Gfx
  )
{
  GMMx00_STRUCT GMMx00;
  GMMx04_STRUCT GMMx04;
  UINT32        Index;
  for (Index = 0; Index < Length; Index = Index + 4 ) {
    GMMx00.Value = 0x80000000 | (FbOffset + Index);
    GMMx04.Value = *(UINT32*) ((UINT8*)Source + Index);
    GmmRegisterWrite (GMMx00_ADDRESS, GMMx00.Value, FALSE, Gfx);
    GmmRegisterWrite (GMMx04_ADDRESS, GMMx04.Value, FALSE, Gfx);
  }
}
