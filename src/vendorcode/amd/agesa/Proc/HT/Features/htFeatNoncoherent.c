/* $NoKeywords:$ */
/**
 * @file
 *
 * Non-Coherent Discovery Routines.
 *
 * Contains routines for enumerating and initializing non-coherent devices.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35740 $   @e \$Date: 2010-07-30 00:04:17 +0800 (Fri, 30 Jul 2010) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNotify.h"
#include "htNb.h"
#include "htFeatNoncoherent.h"
#include "htFeatOptimization.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FEATURES_HTFEATNONCOHERENT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define NO_DEVICE 0xFFFFFFFFull

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***                       Non-coherent init code                        ***
 ***                             Algorithms                              ***
 ***************************************************************************/
/*----------------------------------------------------------------------------------------*/
/**
 * Process a non-coherent Link.
 *
 * @HtFeatMethod{::F_PROCESS_LINK}
 *
 * Enable a range of bus numbers, and set the device ID for all devices found.  Add
 * non-coherent devices, links to the system data structure.
 *
 * @param[in]     Node          Node on which to process nc init
 * @param[in]     Link          The non-coherent Link on that Node
 * @param[in]     IsCompatChain Is this the chain with the southbridge? TRUE if yes.
 * @param[in,out] State         our global state
 */
VOID
ProcessLink (
  IN       UINT8         Node,
  IN       UINT8         Link,
  IN       BOOLEAN       IsCompatChain,
  IN OUT   STATE_DATA    *State
  )
{
  UINT8 SecBus;
  UINT8 SubBus;
  UINT32 CurrentBuid;
  UINT32 Temp;
  UINT32 UnitIdCount;
  PCI_ADDR CurrentPtr;
  PCI_ADDR Link1ControlRegister;
  UINT8 Depth;
  BUID_SWAP_LIST *SwapPtr;
  UINT8  LastLink;
  BOOLEAN IsCaveDevice;

  ASSERT ((Node < MAX_NODES) && (Link < State->Nb->MaxLinks));

  if (!State->HtInterface->GetOverrideBusNumbers (Node, Link, &SecBus, &SubBus, State)) {
    // Assign Bus numbers
    if (State->AutoBusCurrent >= State->HtBlock->AutoBusMax) {
      //  If we run out of Bus Numbers, notify and skip this chain
      //
      IDS_ERROR_TRAP;
      NotifyErrorNcohBusMaxExceed (Node, Link, State->AutoBusCurrent, State);
      return;
    }

    if (State->UsedCfgMapEntries >= 4) {
      // If we have used all the PCI Config maps we can't add another chain.
      // Notify and if call back is unimplemented or returns, skip this chain.
      //
      IDS_ERROR_TRAP;
      NotifyErrorNcohCfgMapExceed (Node, Link, State);
      return;
    }

    SecBus = State->AutoBusCurrent;
    SubBus = SecBus + State->HtBlock->AutoBusIncrement - 1;
    State->AutoBusCurrent = State->AutoBusCurrent + State->HtBlock->AutoBusIncrement;
  }

  State->Nb->SetConfigAddrMap (State->UsedCfgMapEntries, SecBus, SubBus, Node, Link, State, State->Nb);
  State->UsedCfgMapEntries++;

  if (State->HtInterface->GetManualBuidSwapList (Node, Link, &SwapPtr, State)) {
    // Manual non-coherent BUID assignment
    AGESA_TESTPOINT (TpProcHtManualNc, State->ConfigHandle);


    if (!IsCompatChain || !State->IsUsingRecoveryHt) {
      //  If this is the not southbridge chain or Recovery HT was not used
      // then we need to assign BUIDs here.
      //
      Depth = 0;
      // Assign BUID's per manual override
      while (SwapPtr->Swaps[Depth].FromId != 0xFF) {
        CurrentPtr.AddressValue = MAKE_SBDFO (0, SecBus, SwapPtr->Swaps[Depth].FromId, 0, 0);
        if (DoesDeviceHaveHtSubtypeCap (CurrentPtr, HT_SLAVE_CAPABILITY, &CurrentPtr, State)) {
          // Set the device's BUID field [20:16] to the current buid
          CurrentBuid = SwapPtr->Swaps[Depth].ToId;
          LibAmdPciWriteBits (CurrentPtr, 20, 16, &CurrentBuid, State->ConfigHandle);
          Depth++;
        } else {
          // All non-coherent devices must have a slave interface capability.
          ASSERT (FALSE);
          break;
        }
      }
    }

    // Build chain of devices. Do this even if Recovery HT assign BUIDs for this chain.
    Depth = 0;
    while (SwapPtr->FinalIds[Depth] != 0xFF) {
      ASSERT (State->TotalLinks < MAX_PLATFORM_LINKS);
      (*State->PortList)[(State->TotalLinks * 2)].NodeID = Node;
      // Note: depth == 0 is true before depth > 0. This makes LastLink variable work.
      if (Depth == 0) {
        (*State->PortList)[(State->TotalLinks * 2)].Type = PORTLIST_TYPE_CPU;
        (*State->PortList)[(State->TotalLinks * 2)].Link = Link;
      } else {
        // Fill in the host side port.  Link and base pointer can be deduced from the upstream link's
        // downstream port.
        (*State->PortList)[(State->TotalLinks * 2)].Type = PORTLIST_TYPE_IO;
        (*State->PortList)[(State->TotalLinks * 2)].Link = 1 - (*State->PortList)[(((State->TotalLinks - 1) * 2) + 1)].Link;
        (*State->PortList)[(State->TotalLinks * 2)].HostLink = Link;
        (*State->PortList)[(State->TotalLinks * 2)].HostDepth = Depth - 1;
        (*State->PortList)[(State->TotalLinks * 2)].Pointer = (*State->PortList)[(((State->TotalLinks - 1) * 2) + 1)].Pointer;
      }

      (*State->PortList)[(State->TotalLinks * 2) + 1].Type = PORTLIST_TYPE_IO;
      (*State->PortList)[(State->TotalLinks * 2) + 1].NodeID = Node;
      (*State->PortList)[(State->TotalLinks * 2) + 1].HostLink = Link;
      (*State->PortList)[(State->TotalLinks * 2) + 1].HostDepth = Depth;

      CurrentPtr.AddressValue = MAKE_SBDFO (0, SecBus, (SwapPtr->FinalIds[Depth] & 0x3F), 0, 0);
      if (DoesDeviceHaveHtSubtypeCap (CurrentPtr, HT_SLAVE_CAPABILITY, &CurrentPtr, State)) {
        (*State->PortList)[(State->TotalLinks * 2) + 1].Pointer = CurrentPtr;
      } else {
        // All non-coherent devices must have a slave interface capability.
        ASSERT (FALSE);
        break;
      }

      // Bit 6 indicates whether orientation override is desired.
      // Bit 7 indicates the upstream Link if overriding.
      //
      // assert catches at least the one known incorrect setting, that a non-zero link
      // is specified, but override desired is not set.
      ASSERT (((SwapPtr->FinalIds[Depth] & 0x40) != 0) || ((SwapPtr->FinalIds[Depth] & 0x80) == 0));
      if ((SwapPtr->FinalIds[Depth] & 0x40) != 0) {
        // Override the device's orientation
        LastLink = SwapPtr->FinalIds[Depth] >> 7;
      } else {
        // Detect the device's orientation, by reading the Master Host bit [26]
        LibAmdPciReadBits (CurrentPtr, 26, 26, &Temp, State->ConfigHandle);
        LastLink = (UINT8)Temp;
      }
      (*State->PortList)[(State->TotalLinks * 2) + 1].Link = LastLink;

      Depth++;
      State->TotalLinks++;
    }
  } else {
    // Automatic non-coherent device detection
    AGESA_TESTPOINT (TpProcHtAutoNc, State->ConfigHandle);
    IDS_HDT_CONSOLE (HT_TRACE, "Auto IO chain init on node=%d, link=%d, secbus=%d, subbus=%d%s.\n",
                     Node, Link, SecBus, SubBus, (IsCompatChain ? ", Compat" : ""));
    Depth = 0;
    CurrentBuid = 1;
    for (; ; ) {
      CurrentPtr.AddressValue = MAKE_SBDFO (0, SecBus, 0, 0, 0);

      LibAmdPciRead (AccessWidth32, CurrentPtr, &Temp, State->ConfigHandle);
      if (Temp == NO_DEVICE) {
        if (IsCompatChain && State->IsUsingRecoveryHt) {
          // See if the device is aleady at a non-zero BUID because HT Init Reset aleady assigned it.
          CurrentPtr.Address.Device = CurrentBuid;
          LibAmdPciRead (AccessWidth32, CurrentPtr, &Temp, State->ConfigHandle);
          if (Temp == NO_DEVICE) {
            // No more devices already assigned.
            break;
          }
        } else {
          // No more devices found.
          break;
        }
      }

      ASSERT (State->TotalLinks < MAX_PLATFORM_LINKS);

      (*State->PortList)[(State->TotalLinks * 2)].NodeID = Node;
      if (Depth == 0) {
        (*State->PortList)[(State->TotalLinks * 2)].Type = PORTLIST_TYPE_CPU;
        (*State->PortList)[(State->TotalLinks * 2)].Link = Link;
      } else {
        // Fill in the host side port.  Link and base pointer can be deduced from the upstream link's
        // downstream port.
        (*State->PortList)[(State->TotalLinks * 2)].Type = PORTLIST_TYPE_IO;
        (*State->PortList)[(State->TotalLinks * 2)].Link = 1 - (*State->PortList)[((State->TotalLinks - 1) * 2) + 1].Link;
        (*State->PortList)[(State->TotalLinks * 2)].HostLink = Link;
        (*State->PortList)[(State->TotalLinks * 2)].HostDepth = Depth - 1;
        (*State->PortList)[(State->TotalLinks * 2)].Pointer = (*State->PortList)[((State->TotalLinks - 1) * 2) + 1].Pointer;
      }

      (*State->PortList)[(State->TotalLinks * 2) + 1].Type = PORTLIST_TYPE_IO;
      (*State->PortList)[(State->TotalLinks * 2) + 1].NodeID = Node;
      (*State->PortList)[(State->TotalLinks * 2) + 1].HostLink = Link;
      (*State->PortList)[(State->TotalLinks * 2) + 1].HostDepth = Depth;

      if (DoesDeviceHaveHtSubtypeCap (CurrentPtr, HT_SLAVE_CAPABILITY, &CurrentPtr, State)) {

        // Get device's unit id count [25:21]
        LibAmdPciReadBits (CurrentPtr, 25, 21, &UnitIdCount, State->ConfigHandle);
        if (((UnitIdCount + CurrentBuid) > MAX_BUID) || ((SecBus == 0) && ((UnitIdCount + CurrentBuid) > 24))) {
          // An error handler for the case where we run out of BUID's on a chain
          NotifyErrorNcohBuidExceed (Node, Link, Depth, (UINT8)CurrentBuid, (UINT8)UnitIdCount, State);
          IDS_ERROR_TRAP;
          break;
        }
        // While we are still certain we are accessing this device, remember if it is a cave device.
        // This is found by reading EOC from the Link 1 Control Register.
        Link1ControlRegister = CurrentPtr;
        Link1ControlRegister.Address.Register += (HTSLAVE_LINK01_OFFSET + HTSLAVE_LINK_CONTROL_0_REG);
        LibAmdPciReadBits (Link1ControlRegister, 6, 6, &Temp, State->ConfigHandle);
        IsCaveDevice = ((Temp == 0) ? FALSE : TRUE);

        // Attempt to write the new BUID.  Unless this chain was aleady assigned BUIDs during Init Reset,
        // then just re-discover the chain.  Note this may be true whether the device was found at
        // BUID zero or not.
        IDS_HDT_CONSOLE (HT_TRACE, "Found device at depth=%d, BUID=%d.\n", Depth, CurrentPtr.Address.Device);
        if (!IsCompatChain || !State->IsUsingRecoveryHt) {
          IDS_HDT_CONSOLE (HT_TRACE, "Assigning device to BUID=%d.\n", CurrentBuid);
          LibAmdPciWriteBits (CurrentPtr, 20, 16, &CurrentBuid, State->ConfigHandle);
        }

        CurrentPtr.Address.Device = CurrentBuid;
        LibAmdPciReadBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
        if (Temp != CurrentBuid) {
          if ((Depth == 0) && IsCaveDevice) {
            // If the chain only consists of a single cave device, that device may have retained zero
            // for it's BUID.
            CurrentPtr.Address.Device = 0;
            LibAmdPciReadBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
            if (Temp == 0) {
              // Per HyperTransport specification, devices not accepting BUID reassignment hardwire BUID to zero.
              (*State->PortList)[(State->TotalLinks * 2) + 1].Link = 0;
              (*State->PortList)[(State->TotalLinks * 2) + 1].Pointer = CurrentPtr;
              State->TotalLinks++;
              Depth++;
              // Success!
              IDS_HDT_CONSOLE (HT_TRACE, "%s Cave left at BUID=0.\n", ((!IsCompatChain || !State->IsUsingRecoveryHt) ? "Compatible" : "Already Assigned"));
              break;
            } else if (Temp == CurrentBuid) {
              // and then, there are the other kind of devices ....
              // Restore the writable BUID field (which contains the value we just wrote) to zero.
              Temp = 0;
              LibAmdPciWriteBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
              (*State->PortList)[(State->TotalLinks * 2) + 1].Link = 0;
              (*State->PortList)[(State->TotalLinks * 2) + 1].Pointer = CurrentPtr;
              State->TotalLinks++;
              Depth++;
              // Success!
              IDS_HDT_CONSOLE (HT_TRACE, "Cave left at BUID=0.\n");
              break;
            }
          }
          // An error handler for this error,
          // this often occurs in new BIOS ports and it means you need to use a Manual BUID Swap List.
          NotifyErrorNcohDeviceFailed (Node, Link, Depth, (UINT8)CurrentBuid, State);
          IDS_ERROR_TRAP;
          break;
        }

        LibAmdPciReadBits (CurrentPtr, 26, 26, &Temp, State->ConfigHandle);
        (*State->PortList)[(State->TotalLinks * 2) + 1].Link = (UINT8)Temp;
        (*State->PortList)[(State->TotalLinks * 2) + 1].Pointer = CurrentPtr;

        IDS_HDT_CONSOLE (HT_TRACE, "Device assigned.\n");
        Depth++;
        State->TotalLinks++;
        CurrentBuid += UnitIdCount;
      } else {
        // All non-coherent devices must have a slave interface capability.
        ASSERT (FALSE);
        break;
      }
    }
    // Provide information on automatic device results
    NotifyInfoNcohAutoDepth (Node, Link, (Depth - 1), State);
  }
}
