
/**
 * @file
 *
 *
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _NBEVENTLOG_H_
#define _NBEVENTLOG_H_

#pragma pack(push, 1)

/**
 * Event
 *
 */
typedef struct {
  UINTN    EventClass;     /**< Event Class.
                            *  @li <b>AGESA_WARNING</b>
                            *  @li <b>AGESA_ERROR</b>
                            *  @li <b>AGESA_FATAL</b>
                           */
  UINT32   EventInfo;     /**< Event Info.
                           *   This parameter used as event identifier
                           */
  UINT32   DataParam1;    ///< Event specific data
  UINT32   DataParam2;    ///< Event specific data
  UINT32   DataParam3;    ///< Event specific data
  UINT32   DataParam4;    ///< Event specific data
} AGESA_EVENT;


#define PCIE_ERROR_HOTPLUG_INIT                 0x20010100
#define PCIE_ERROR_TRAINING_FAIL                0x20010200
#define PCIE_ERROR_CORE_CONFIGURATION           0x20010300
#define PCIE_ERROR_BROKEN_LINE                  0x20010400
#define PCIE_ERROR_GEN2_FAIL                    0x20010500
#define PCIE_ERROR_VCO_NEGOTIATON               0x20010600
#define PCIE_ERROR_DEVICE_REMAP                 0x20010700
#define GENERAL_ERROR_BAD_CONFIGURATION         0x20000100
#define GENERAL_ERROR_NB_NOT_PRESENT            0x20000200
#define GENERAL_ERROR_LOCATE_ACPI_TABLE         0x20000300


VOID
LibNbEventLog (
  IN      UINTN         EventClass,
  IN      UINT32        EventInfo,
  IN      UINT32        DataParam1,
  IN      UINT32        DataParam2,
  IN      UINT32        DataParam3,
  IN      UINT32        DataParam4,
  IN      AMD_NB_CONFIG *NbConfigPtr
  );

#pragma pack(pop)

#endif