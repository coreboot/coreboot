/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement External, AGESA Common, and CPU component General Services.
 *
 * Contains implementation of the interfaces: General Services API in AGESA.h,
 * GeneralServices.h, and cpuServices.h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "Options.h"
#include "Topology.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuServices.h"
#include "heapManager.h"
#include "cpuApicUtilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUGENERALSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTIONS_CONFIG_TOPOLOGY TopologyConfiguration;
extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *            E X P O R T E D    F U N C T I O N S - External General Services API
 *----------------------------------------------------------------------------------------
 */

/**
 * Get a specified Core's APIC ID.
 *
 * Invoke corresponding Cpu Service for external user.
 *
 * @param[in,out]    AmdParamApic    Our interface struct
 *
 * @return         The most severe status of any called service.
 */
AGESA_STATUS
AmdGetApicId (
  IN OUT   AMD_APIC_PARAMS *AmdParamApic
  )
{
  AGESA_STATUS AgesaStatus;

  AGESA_TESTPOINT (TpIfAmdGetApicIdEntry, &AmdParamApic->StdHeader);
  AmdParamApic->StdHeader.HeapBasePtr = HeapGetBaseAddress (&AmdParamApic->StdHeader);

  AmdParamApic->IsPresent = GetApicId (
    &AmdParamApic->StdHeader,
    AmdParamApic->Socket,
    AmdParamApic->Core,
    &AmdParamApic->ApicAddress,
    &AgesaStatus
    );

  AGESA_TESTPOINT (TpIfAmdGetApicIdExit, &AmdParamApic->StdHeader);
  return AgesaStatus;
}

/**
 * Get Processor Module's PCI Config Space address.
 *
 * Invoke corresponding Cpu Service for external user.
 *
 * @param[in,out]    AmdParamGetPci    Our interface struct
 *
 * @return         The most severe status of any called service.
 */
AGESA_STATUS
AmdGetPciAddress (
  IN OUT   AMD_GET_PCI_PARAMS *AmdParamGetPci
  )
{
  AGESA_STATUS AgesaStatus;

  AGESA_TESTPOINT (TpIfAmdGetPciAddressEntry, &AmdParamGetPci->StdHeader);
  AmdParamGetPci->StdHeader.HeapBasePtr = HeapGetBaseAddress (&AmdParamGetPci->StdHeader);

  AmdParamGetPci->IsPresent = GetPciAddress (
    &AmdParamGetPci->StdHeader,
    AmdParamGetPci->Socket,
    AmdParamGetPci->Module,
    &AmdParamGetPci->PciAddress,
    &AgesaStatus
    );

  AGESA_TESTPOINT (TpIfAmdGetPciAddressExit, &AmdParamGetPci->StdHeader);
  return AgesaStatus;
}

/**
 * "Who am I" for the current running core.
 *
 * Invoke corresponding Cpu Service for external user.
 *
 * @param[in,out]    AmdParamIdentify    Our interface struct
 *
 * @return         The most severe status of any called service.
 */
AGESA_STATUS
AmdIdentifyCore (
  IN OUT   AMD_IDENTIFY_PARAMS *AmdParamIdentify
  )
{
  AGESA_STATUS AgesaStatus;
  UINT32 Socket;
  UINT32 Module;
  UINT32 Core;

  AGESA_TESTPOINT (TpIfAmdIdentifyCoreEntry, &AmdParamIdentify->StdHeader);
  AmdParamIdentify->StdHeader.HeapBasePtr = HeapGetBaseAddress (&AmdParamIdentify->StdHeader);

  IdentifyCore (
    &AmdParamIdentify->StdHeader,
    &Socket,
    &Module,
    &Core,
    &AgesaStatus
    );
  AmdParamIdentify->Socket = (UINT8)Socket;
  AmdParamIdentify->Module = (UINT8)Module;
  AmdParamIdentify->Core = (UINT8)Core;

  AGESA_TESTPOINT (TpIfAmdIdentifyCoreExit, &AmdParamIdentify->StdHeader);
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------
 *            E X P O R T E D    F U N C T I O N S - AGESA common General Services
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Get a specified Core's APIC ID.
 *
 * Code sync: This calculation MUST match the assignment
 *    calculation done in LocalApicInitializationAtEarly function.
 *
 * @param[in]    StdHeader    Header for library and services.
 * @param[in]    Socket       The socket in which the Core's Processor is installed.
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
  )
{
  BOOLEAN ReturnValue;
  UINT32  CoreCount;
  UINT32  ApicID;

  ReturnValue = FALSE;
  if (GetActiveCoresInGivenSocket (Socket, &CoreCount, StdHeader)) {
    if (Core < CoreCount) {
      ReturnValue = TRUE;
      GetLocalApicIdForCore (Socket, Core, &ApicID, StdHeader);
      *ApicAddress = (UINT8) ApicID;
    }
  }

  // Always Succeeds.
  *AgesaStatus = AGESA_SUCCESS;

  return ReturnValue;
}

