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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
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
