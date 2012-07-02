/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
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
 ******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "IdsLib.h"
#include "amdlib.h"
#include "AMD.h"
#include "IdsDebugPrint.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSDPSERIAL_FILECODE

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
STATIC BOOLEAN
AmdIdsSerialSupport (
  VOID
  )
{

  return TRUE;
}

/**
 *  Get Serial customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
STATIC BOOLEAN
AmdIdsSerialGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}


#define IDS_SERIAL_PORT_LSR (IDS_SERIAL_PORT + 5)
#define IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK BIT5
/**
 *  Send byte to  Serial Port
 *
 *  Before use this routine, please make sure Serial Communications Chip have been initialed
 *
 *  @param[in] ByteSended   Byte to be sended
 *
 *  @retval       TRUE    Byte sended successfully
 *  @retval       FALSE   Byte sended failed
 *
 **/
STATIC BOOLEAN
AmdIdsSerialSendByte (
  IN      CHAR8   ByteSended
  )
{
  UINT32 RetryCount;
  UINT8 Value;

  //Wait until LSR.Bit5 (Transmitter holding register Empty)
  RetryCount = 200;
  do {
    LibAmdIoRead (AccessWidth8, IDS_SERIAL_PORT_LSR, &Value, NULL);
    RetryCount--;
  } while (((Value & IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK) == 0) &&
           (RetryCount > 0));

  if (RetryCount == 0) {
    //Time expired
    return FALSE;
  } else {
    LibAmdIoWrite (AccessWidth8, IDS_SERIAL_PORT, &ByteSended, NULL);
    return TRUE;
  }
}


/**
 *  Print formated string
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] debugPrintPrivate  - Option
 *
**/
STATIC VOID
AmdIdsSerialPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
  BOOLEAN SendStatus;
  UINT32 RetryCount;
  RetryCount = 200;
  while (BufferSize--) {
    do {
      if (*Buffer == '\n') {
        SendStatus = AmdIdsSerialSendByte ('\r');
      }
      SendStatus = AmdIdsSerialSendByte (*Buffer);
      RetryCount--;
    } while ((SendStatus == FALSE) && (RetryCount > 0));
    Buffer ++;
  }
}

/**
 *  Init local private data
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] debugPrintPrivate  - Point to debugPrintPrivate
 *
**/
STATIC VOID
AmdIdsSerialInitPrivateData (
  IN      UINT64 Flag,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{

}

CONST IDS_DEBUG_PRINT ROMDATA  IdsDebugPrintSerialInstance =
{
  AmdIdsSerialSupport,
  AmdIdsSerialGetFilter,
  AmdIdsSerialInitPrivateData,
  AmdIdsSerialPrint
};