/*---------------------------------------------------------------------------------------*/
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
  )
{
  UINT8        Node;
  BOOLEAN      Result;

  ASSERT (Socket < MAX_SOCKETS);
  ASSERT (Module < MAX_DIES);

  Result = TRUE;
  // Always Succeeds.
  *AgesaStatus = AGESA_SUCCESS;

  if (GetNodeId (Socket, Module, &Node, StdHeader)) {
    // socket is populated
    PciAddress->AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    PciAddress->Address.Device = PciAddress->Address.Device + Node;
  } else {
    // socket is not populated
    PciAddress->AddressValue = ILLEGAL_SBDFO;
    Result = FALSE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
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
  )
{
  AP_MAIL_INFO ApMailboxInfo;
  UINT32 CurrentCore;

  // Always Succeeds.
  *AgesaStatus = AGESA_SUCCESS;

  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  ASSERT (ApMailboxInfo.Fields.Socket < MAX_SOCKETS);
  ASSERT (ApMailboxInfo.Fields.Module < MAX_DIES);
  *Socket = (UINT8)ApMailboxInfo.Fields.Socket;
  *Module = (UINT8)ApMailboxInfo.Fields.Module;

  // Get Core Id
  GetCurrentCore (&CurrentCore, StdHeader);
  *Core = (UINT8)CurrentCore;
}


/*----------------------------------------------------------------------------------------
 *            E X P O R T E D    F U N C T I O N S - cpu component General Services
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Get the current Platform's number of Sockets, regardless of how many are populated.
 *
 * The Options component can provide how many sockets are available in system.
 * This can be used to avoid testing presence of Processors in Sockets which don't exist.
 * The result can be one socket to the maximum possible sockets of any supported processor family.
 * You cannot assume that all sockets contain a processor or that the sockets have processors
 * installed in any particular order.  Do not convert this number to a number of nodes.
 *
 * @return   The number of available sockets for the platform.
 *
 */
UINT32
GetPlatformNumberOfSockets ()
{
  return TopologyConfiguration.PlatformNumberOfSockets;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the number of Modules to check presence in each Processor.
 *
 * The Options component can provide how many modules need to be check for presence in each
 * processor, regardless whether all, or any, processor have that many modules present on this boot.
 * The result can be one module to the maximum possible modules of any supported processor family.
 * You cannot assume that Modules are in any particular order, especially with respect to node id.
 *
 * @return   The maximum number of modules in each processor.
 *
 */
UINT32
GetPlatformNumberOfModules ()
{
  return TopologyConfiguration.PlatformNumberOfModules;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Is a processor present in Socket?
 *
 * Check to see if any possible module of the processor is present.  This provides
 * support for a few cases where a PCI address isn't needed, but code still needs to
 * iterate by Socket.
 *
 * @param[in]   Socket  The socket which is being tested
 * @param[in]   StdHeader    Header for library and services.
 *
 * @retval      TRUE    The socket has a processor installed
 * @retval      FALSE   The socket is empty (or the processor is dead).
 *
 */
BOOLEAN
IsProcessorPresent (
  IN       UINT32             Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  SOCKET_DIE_TO_NODE_MAP  pSocketDieMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  BOOLEAN Result;
  UINT32 Module;
  AGESA_STATUS Status;

  ASSERT (Socket < MAX_SOCKETS);
  Result = FALSE;
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;

  //  Get data block from heap
  Status = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (Status == AGESA_SUCCESS));
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if ((*pSocketDieMap)[Socket][Module].Node != 0xFF) {
      Result = TRUE;
      break;
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Provide the number of installed processors (not Nodes! and not Sockets!)
 *
 * Iterate over the Socket, Module to Node Map, counting the number of present nodes.
 * Do not use this as a Node Count!  Do not use this as the number of Sockets!  (This
 * is for APIC ID utilities.)
 *
 * @param[in]   StdHeader    Header for library and services.
 *
 * @return      the number of processors installed
 *
 */
UINT32
GetNumberOfProcessors (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  SOCKET_DIE_TO_NODE_MAP  pSocketDieMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  UINT32 Result;
  UINT32 Socket;
  UINT32 Module;
  AGESA_STATUS Status;

  Result = 0;
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;

  //  Get data block from heap
  Status = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (Status == AGESA_SUCCESS));
  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
      if ((*pSocketDieMap)[Socket][Module].Node != 0xFF) {
        Result++;
        break;
      }
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * For a specific Node, get its Socket and Module ids.
 *
 * If asking for the current running Node, read the mailbox socket, module. Specific Node,
 * locate the Node to Socket/Module Map in heap, and return the ids, if present.
 *
 * @param[in]    Node         What Socket and Module is this Node?
 * @param[out]   Socket       The Socket containing that Node.
 * @param[out]   Module       The Processor Module of that Node.
 * @param[in]    StdHeader    Header for library and services.
 *
 * @retval TRUE Node is present, Socket, Module are valid.
 * @retval FALSE Node is not present, why do you ask?
 */
BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
     OUT   UINT32    *Socket,
     OUT   UINT32    *Module,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  NODE_TO_SOCKET_DIE_MAP  pNodeMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  BOOLEAN Result;
  AGESA_STATUS Status;

  Result = FALSE;

  ASSERT (Node < MAX_NODES);

  //  Get Map from heap
  SocketDieHeapDataBlock.BufferHandle = NODE_ID_MAP_HANDLE;
  Status = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pNodeMap = (NODE_TO_SOCKET_DIE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pNodeMap != NULL) && (Status == AGESA_SUCCESS));
  *Socket = (*pNodeMap)[Node].Socket;
  *Module = (*pNodeMap)[Node].Die;
  if ((*pNodeMap)[Node].Socket != 0xFF) {
    Result = TRUE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the current core's Processor APIC Index.
 *
 * The Processor APIC Index is the position of the current processor in the APIC id
 * assignment.  Processors are ordered in node id order.  This is not the same, however,
 * as the node id of the current socket and module or the current socket id.
 *
 * @param[in]    Node      The current desired core's node id (usually the current core).
 * @param[in]    StdHeader Header for library and services.
 *
 * @return  Processor APIC Index
 *
 */
UINT32
GetProcessorApicIndex (
  IN       UINT32            Node,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ProcessorApicIndex;
  UINT32 PreviousSocket;
  UINT32 CurrentSocket;
  UINT32 Ignored;
  UINT32 i;

  ASSERT (Node < MAX_NODES);

  // Calculate total APIC devices up to Current Node, Core.
  ProcessorApicIndex = 0;
  PreviousSocket = 0xFF;
  for (i = 0; i < (Node + 1); i++) {
    GetSocketModuleOfNode (i, &CurrentSocket, &Ignored, StdHeader);
    if (CurrentSocket != PreviousSocket) {
      ProcessorApicIndex++;
      PreviousSocket = CurrentSocket;
    }
  }
  // Convert to Index (zero based) from count (one based).
  ProcessorApicIndex--;
  return ProcessorApicIndex;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns current node number
 *
 *  @param[out]      Node       This Core's Node id
 *  @param[in]        StdHeader  Header for library and services.
 *
 */
VOID
GetCurrentNodeNum (
     OUT   UINT32 *Node,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_MAIL_INFO ApMailboxInfo;

  // Get the Node Id from the Mailbox.
  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  ASSERT (ApMailboxInfo.Fields.Node < MAX_NODES);
  *Node = ApMailboxInfo.Fields.Node;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Writes to all nodes on the executing core's socket.
 *
 *  @param[in]     PciAddress    The Function and Register to update
 *  @param[in]     Mask          The bitwise AND mask to apply to the current register value
 *  @param[in]     Data          The bitwise OR mask to apply to the current register value
 *  @param[in]     StdHeader     Header for library and services.
 *
 */
VOID
ModifyCurrentSocketPci (
  IN       PCI_ADDR          *PciAddress,
  IN       UINT32            Mask,
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Socket;
  UINT32 Module;
  UINT32 Core;
  UINT32 LocalPciRegister;
  AGESA_STATUS AgesaStatus;
  PCI_ADDR Reg;

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &AgesaStatus);

  for (Module = 0; Module < (UINT8)GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &Reg, &AgesaStatus)) {
      Reg.Address.Function = PciAddress->Address.Function;
      Reg.Address.Register = PciAddress->Address.Register;
      LibAmdPciRead (AccessWidth32, Reg, &LocalPciRegister, StdHeader);
      LocalPciRegister &= Mask;
      LocalPciRegister |= Data;
      LibAmdPciWrite (AccessWidth32, Reg, &LocalPciRegister, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns Total number of active cores in the current socket
 *
 * @param[out]     CoreCount    The cores in this processor.
 * @param[in]      StdHeader    Header for library and services.
 *
 */
VOID
GetActiveCoresInCurrentSocket (
     OUT   UINT32 *CoreCount,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPUID_DATA  CpuidDataStruct;
  UINT32 TotalCoresCount;

  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuidDataStruct, StdHeader);
  TotalCoresCount = (CpuidDataStruct.ECX_Reg & 0x000000FF) + 1;
  *CoreCount = TotalCoresCount;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Provides the Total number of active cores in the current core's node.
 *
 * @param[in]      StdHeader   Header for library and services.
 *
 * @return         The current node core count
 */
UINTN
GetActiveCoresInCurrentModule (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32 Socket;
  UINT32 Module;
  UINT32 Core;
  UINT32 LowCore;
  UINT32 HighCore;
  UINT32 ProcessorCoreCount;
  AGESA_STATUS AgesaStatus;

  ProcessorCoreCount = 0;

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &AgesaStatus);
  if (GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader)) {
    ProcessorCoreCount = ((HighCore - LowCore) + 1);
  }
  return ProcessorCoreCount;
}

/**
 * Provide the number of compute units on current module.
 *
 *
 * @param[in]   StdHeader    Header for library and services.
 *
 * @return      The current compute unit counts.
 *
 */
UINTN
GetNumberOfCompUnitsInCurrentModule (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32                 Socket;
  UINT32                 Module;
  UINT32                 CurrentCore;
  UINT32                 ComputeUnitCount;
  UINT32                 Enabled;
  AGESA_STATUS           IgnoredSts;
  LOCATE_HEAP_PTR        SocketDieHeapDataBlock;
  SOCKET_DIE_TO_NODE_MAP pSocketDieMap;

  ComputeUnitCount = 0;

  ASSERT ((GetComputeUnitMapping (StdHeader) == AllCoresMapping) ||
          (GetComputeUnitMapping (StdHeader) == EvenCoresMapping));

  IdentifyCore (StdHeader, &Socket, &Module, &CurrentCore, &IgnoredSts);
  //  Get data block from heap
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;
  IgnoredSts = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (IgnoredSts == AGESA_SUCCESS));
  // Current Core's socket, module must be present.
  ASSERT ((*pSocketDieMap)[Socket][Module].Node != 0xFF);
  // Process compute unit info
  Enabled = (*pSocketDieMap)[Socket][Module].EnabledComputeUnits;

  while (Enabled > 0) {
    if ((Enabled & 0x1) != 0) {
      ComputeUnitCount++;
    }
    Enabled >>= 1;
  }

  return ComputeUnitCount;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Provides the Total number of active cores in the given socket.
 *
 * @param[in]      Socket      Get a core count for the processor in this socket.
 * @param[out]     CoreCount   Its core count
 * @param[in]      StdHeader   Header for library and services.
 *
 * @retval     TRUE    A processor is present in the Socket and the CoreCount is valid.
 * @retval     FALSE   The Socket does not have a Processor
 */
BOOLEAN
GetActiveCoresInGivenSocket (
  IN       UINT32             Socket,
     OUT   UINT32             *CoreCount,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32 Module;
  UINT32 LowCore;
  UINT32 HighCore;
  UINT32 ProcessorCoreCount;
  BOOLEAN Result;

  Result = FALSE;
  ProcessorCoreCount = 0;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader)) {
      ProcessorCoreCount = ProcessorCoreCount + ((HighCore - LowCore) + 1);
      Result = TRUE;
    } else {
      break;
    }
  }
  *CoreCount = ProcessorCoreCount;
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Provides the range of Cores in a Processor which are in a Module.
 *
 * Cores are named uniquely in a processor, 0 to TotalCores.  Any module in the processor has
 * a set of those cores, named from LowCore to HighCore.
 *
 * @param[in]      Socket      Get a core range for the processor in this socket.
 * @param[in]      Module      Get a core range for this Module in the processor.
 * @param[out]     LowCore     The lowest Processor Core in the Module.
 * @param[out]     HighCore    The highest Processor Core in the Module.
 * @param[in]      StdHeader   Header for library and services.
 *
 * @retval     TRUE    A processor is present in the Socket and the Core Range is valid.
 * @retval     FALSE   The Socket does not have a Processor
 */
BOOLEAN
GetGivenModuleCoreRange (
  IN       UINT32            Socket,
  IN       UINT32            Module,
     OUT   UINT32            *LowCore,
     OUT   UINT32            *HighCore,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  SOCKET_DIE_TO_NODE_MAP  pSocketDieMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  BOOLEAN Result;
  AGESA_STATUS Status;

  ASSERT (Socket < MAX_SOCKETS);
  ASSERT (Module < MAX_DIES);
  Result = FALSE;
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;

  //  Get data block from heap
  Status = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (Status == AGESA_SUCCESS));
  *LowCore = (*pSocketDieMap)[Socket][Module].LowCore;
  *HighCore = (*pSocketDieMap)[Socket][Module].HighCore;
  if ((*pSocketDieMap)[Socket][Module].Node != 0xFF) {
    Result = TRUE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the current running core number.
 *
 * @param[out]     Core       The core id.
 * @param[in]      StdHeader  Header for library and services.
 *
 */
VOID
GetCurrentCore (
     OUT   UINT32 *Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPUID_DATA  CpuidDataStruct;
  UINT32      LocalApicId;
  UINT32      ApicIdCoreIdSize;
  CORE_ID_POSITION      InitApicIdCpuIdLo;
  CPU_SPECIFIC_SERVICES *FamilyServices;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  //  Read CPUID ebx[31:24] to get initial APICID
  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuidDataStruct, StdHeader);
  LocalApicId = (CpuidDataStruct.EBX_Reg & 0xFF000000) >> 24;

  // Find the core ID size.
  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuidDataStruct, StdHeader);
  ApicIdCoreIdSize = (CpuidDataStruct.ECX_Reg & 0x0000F000) >> 12;

  InitApicIdCpuIdLo = FamilyServices->CoreIdPositionInInitialApicId (FamilyServices, StdHeader);
  ASSERT (InitApicIdCpuIdLo < CoreIdPositionMax);

  // Now extract the core ID from the Apic ID by right justifying the id and masking off non-core Id bits.
  *Core = ((LocalApicId >> ((1 - (UINT32)InitApicIdCpuIdLo) * (MAX_CORE_ID_SIZE - ApicIdCoreIdSize))) &
             (MAX_CORE_ID_MASK >> (MAX_CORE_ID_SIZE - ApicIdCoreIdSize)));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns current node, and core number.
 *
 * @param[out]     Node         The node id of the current core's node.
 * @param[out]     Core         The core id if the current core.
 * @param[in]      StdHeader    Config handle for library and services.
 *
 */
VOID
GetCurrentNodeAndCore (
     OUT   UINT32 *Node,
     OUT   UINT32 *Core,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  // Get Node Id
  GetCurrentNodeNum (Node, StdHeader);

  // Get Core Id
  GetCurrentCore (Core, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
IsCurrentCorePrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN Result;
  UINT32 Core;
  UINT32 Socket;
  UINT32 Module;
  UINT32 PrimaryCore;
  UINT32 IgnoredCore;
  AGESA_STATUS IgnoredSts;

  Result = FALSE;

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  GetGivenModuleCoreRange (Socket, Module, &PrimaryCore, &IgnoredCore, StdHeader);
  if (Core == PrimaryCore) {
    Result = TRUE;
  }
  return Result;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Returns node id based on SocketId and ModuleId.
 *
 * @param[in]  SocketId       The socket to look up
 * @param[in]  ModuleId       The module in that socket
 * @param[out] NodeId         Provide the corresponding Node Id.
 * @param[in]  StdHeader   Handle of Header for calling lib functions and services.
 *
 * @retval     TRUE       The socket is populated
 * @retval     FALSE      The socket is not populated
 *
 */
BOOLEAN
GetNodeId (
  IN       UINT32  SocketId,
  IN       UINT32  ModuleId,
     OUT   UINT8   *NodeId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  SOCKET_DIE_TO_NODE_MAP  pSocketDieMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  BOOLEAN Result;
  AGESA_STATUS Status;

  Result = FALSE;
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;

  //  Get data block from heap
  Status = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (Status == AGESA_SUCCESS));
  *NodeId = (*pSocketDieMap)[SocketId][ModuleId].Node;
  if ((*pSocketDieMap)[SocketId][ModuleId].Node != 0xFF) {
    Result = TRUE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the cached AP Mailbox Info if available, or read the info from the hardware.
 *
 * Locate the known AP Mailbox Info Cache buffer in this core's local heap.  If it
 * doesn't exist, read the hardware to get the info.
 * This routine gets the main AP mailbox, not the system degree.
 *
 * @param[out]    ApMailboxInfo  Provide the info in this AP core's mailbox
 * @param[in]     StdHeader      Config handle for library and services.
 *
 */
VOID
GetApMailbox (
     OUT   UINT32               *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AGESA_STATUS     Ignored;
  LOCATE_HEAP_PTR  LocalApMailboxCache;
  CPU_SPECIFIC_SERVICES *FamilyServices;
  AP_MAILBOXES     ApMailboxes;
  BOOLEAN IamBsp;

  IamBsp = IsBsp (StdHeader, &Ignored);
  LocalApMailboxCache.BufferHandle = LOCAL_AP_MAIL_BOX_CACHE_HANDLE;
  if (((StdHeader->HeapStatus == HEAP_LOCAL_CACHE) || IamBsp) &&
      (HeapLocateBuffer (&LocalApMailboxCache, StdHeader) == AGESA_SUCCESS)) {
    // If during HEAP_LOCAL_CACHE stage, we always try to get ApMailbox from heap
    // If we're not in HEAP_LOCAL_CACHE stage, only BSP can get ApMailbox from heap
    *ApMailboxInfo = ((AP_MAILBOXES *) LocalApMailboxCache.BufferPtr)->ApMailInfo.Info;
  } else if (!IamBsp) {
    // If this is an AP, the hardware register should be good.
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);
    FamilyServices->GetApMailboxFromHardware (FamilyServices, &ApMailboxes, StdHeader);
    *ApMailboxInfo = ApMailboxes.ApMailInfo.Info;
  } else {
    // This is the BSC.  The hardware mailbox has not been set up yet.
    ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Cache the Ap Mailbox info in our local heap for later use.
 *
 * This enables us to use the info even after the mailbox register is initialized
 * with operational values.  Get all the AP mailboxes and keep them in one buffer.
 *
 * @param[in]     StdHeader   Config handle for library and services.
 *
 */
VOID
CacheApMailbox (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  AP_MAILBOXES ApMailboxes;
  CPU_SPECIFIC_SERVICES *FamilyServices;

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  // Get mailbox from hardware.
  FamilyServices->GetApMailboxFromHardware (FamilyServices, &ApMailboxes, StdHeader);

  // Allocate heap for the info
  AllocHeapParams.RequestedBufferSize = sizeof (AP_MAILBOXES);
  AllocHeapParams.BufferHandle = LOCAL_AP_MAIL_BOX_CACHE_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    *(AP_MAILBOXES *)AllocHeapParams.BufferPtr = ApMailboxes;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Compute the degree of the system.
 *
 * The degree of a system is the maximum degree of any node.  The degree of a node is the
 * number of nodes to which it is directly connected (not considering width or redundant
 * links).
 *
 * @param[in]     StdHeader            Config handle for library and services.
 *
 */
UINTN
GetSystemDegree (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_MAILBOXES *ApMailboxes;
  LOCATE_HEAP_PTR  LocalApMailboxCache;
  AGESA_STATUS Status;

  //  Get data block from heap
  LocalApMailboxCache.BufferHandle = LOCAL_AP_MAIL_BOX_CACHE_HANDLE;
  Status = HeapLocateBuffer (&LocalApMailboxCache, StdHeader);
  // non-Success handled by ASSERT not NULL below.
  ApMailboxes = (AP_MAILBOXES *)LocalApMailboxCache.BufferPtr;
  ASSERT ((ApMailboxes != NULL) && (Status == AGESA_SUCCESS));
  return ApMailboxes->ApMailExtInfo.Fields.SystemDegree;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Spins until the number of microseconds specified have
 *  expired regardless of CPU operational frequency.
 *
 *  @param[in]   Microseconds  Wait time in microseconds
 *  @param[in]   StdHeader     Header for library and services
 *
 */
VOID
WaitMicroseconds (
  IN       UINT32 Microseconds,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 TscRateInMhz;
  UINT64 NumberOfTicks;
  UINT64 InitialTsc;
  UINT64 CurrentTsc;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  LibAmdMsrRead (TSC, &InitialTsc, StdHeader);
  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetTscRate (FamilySpecificServices, &TscRateInMhz, StdHeader);
  NumberOfTicks = Microseconds * TscRateInMhz;
  do {
    LibAmdMsrRead (TSC, &CurrentTsc, StdHeader);
  } while ((CurrentTsc - InitialTsc) < NumberOfTicks);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  A boolean function determine executed CPU is BSP core.
 *
 *  @param[in,out]   StdHeader        Header for library and services
 *  @param[out]      AgesaStatus      Aggregates AGESA_STATUS for external interface, Always Succeeds.
 *
 */
BOOLEAN
IsBsp (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
     OUT   AGESA_STATUS      *AgesaStatus
  )
{
  UINT64 MsrData;

  // Always Succeeds.
  *AgesaStatus = AGESA_SUCCESS;

  //  Read APIC_BASE register (0x1B), bit[8] returns 1 for BSP
  LibAmdMsrRead (MSR_APIC_BAR, &MsrData, StdHeader);
  if ((MsrData & BIT8) != 0 ) {
    return TRUE;
  } else {
    return FALSE;
  }

}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the compute unit mapping algorithm.
 *
 * Look up the compute unit values for the current core's socket/module and find the matching
 * core pair map item.  This will tell us how to determine the core's status.
 *
 * @param[in]   StdHeader        Header for library and services
 *
 * @retval      AllCoresMapping  Each core is in a compute unit of its own.
 * @retval      EvenCoresMapping Even/Odd pairs of cores are in each compute unit.
 */
COMPUTE_UNIT_MAPPING
GetComputeUnitMapping (
  IN      AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT32  CurrentCore;
  UINT32  Module;
  UINT32  Socket;
  UINT8   Enabled;
  UINT8   DualCore;
  AGESA_STATUS  IgnoredSts;
  SOCKET_DIE_TO_NODE_MAP  pSocketDieMap;
  LOCATE_HEAP_PTR  SocketDieHeapDataBlock;
  CPU_SPECIFIC_SERVICES *FamilyServices;
  CORE_PAIR_MAP *CorePairMap;
  COMPUTE_UNIT_MAPPING Result;

  // Invalid mapping, unless we find one.
  Result = MaxComputeUnitMapping;

  IdentifyCore (StdHeader, &Socket, &Module, &CurrentCore, &IgnoredSts);
  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  //  Get data block from heap
  SocketDieHeapDataBlock.BufferHandle = SOCKET_DIE_MAP_HANDLE;
  IgnoredSts = HeapLocateBuffer (&SocketDieHeapDataBlock, StdHeader);
  pSocketDieMap = (SOCKET_DIE_TO_NODE_MAP)SocketDieHeapDataBlock.BufferPtr;
  ASSERT ((pSocketDieMap != NULL) && (IgnoredSts == AGESA_SUCCESS));
  // Current Core's socket, module must be present.
  ASSERT ((*pSocketDieMap)[Socket][Module].Node != 0xFF);

  // Process compute unit info
  Enabled = (*pSocketDieMap)[Socket][Module].EnabledComputeUnits;
  DualCore = (*pSocketDieMap)[Socket][Module].DualCoreComputeUnits;
  CorePairMap = FamilyServices->CorePairMap;
  if ((Enabled != 0) && (CorePairMap != NULL)) {
    while (CorePairMap->Enabled != 0xFF) {
      if ((Enabled == CorePairMap->Enabled) && (DualCore == CorePairMap->DualCore)) {
        break;
      }
      CorePairMap++;
    }
    // The assert is for finding a processor configured in a way the core pair map doesn't support.
    ASSERT (CorePairMap->Enabled != 0xFF);
    Result = CorePairMap->Mapping;
  } else {
    // Families that don't have compute units act as though each core is in its own compute unit
    // and all cores are primary
    Result = AllCoresMapping;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Is current core the primary core of its compute unit?
 *
 * Get the mapping algorithm and the current core number.  Selecting First/Last ordering for
 * primary @b ASSUMES cores are launched in ascending core number order.
 *
 * @param[in]   Selector         Select whether first or last core has the primary core role.
 * @param[in]   StdHeader        Header for library and services
 *
 * @retval       TRUE             This is the primary core of a compute unit.
 * @retval       FALSE            This is the second shared core of a compute unit.
 *
 */
BOOLEAN
IsCorePairPrimary (
  IN       COMPUTE_UNIT_PRIMARY_SELECTOR Selector,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  )
{
  BOOLEAN Result;
  UINT32  CurrentCore;
  UINT32  Module;
  UINT32  Socket;
  AGESA_STATUS  IgnoredSts;

  IdentifyCore (StdHeader, &Socket, &Module, &CurrentCore, &IgnoredSts);

  Result = FALSE;
  switch (GetComputeUnitMapping (StdHeader)) {
  case AllCoresMapping:
    // All cores are primaries
    Result = TRUE;
    break;
  case EvenCoresMapping:
    // Even core numbers are first to execute, odd cores are last to execute
    if (Selector == FirstCoreIsComputeUnitPrimary) {
      Result = (BOOLEAN) ((CurrentCore & 1) == 0);
    } else {
      Result = (BOOLEAN) ((CurrentCore & 1) != 0);
    }
    break;
  default:
    ASSERT (FALSE);
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Are the two specified cores shared in a compute unit?
 *
 * Look up the compute unit values for the current core's socket/module and find the matching
 * core pair map item.  This will tell us how to determine the core's status.
 *
 * @param[in]   Socket           The processor in this socket is to be checked
 * @param[in]   Module           The processor in this module is to be checked
 * @param[in]   CoreA            One of the two cores to check
 * @param[in]   CoreB            The other core to be checked
 * @param[in]   StdHeader        Header for library and services
 *
 * @retval       TRUE            The cores are in the same compute unit.
 * @retval       FALSE           The cores are not in the same compute unit, or the processor does
 *                               not have compute units.
 *
 */
BOOLEAN
AreCoresPaired (
  IN       UINT32  Socket,
  IN       UINT32  Module,
  IN       UINT32  CoreA,
  IN       UINT32  CoreB,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN Result;

  Result = FALSE;
  switch (GetComputeUnitMapping (StdHeader)) {
  case AllCoresMapping:
    // No cores are sharing a compute unit
    Result = FALSE;
    break;
  case EvenCoresMapping:
    // Even core numbers are paired with odd core numbers, n with n + 1
    if ((CoreA & 1) == 0) {
      Result = (BOOLEAN) (CoreA == (CoreB - 1));
    } else {
      Result = (BOOLEAN) (CoreA == (CoreB + 1));
    }
    break;
  default:
    ASSERT (FALSE);
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This routine programs the registers necessary to get the PCI MMIO mechanism
 * up and functioning.
 *
 * @param[in]      StdHeader               Pointer to structure containing the function call
 *                                         whose parameter structure is to be created, the
 *                                         allocation method, and a pointer to the newly
 *                                         created structure.
 *
 */
VOID
InitializePciMmio (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8        EncodedSize;
  UINT64       LocalMsrRegister;

  // Make sure that Standard header is valid
  ASSERT (StdHeader != NULL);

  if ((UserOptions.CfgPciMmioAddress != 0) && (UserOptions.CfgPciMmioSize != 0)) {
    EncodedSize = LibAmdBitScanForward (UserOptions.CfgPciMmioSize);
    LocalMsrRegister = ((UserOptions.CfgPciMmioAddress | BIT0) | (EncodedSize << 2));
    LibAmdMsrWrite (MSR_MMIO_Cfg_Base, &LocalMsrRegister, StdHeader);
  }
}
