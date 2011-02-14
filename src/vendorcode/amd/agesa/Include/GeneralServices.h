/* $NoKeywords:$ */
/**
 * @file
 *
 * General Services
 *
 * Provides Services similar to the external General Services API, except
 * suited to use within AGESA components.  Socket, Core and PCI identification.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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

#ifndef _GENERAL_SERVICES_H_
#define _GENERAL_SERVICES_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NUMBER_OF_EVENT_DATA_PARAMS 4

/**
 * AMD Device id for MMIO check.
 */
#define AMD_DEV_VEN_ID 0x1022
#define AMD_DEV_VEN_ID_ADDRESS 0

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

/**
 * An AGESA Event Log entry.
 */
typedef struct {
  AGESA_STATUS EventClass;   ///< The severity of the event, its associated AGESA_STATUS.
  UINT32   EventInfo;        ///< Uniquely identifies the event.
  UINT32   DataParam1;       ///< Event specific additional data
  UINT32   DataParam2;       ///< Event specific additional data
  UINT32   DataParam3;       ///< Event specific additional data
  UINT32   DataParam4;       ///< Event specific additional data
} AGESA_EVENT;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

/**
 * Get a specified Core's APIC ID.
 *
 * @param[in]    StdHeader    Header for library and services.
 * @param[in]    Socket       The Core's Socket.
 * @param[in]    Core         The Core id.
 * @param[out]   ApicAddress  The Core's APIC ID.
 * @param[out]   AgesaStatus  Aggregates AGESA_STATUS for external interface, Always Succeeds.
 *
 * @retval       TRUE         The core is present, APIC Id valid
 * @retval       FALSE        The core is not present, APIC Id not valid.
 */
BOOLEAN
GetApicId (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Core,
     OUT   UINT8             *ApicAddress,
     OUT   AGESA_STATUS      *AgesaStatus
);

/**
 * Get Processor Module's PCI Config Space address.
 *
 * @param[in]    StdHeader    Header for library and services.
 * @param[in]    Socket       The Core's Socket.
 * @param[in]    Module       The Module in that Processor
 * @param[out]   PciAddress   The Processor's PCI Config Space address (Function 0, Register 0)
 * @param[out]   AgesaStatus  Aggregates AGESA_STATUS for external interface, Always Succeeds.
 *
 * @retval       TRUE         The core is present, PCI Address valid
 * @retval       FALSE        The core is not present, PCI Address not valid.
 */
BOOLEAN
GetPciAddress (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Module,
     OUT   PCI_ADDR          *PciAddress,
     OUT   AGESA_STATUS      *AgesaStatus
);

/**
 * "Who am I" for the current running core.
 *
 * @param[in]    StdHeader    Header for library and services.
 * @param[out]   Socket       The current Core's Socket
 * @param[out]   Module       The current Core's Processor Module
 * @param[out]   Core         The current Core's core id.
 * @param[out]   AgesaStatus  Aggregates AGESA_STATUS for external interface, Always Succeeds.
 *
 */
VOID
IdentifyCore (
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   UINT32            *Socket,
     OUT   UINT32            *Module,
     OUT   UINT32            *Core,
     OUT   AGESA_STATUS      *AgesaStatus
);

/**
 *  A boolean function determine executed CPU is BSP core.
 */
BOOLEAN
IsBsp (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
     OUT   AGESA_STATUS      *AgesaStatus
  );

/**
 * This function logs AGESA events into the event log.
 */
VOID
PutEventLog (
  IN       AGESA_STATUS EventClass,
  IN       UINT32 EventInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function gets event logs from the circular buffer.
 */
AGESA_STATUS
GetEventLog (
     OUT   AGESA_EVENT *EventRecord,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * This function gets event logs from the circular buffer without flushing the entry.
 */
BOOLEAN
PeekEventLog (
     OUT   AGESA_EVENT *EventRecord,
  IN       UINT16 Index,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * This routine programs the registers necessary to get the PCI MMIO mechanism
 * up and functioning.
 */
VOID
InitializePciMmio (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  // _GENERAL_SERVICES_H_
