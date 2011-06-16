/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39623 $   @e \$Date: 2010-10-13 13:37:42 -0700 (Wed, 13 Oct 2010) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "cpuFamilyTranslation.h"
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbLibStall.h"
#include  "GnbTable.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBTABLE_FILECODE
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
 * Process table
 *
 *
 *
 * @param[in] Table           Table pointer
 * @param[in] Property        Property
 * @param[in] Flags           Flags
 * @param[in] Protocol        Register access protocol
 * @param[in] StdHeader       Standard configuration header
 */
AGESA_STATUS
GnbProcessTable (
  IN      GNB_TABLE               *Table,
  IN      UINT32                  Property,
  IN      UINT32                  Flags,
  IN      GNB_REGISTER_PROTOCOL   *Protocol,
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{
  return GnbProcessTableExt (0, 0, Table, Property, Flags, Protocol, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Process table
 *
 * @param[in] Socket          Socket
 * @param[in] Module          Module
 * @param[in] Table           Table pointer
 * @param[in] Property        Property
 * @param[in] Flags           Flags
 * @param[in] Protocol        Register access protocol
 * @param[in] StdHeader       Standard configuration header
 */

AGESA_STATUS
GnbProcessTableExt (
  IN      UINT32                Socket,
  IN      UINT8                 Module,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      GNB_REGISTER_PROTOCOL *Protocol,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8           *EntryPointer;
  UINT64          Data;
  UINT64          Temp;
  UINT64          Mask;
  UINT32          WriteAccFlags;
  CPU_LOGICAL_ID  LogicalId;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Enter\n");
  IDS_HDT_CONSOLE (GNB_TRACE, " Property - 0x%08x\n", Property);
  GetLogicalIdOfSocket (Socket, &LogicalId, StdHeader);
  EntryPointer = (UINT8 *) Table;
  WriteAccFlags = 0;
  if ((Flags & GNB_TABLE_FLAGS_FORCE_S3_SAVE) != 0) {
    WriteAccFlags |= GNB_REG_ACC_FLAG_S3SAVE;
  }
  while (*EntryPointer != GnbEntryTerminate) {
    Data = 0;
    Temp = 0;
    switch (*EntryPointer) {
    case GnbEntryWr:
      Protocol->Write (
        ((GNB_TABLE_ENTRY_WR*) EntryPointer)->RegisterSpaceType,
        ((GNB_TABLE_ENTRY_WR*) EntryPointer)->Address,
        &((GNB_TABLE_ENTRY_WR*) EntryPointer)->Value,
        WriteAccFlags,
        StdHeader
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_WR);
      break;
    case GnbEntryPropertyWr:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Property) != 0) {
        Protocol->Write (
          ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Address,
          &((GNB_TABLE_ENTRY_PROPERTY_WR *) EntryPointer)->Value,
          WriteAccFlags,
          StdHeader
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_WR);
      break;
    case GnbEntryFullWr:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Property) != 0) {
        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Revision) != 0) {
          Protocol->Write (
            ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Address,
            &((GNB_TABLE_ENTRY_FULL_WR*) EntryPointer)->Value,
            WriteAccFlags,
            StdHeader
            );
        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_WR);
      break;
    case GnbEntryRmw:
      Protocol->Read (
        ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->RegisterSpaceType,
        ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->Address,
        &Data,
        0,
        StdHeader
        );
      Data = (Data & (~ (UINT64) ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->AndMask)) | ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->OrMask;
      Protocol->Write (
        ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->RegisterSpaceType,
        ((GNB_TABLE_ENTRY_RMW*) EntryPointer)->Address,
        &Data,
        WriteAccFlags,
        StdHeader
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_RMW);
      break;
    case GnbEntryPropertyRmw:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Property) != 0) {
        Protocol->Read (
          ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Address,
          &Data,
          0,
          StdHeader
          );
        Data = (Data & (~ (UINT64) ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->AndMask)) | ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->OrMask;
        Protocol->Write (
          ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Address,
          &Data,
          WriteAccFlags,
          StdHeader
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_RMW);
      break;
    case GnbEntryFullRmw:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_WR *) EntryPointer)->Property) != 0) {
        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_WR *) EntryPointer)->Revision) != 0) {
          Protocol->Read (
            ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Address,
            &Data,
            0,
            StdHeader
            );
          Data = (Data & (~ (UINT64) ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->AndMask)) | ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->OrMask;
          Protocol->Write (
            ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Address,
            &Data,
            WriteAccFlags,
            StdHeader
            );
        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_RMW);
      break;
    case GnbEntryPoll:
      do {
        Protocol->Read (
          ((GNB_TABLE_ENTRY_POLL *) EntryPointer)->RegisterSpaceType,
          ((GNB_TABLE_ENTRY_POLL *) EntryPointer)->Address,
          &Data,
          0,
          StdHeader
          );
      } while ((Data & ((GNB_TABLE_ENTRY_POLL*) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_POLL*) EntryPointer)->CompareValue);
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_POLL);
      break;
    case GnbEntryPropertyPoll:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->Property) != 0) {
        do {
          Protocol->Read (
            ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->RegisterSpaceType,
            ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->Address,
            &Data,
            0,
            StdHeader
            );
        } while ((Data & ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_PROPERTY_POLL *) EntryPointer)->CompareValue);
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_POLL);
      break;
    case GnbEntryCopy:
      Protocol->Read (
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcAddress,
        &Data,
        0,
        StdHeader
        );
      Mask = (1ull << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldWidth) - 1;
      Data = (Data >> ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldOffset) & Mask;
      Protocol->Read (
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestAddress,
        &Temp,
        0,
        StdHeader
        );
      Mask = (1ull << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldWidth) - 1;
      Temp = Temp & ( ~ (Mask << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset));
      Temp = Temp | ((Data & Mask) << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset);
      Protocol->Write (
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestAddress,
        &Temp,
        WriteAccFlags,
        StdHeader
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_COPY);
      break;
    case GnbEntryStall:
      if ((WriteAccFlags & GNB_TABLE_FLAGS_FORCE_S3_SAVE) != 0) {
        GnbLibStallS3Save (((GNB_TABLE_ENTRY_STALL*) EntryPointer)->Microsecond, StdHeader);
      } else {
        GnbLibStall (((GNB_TABLE_ENTRY_STALL*) EntryPointer)->Microsecond, StdHeader);
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_STALL);
      break;
    default:
      ASSERT (FALSE);
      IDS_HDT_CONSOLE (NB_MISC, "  ERROR!!! Regiter table parse\n");
      return AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Exit\n");
  return AGESA_SUCCESS;
}
