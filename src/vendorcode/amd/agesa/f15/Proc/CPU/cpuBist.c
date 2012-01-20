/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BIST Status Check Implementation.
 *
 * Implement CPU BIST Status checking
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
 *
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuApicUtilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_CPUBIST_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT32
STATIC
GetBistResults (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

 /*----------------------------------------------------------------------------------------
  *                          E X P O R T E D    F U N C T I O N S
  *----------------------------------------------------------------------------------------
 */

 /*---------------------------------------------------------------------------------------*/
 /**
  *
  * This function checks the status of BIST and places the error status in the event log
  * if there are any errors
  *
  * @param[in]      StdHeader              Header for library and services
  *
  * @retval         AGESA_SUCCESS          No BIST errors have been logged.
  * @retval         AGESA_ALERT            BIST errors have been detected and added to the
  *                                        event log.
  */
AGESA_STATUS
CheckBistStatus (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32                Socket;
  UINT32                Core;
  UINT32                BscSocket;
  UINT32                BscCoreNum;
  UINT32                NumberOfSockets;
  UINT32                NumberOfCores;
  UINT32                Ignored;
  UINT32                ReturnCode;
  AGESA_STATUS          IgnoredSts;
  AGESA_STATUS          AgesaStatus;
  AP_TASK               TaskPtr;

  // Make sure that Standard Header is valid
  ASSERT (StdHeader != NULL);
  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  AgesaStatus = AGESA_SUCCESS;

  // Get the BscSocket, BscCoreNum and NumberOfSockets in the system
  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  // Setup TaskPtr struct to execute routine on APs
  TaskPtr.FuncAddress.PfApTaskO = GetBistResults;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = TASK_HAS_OUTPUT | WAIT_FOR_CORE;

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != BscSocket) || (Core != BscCoreNum)) {
          ReturnCode = ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)Core, &TaskPtr, StdHeader);
        } else {
          ReturnCode = TaskPtr.FuncAddress.PfApTaskO (StdHeader);
        }

        // If BIST value is non-zero, add to BSP's event log
        if (ReturnCode != 0) {
          IDS_HDT_CONSOLE (CPU_TRACE, "  BIST failure: socket %d core %d, status = 0x%x\n", Socket, Core, ReturnCode);
          AgesaStatus = AGESA_ALERT;
          PutEventLog (AGESA_ALERT,
                       CPU_EVENT_BIST_ERROR,
                       ReturnCode, Socket, Core, 0, StdHeader);
        }
      }
    }
  }

  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L          F U N C T I O N S
 *----------------------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Reads the lower 32 bits of the BIST register
 *
 * @param[in]      StdHeader              Header for library and services
 *
 * @retval         Value of the BIST register
*/
UINT32
STATIC
GetBistResults (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64   BistResults;

  // Read MSRC001_0060 BIST Results Register
  LibAmdMsrRead (MSR_BIST, &BistResults, StdHeader);

  return (UINT32) (BistResults & 0xFFFFFFFF);
}
