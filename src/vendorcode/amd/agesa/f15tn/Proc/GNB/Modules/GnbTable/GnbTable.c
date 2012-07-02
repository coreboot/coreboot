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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include  "GnbPcieConfig.h"
#include  "GnbLib.h"
#include  "GnbTimerLib.h"
#include  "GnbFamServices.h"
#include  "GnbTable.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBTABLE_GNBTABLE_FILECODE
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
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_REGISTER_SERVICE  *GnbRegisterAccessProtocol,
  IN      GNB_RMW_BLOCK         *Data,
  IN      UINT32                Flags,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Process table
 *
 * @param[in] GnbHandle       Gnb handle
 * @param[in] Table           Table pointer
 * @param[in] Property        Property
 * @param[in] Flags           Flags
 * @param[in] StdHeader       Standard configuration header
 */

AGESA_STATUS
GnbProcessTable (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8                   *EntryPointer;
  UINT64                  Data;
  UINT64                  Temp;
  UINT64                  Mask;
  UINT32                  WriteAccFlags;
  GNB_REGISTER_SERVICE    *GnbRegisterAccessProtocol;
  CPU_LOGICAL_ID          LogicalId;
  AGESA_STATUS            Status;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbProcessTableExt Enter\n");
  IDS_HDT_CONSOLE (GNB_TRACE, " Property - 0x%08x\n", Property);

  GetLogicalIdOfSocket (GnbGetSocketId (GnbHandle), &LogicalId, StdHeader);
  EntryPointer = (UINT8 *) Table;
  WriteAccFlags = 0;
  if ((Flags & GNB_TABLE_FLAGS_FORCE_S3_SAVE) != 0) {
    WriteAccFlags |= GNB_REG_ACC_FLAG_S3SAVE;
  }

  Status = GnbLibLocateService (GnbRegisterAccessService, GnbGetSocketId (GnbHandle), (VOID **)&GnbRegisterAccessProtocol, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);

  while (*EntryPointer != GnbEntryTerminate) {
    Data = 0;
    Temp = 0;
    switch (*EntryPointer) {
    case GnbEntryWr:
      GnbRegisterAccessProtocol->Write (
        GnbHandle,
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
        GnbRegisterAccessProtocol->Write (
          GnbHandle,
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
          GnbRegisterAccessProtocol->Write (
            GnbHandle,
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
      GnbProcessTableRegisterRmw (
        GnbHandle,
        GnbRegisterAccessProtocol,
        &((GNB_TABLE_ENTRY_RMW *) EntryPointer)->Data,
        WriteAccFlags,
        StdHeader
        );
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_RMW);
      break;
    case GnbEntryPropertyRmw:
      if ((Property & ((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Property) != 0) {
        GnbProcessTableRegisterRmw (
          GnbHandle,
          GnbRegisterAccessProtocol,
          &((GNB_TABLE_ENTRY_PROPERTY_RMW *) EntryPointer)->Data,
          WriteAccFlags,
          StdHeader
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_PROPERTY_RMW);
      break;
    case GnbEntryRevRmw:
      if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_REV_RMW *) EntryPointer)->Revision) != 0) {
        GnbProcessTableRegisterRmw (
          GnbHandle,
          GnbRegisterAccessProtocol,
          &((GNB_TABLE_ENTRY_REV_RMW *) EntryPointer)->Data,
          WriteAccFlags,
          StdHeader
          );
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_REV_RMW);
      break;
    case GnbEntryFullRmw:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Property) != 0) {
        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Revision) != 0) {
          GnbProcessTableRegisterRmw (
            GnbHandle,
            GnbRegisterAccessProtocol,
            &((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Data,
            WriteAccFlags,
            StdHeader
            );
        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_RMW);
      break;
    case GnbEntryPoll:
      do {
        GnbRegisterAccessProtocol->Read (
          GnbHandle,
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
          GnbRegisterAccessProtocol->Read (
            GnbHandle,
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
    case GnbEntryFullPoll:
      if ((Property & ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->Property) != 0) {
        if ((LogicalId.Revision & ((GNB_TABLE_ENTRY_FULL_RMW *) EntryPointer)->Revision) != 0) {
          do {
            GnbRegisterAccessProtocol->Read (
              GnbHandle,
              ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->RegisterSpaceType,
              ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->Address,
              &Data,
              0,
              StdHeader
              );
          } while ((Data & ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->AndMask) != ((GNB_TABLE_ENTRY_FULL_POLL *) EntryPointer)->CompareValue);
        }
      }
      EntryPointer = EntryPointer + sizeof (GNB_TABLE_ENTRY_FULL_POLL);
      break;
    case GnbEntryCopy:
      GnbRegisterAccessProtocol->Read (
        GnbHandle,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcAddress,
        &Data,
        0,
        StdHeader
        );
      Mask = (1ull << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldWidth) - 1;
      Data = (Data >> ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->SrcFieldOffset) & Mask;
      GnbRegisterAccessProtocol->Read (
        GnbHandle,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestRegisterSpaceType,
        ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestAddress,
        &Temp,
        0,
        StdHeader
        );
      Mask = (1ull << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldWidth) - 1;
      Temp = Temp & ( ~ (Mask << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset));
      Temp = Temp | ((Data & Mask) << ((GNB_TABLE_ENTRY_COPY*) EntryPointer)->DestFieldOffset);
      GnbRegisterAccessProtocol->Write (
        GnbHandle,
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

/*----------------------------------------------------------------------------------------*/
/**
 * Supporting function for register read modify write
 *
 * @param[in] GnbHandle                 Gnb handle
 * @param[in] GnbRegisterAccessProtocol Register access protocol
 * @param[in] Data                      Data pointer
 * @param[in] Flags                     Flags
 * @param[in] StdHeader                 Standard configuration header
 */

VOID
STATIC
GnbProcessTableRegisterRmw (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_REGISTER_SERVICE  *GnbRegisterAccessProtocol,
  IN      GNB_RMW_BLOCK         *Data,
  IN      UINT32                Flags,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT64  Value;
  Value = 0;
  GnbRegisterAccessProtocol->Read (
    GnbHandle,
    Data->RegisterSpaceType,
    Data->Address,
    &Value,
    0,
    StdHeader
    );
  Value = (Value & (~ (UINT64) Data->AndMask)) | Data->OrMask;
  GnbRegisterAccessProtocol->Write (
    GnbHandle,
    Data->RegisterSpaceType,
    Data->Address,
    &Value,
    Flags,
    StdHeader
    );
}
