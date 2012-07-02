/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63552 $   @e \$Date: 2011-12-26 19:46:05 -0600 (Mon, 26 Dec 2011) $
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
#include "heapManager.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "IdsDebugPrint.h"
#include "IdsDpHdtout.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSDEBUG_FILECODE

/*--------------------------------------------------------------------------------------*/
/**
 *  IDS back-end code for AGESA_TESTPOINT
 *
 *  @param[in] TestPoint  Progress indicator value, see @ref AGESA_TP
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
IdsAgesaTestPoint (
  IN       AGESA_TP      TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth8, IDS_DEBUG_PORT, &TestPoint, StdHeader);
}

/**
 *
 *  HDT out Function for Assert.
 *
 *  fire a HDTOUT Command of assert to let hdtout script do corresponding things.
 *
 *  @param[in,out] FileCode    FileCode of the line
 *
 **/
#if 0
VOID
AmdIdsDebugPrintAssert (
  IN      UINT32 FileCode
  )
{
  //TODO:if (AmdIdsHdtOutSupport ()) {
  //  IdsOutPort (HDTOUT_ASSERT, FileCode, 0);
  //}
}
#endif

/**
 *  IDS Backend Function for ASSERT
 *
 * Halt execution with stop code display.  Stop Code is displayed on port 80, with rotation so that
 * it is visible on 8, 16, or 32 bit display.  The stop code is alternated with 0xDEAD on the display,
 * to help distinguish the stop code from a post code loop.
 * Additional features may be available if using simulation.
 *
 * @param[in]     FileCode    File code(define in FILECODE.h) mix with assert Line num.
 *
 * @retval         TRUE     No error
 **/
BOOLEAN
IdsAssert (
  IN      UINT32 FileCode
  )
{
  UINT32 file;
  UINT32 line;

  file = (FileCode >> 16);
  line = (FileCode & 0xFFFF);
  IDS_HDT_CONSOLE (MAIN_FLOW, "ASSERT on File[%x] Line[%x]\n", (UINTN) file, (UINTN) line);
  IDS_HDT_CONSOLE_FLUSH_BUFFER (NULL);
  IDS_HDT_CONSOLE_ASSERT (FileCode);
  //IdsErrorStop (FileCode);
  return TRUE;
}

/**
 *  IDS Backend Function for Memory timeout control
 *
 *  This function is used to override Memory timeout control.
 *
 *  @param[in,out]   DataPtr      The Pointer of UINT8.
 *
 **/
VOID
IdsMemTimeOut (
  IN OUT   VOID *DataPtr
  )
{
//TODO:  UINTN i;
//
//  IDS_DEBUG_PRINT **DebugPrintList;
//
//  GetDebugPrintList (&DebugPrintList);
//
//  for (i = 0; DebugPrintList[i] != NULL; i++) {
//    if (DebugPrintList[i]->support ()) {
//      // Turn timeout off if any Debug service is on
//      *((UINT8 *)DataPtr) = (UINT8)0;
//    }
//  }
}

/**
 *
 *  IDS Debug Function to check the sentinels are intact
 *
 * This function complete heap walk and check to be performed at any time.
 *
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         TRUE     No error
 *
 **/
BOOLEAN
AmdHeapIntactCheck (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return TRUE;
}

/**
 * Check for CAR Corruption, the performance monitor number three programed to log the CAR Corruption.
 * Check to see if control register is enabled and then check the preformance counter and stop the system by executing
 * IDS_ERROR_TRAP if counter has any value other than zero.
 *
 *  @param[in,out] StdHeader    The Pointer of Standard Header.
 *
 *
 **/
VOID
IdsCarCorruptionCheck (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{

  UINT64 Perf_Msr;

  LibAmdMsrRead (MSR_PERF_CONTROL3, (UINT64*)&Perf_Msr, StdHeader);
  if ((Perf_Msr & PERF_RESERVE_BIT_MASK) == PERF_CAR_CORRUPTION_EVENT) {
    LibAmdMsrRead (MSR_PERF_COUNTER3, (UINT64*)&Perf_Msr, StdHeader);
    if ((Perf_Msr != 0)) {
      IDS_ERROR_TRAP;
    }
  }
}


