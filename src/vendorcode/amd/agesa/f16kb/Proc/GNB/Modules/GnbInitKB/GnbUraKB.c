/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize PP/DPM fuse table.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbUra.h"
#include  "GnbUraToken.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBURAKB_FILECODE

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
VOID
GnbUraGetKB (
  IN       DEV_OBJECT      *Device,
  IN       URA_TOKEN_INFO  *UraTokenInfo,
  IN OUT   VOID            *Value
  );

VOID
GnbUraSetKB (
  IN       DEV_OBJECT      *Device,
  IN       URA_TOKEN_INFO  *UraTokenInfo,
  IN OUT   VOID            *Value
  );

VOID
GnbUraStreamSetKB (
  IN       DEV_OBJECT        *Device,
  IN       URA_TOKEN_INFO    *UraTokenInfo,
  IN OUT   URA_TUPLE         *UraTuple,
  IN       UINT32            CombinedCount
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device           Pointer to Device object
 * @param[in]       UraTokenInfo     Pointer to URA_TOKEN_INFO structure
 * @param[in, out]  Value            Pointer to Context
 */
VOID
GnbUraGetKB (
  IN       DEV_OBJECT      *Device,
  IN       URA_TOKEN_INFO  *UraTokenInfo,
  IN OUT   VOID            *Value
  )
{
  ACCESS_WIDTH      Width;
  UINT32            RegValue;
  UINT32            FieldMask;
  UINT32            TargetAddress;

  RegValue = 0;
  Width = (UraTokenInfo->Flags == GNB_URA_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  switch (UraTokenInfo->MethodType) {
  case TYPE_GNB_INDIRECT_ACCESS:
    TargetAddress = Device->DevPciAddress.AddressValue | UraTokenInfo->RegDomainType;
    GnbLibPciIndirectRead ( TargetAddress, UraTokenInfo->RegAddress, Width, &RegValue, Device->StdHeader);
    IDS_HDT_CONSOLE (NB_MISC, "  Ura GET:  RegDomainType = 0x%x  IndirectAddress = 0x%08x, Value = 0x%08x\n", UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, RegValue);
    break;

  case TYPE_GNB_PROTOCOL_ACCESS:
    TargetAddress = UraTokenInfo->RegDomainType;
    GnbRegisterReadKB (Device->GnbHandle, (UINT8)TargetAddress, UraTokenInfo->RegAddress, &RegValue, UraTokenInfo->Flags, Device->StdHeader);
    IDS_HDT_CONSOLE (NB_MISC, "  Ura GET:  RegDomainType = %d, Address = 0x%08x, Value = 0x%08x\n",  TargetAddress, UraTokenInfo->RegAddress, RegValue);
    break;

  default:
    ASSERT (FALSE);
    return;
  }

  if (UraTokenInfo->WholeRegAccess == TRUE) {
    *(UINT32 *)Value = RegValue;
  } else {
    RegValue = RegValue >> UraTokenInfo->BfOffset;
    FieldMask = (((UINT32)1 << UraTokenInfo->BfWidth) - 1);
    *(UINT32 *)Value = RegValue & FieldMask;
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device           Pointer to Device object
 * @param[in]       UraTokenInfo     Pointer to URA_TOKEN_INFO structure
 * @param[in, out]  Value            Pointer to Context
 */
VOID
GnbUraSetKB (
  IN       DEV_OBJECT      *Device,
  IN       URA_TOKEN_INFO  *UraTokenInfo,
  IN OUT   VOID            *Value
  )
{
  ACCESS_WIDTH       Width;
  UINT32                   RegValue;
  UINT32                   TargetValue;
  UINT32                   FieldMask;
  UINT32                   TempValue;

  Width = (UraTokenInfo->Flags == GNB_URA_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  FieldMask = 0;
  TempValue = *(UINT32 *)Value;

  switch (UraTokenInfo->MethodType) {
  case TYPE_GNB_INDIRECT_ACCESS:
    if (UraTokenInfo->WholeRegAccess == TRUE) {
      TargetValue =  TempValue;
    } else {
      GnbLibPciIndirectRead ( Device->DevPciAddress.AddressValue | UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, Width, &RegValue, Device->StdHeader);
      FieldMask = (((UINT32)1 << UraTokenInfo->BfWidth) - 1);
      TargetValue = RegValue & (~(FieldMask << UraTokenInfo->BfOffset));
      TargetValue |= (TempValue & FieldMask) << UraTokenInfo->BfOffset;
    }
    GnbLibPciIndirectWrite ( Device->DevPciAddress.AddressValue | UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, Width, &TargetValue, Device->StdHeader);
    IDS_HDT_CONSOLE (NB_MISC, "  Ura SET:  RegDomainType = 0x%x  IndirectAddress = 0x%08x, Value = 0x%08x\n", UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, TargetValue);
    break;

  case TYPE_GNB_PROTOCOL_ACCESS:
    if (UraTokenInfo->WholeRegAccess == TRUE) {
      TargetValue =  TempValue;
    } else {
      GnbRegisterReadKB (Device->GnbHandle, UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, &RegValue, UraTokenInfo->Flags, Device->StdHeader);
      FieldMask = (((UINT32)1 << UraTokenInfo->BfWidth) - 1);
      TargetValue = RegValue & (~(FieldMask << UraTokenInfo->BfOffset));
      TargetValue |= (TempValue & FieldMask) << UraTokenInfo->BfOffset;
    }
    GnbRegisterWriteKB (Device->GnbHandle, UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, &TargetValue, UraTokenInfo->Flags, Device->StdHeader);
    IDS_HDT_CONSOLE (NB_MISC, "  Ura SET:  RegDomainType = %d, Address = 0x%08x, Value = 0x%08x\n",  UraTokenInfo->RegDomainType, UraTokenInfo->RegAddress, TargetValue);
    break;

  default:
    ASSERT (FALSE);
    break;
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device             Pointer to device object
 * @param[in]       UraTokenInfo       Pointer to URA_TOKEN_INFO structure
 * @param[in, out]  UraTuple           Pointer to structure URA_TUPLE
 * @param[in]       CombinedCount      Token count
 */
VOID
GnbUraStreamSetKB (
  IN       DEV_OBJECT        *Device,
  IN       URA_TOKEN_INFO    *UraTokenInfo,
  IN OUT   URA_TUPLE         *UraTuple,
  IN       UINT32            CombinedCount
  )
{
  ACCESS_WIDTH      Width;
  UINT32            RegValue;
  UINT32            Index;
  UINT32            StreamSetAddress;
  UINT32            StepLength;
  UINT32            TargetAddress;

  Width = (UraTokenInfo->Flags == GNB_URA_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  StreamSetAddress = UraTokenInfo->RegAddress;
  StepLength = UraTuple->StepLength;
  for (Index = 0; Index < CombinedCount; Index++) {
    RegValue = *(((UINT32 *) ((UINTN)UraTuple->Value)) + Index);
    switch (UraTokenInfo->MethodType) {
    case TYPE_GNB_INDIRECT_ACCESS:
      TargetAddress = Device->DevPciAddress.AddressValue | UraTokenInfo->RegDomainType;
      //IDS_HDT_CONSOLE (NB_MISC, "0x%08x:0x%08x, \n", StreamSetAddress, RegValue);
      GnbLibPciIndirectWrite (TargetAddress, StreamSetAddress, Width, &RegValue, Device->StdHeader);
      break;

    case TYPE_GNB_PROTOCOL_ACCESS:
      TargetAddress = UraTokenInfo->RegDomainType;
      GnbRegisterWriteKB (Device->GnbHandle, (UINT8)TargetAddress, StreamSetAddress, &RegValue, UraTokenInfo->Flags, Device->StdHeader);
      break;

    default:
      ASSERT (FALSE);
      return;
    }
    StreamSetAddress += StepLength;
  }
}

