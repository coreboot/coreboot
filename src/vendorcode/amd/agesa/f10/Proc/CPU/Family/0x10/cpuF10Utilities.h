/**
 * @file
 *
 * AMD Family_10 specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_F10_UTILITES_H_
#define _CPU_F10_UTILITES_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// The structure for Software Initiated NB Voltage Transitions
typedef struct {
  UINT32  VidCode;          ///< VID code to transition to
  BOOLEAN SlamMode;         ///< Whether voltage is to be slammed, or stepped
} SW_VOLT_TRANS_NB;


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
VOID
F10PmSwVoltageTransition (
  IN       UINT32 VidCode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F10PmSwVoltageTransitionServerNb (
  IN       UINT32 VidCode,
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
F10SwVoltageTransitionServerNbCore (
  IN       VOID  *InputData,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F10WaitOutVoltageTransition (
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F10GetCurrentVsTimeInUsecs (
     OUT   UINT32 *VsTimeUsecs,
  IN       BOOLEAN SlamMode,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F10ProgramVSSlamTimeOnSocket (
  IN       PCI_ADDR             *PciAddress,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

UINT32
F10GetSlamTimeEncoding (
  IN       UINT8                HighVoltageVid,
  IN       UINT8                LowVoltageVid,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       CONST UINT32         *SlamTimeTable,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

AGESA_STATUS
F10GetPstatePower (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *PowerInMw,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10GetPstateFrequency (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10TransitionPstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10GetTscRate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
F10LaunchApCore (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT32 SocketNum,
  IN       UINT32 ModuleNum,
  IN       UINT32 CoreNum,
  IN       UINT32 PrimaryCoreNum,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10GetFrequencyXlatRegInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT8  PStateNumber,
  IN       UINT32 Frequency,
     OUT   UINT32               *CpuFidPtr,
     OUT   UINT32               *CpuDidPtr1,
     OUT   UINT32               *CpuDidPtr2,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F10PstateLevelingCoreMsrModify (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       S_CPU_AMD_PSTATE   *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

AGESA_STATUS
F10GetPstateTransLatency (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PSTATE_LEVELING        *PStateLevelingBufferStructPtr,
  IN       PCI_ADDR               *PciAddress,
     OUT   UINT32                 *TransitionLatency,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
F10GetPstateRegisterInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32              PState,
     OUT   BOOLEAN             *PStateEnabled,
  IN OUT   UINT32              *IddVal,
  IN OUT   UINT32              *IddDiv,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
F10GetPstateMaxState (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   UINT32              *MaxPStateNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
F10GetPlatformTypeSpecificInfo (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PLATFORM_FEATS         *Features,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

#endif  // _CPU_F10_UTILITES_H_
