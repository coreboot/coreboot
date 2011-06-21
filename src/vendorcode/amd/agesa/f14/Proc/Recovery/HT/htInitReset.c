/* $NoKeywords:$ */
/**
 * @file
 *
 * Recovery HT, a Hypertransport init for Boot Blocks.  For normal
 * boots, run Recovery HT first in boot block, then run full HT init
 * in the system BIOS.  Recovery HT moves the devices on the chain with
 * the southbridge to their assigned device IDS, so that all their PCI
 * Config space is accessible.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Recovery HyperTransport
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
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_HT_HTINITRESET_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_NODES 1
#define MAX_LINKS 8

extern CONST AMD_HT_RESET_INTERFACE HtOptionResetDefaults;

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/**
 * Our global state data structure.
 *
 * Keep track of inputs and outputs, and keep any working state.
 */
typedef struct {
  AMD_HT_RESET_INTERFACE      *HtBlock;       ///< The interface
  AGESA_STATUS                Status;         ///< Remember the highest severity status event
  VOID                        *ConfigHandle;  ///< Config Pointer, opaque handle for passing to lib
} HTR_STATE_DATA;

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
 ***               FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS                 ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Enable Routing Tables.
 *
 * Turns routing tables on for a node zero.
 *
 * @param[in] State Our State
 */

VOID
STATIC
HtrEnableRoutingTables (
  IN HTR_STATE_DATA *State
  )
{
  PCI_ADDR Reg;
  UINT32 Temp;
  Temp = 0;
  Reg.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0x6C);
  LibAmdPciWriteBits (Reg, 0, 0, &Temp, State->ConfigHandle);
}

/***************************************************************************
 ***                       Non-coherent init code                        ***
 ***                             Algorithms                              ***
 ***************************************************************************/
/*----------------------------------------------------------------------------------------*/
/**
 * Process the SouthBridge Link.
 *
 * Process a non-coherent link, and setting the device ID for all devices found.
 *
 * @param[in] State Our State, Inputs
 */
VOID
STATIC
HtrProcessLink (
  IN HTR_STATE_DATA *State
  )
{
  UINT32 CurrentBUID;
  UINT32 Temp;
  UINT32 UnitIDcnt;
  PCI_ADDR CurrentPtr;
  UINT8 Depth;
  BUID_SWAP_LIST *SwapPtr;
  PCI_ADDR Link1ControlRegister;
  BOOLEAN IsCaveDevice;

  // No PCI init to run, everything has to be on Bus zero.  This makes fewer
  // northbridge dependencies.
  //
  // Assign BUIDs so that config space for all devices is visible.
  //
  if (State->HtBlock->ManualBuidSwapList != NULL) {
    // Manual non-coherent BUID assignment
    // Assign BUID's per manual override
    //
    SwapPtr = &(State->HtBlock->ManualBuidSwapList->SwapList);
    Depth = 0;
    while (SwapPtr->Swaps[Depth].FromId != 0xFF) {
      CurrentPtr.AddressValue = MAKE_SBDFO (0, 0, SwapPtr->Swaps[Depth].FromId, 0, 0);

      do {
        LibAmdPciFindNextCap (&CurrentPtr, State->ConfigHandle);
        ASSERT (CurrentPtr.AddressValue != ILLEGAL_SBDFO);
        LibAmdPciRead (AccessWidth32, CurrentPtr, &Temp, State->ConfigHandle);
      } while ((Temp & (UINT32)0xE00000FF) != (UINT32)0x00000008); // HyperTransport Slave Capability

      CurrentBUID = SwapPtr->Swaps[Depth].ToId;
      // Set the device's BUID
      LibAmdPciWriteBits (CurrentPtr, 20, 16, &CurrentBUID, State->ConfigHandle);
      Depth++;
    }
  } else {
    // Automatic non-coherent device detection
    Depth = 0;
    CurrentBUID = 1;
    for (;;) {
      CurrentPtr.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);

      LibAmdPciRead (AccessWidth32, CurrentPtr, &Temp, State->ConfigHandle);
      if (Temp == (UINT32)0xFFFFFFFF) {
        // No device found at currentPtr
        break;
      }

      // HyperTransport Slave Capability
      do {
        LibAmdPciFindNextCap (&CurrentPtr, State->ConfigHandle);
        if (CurrentPtr.AddressValue == ILLEGAL_SBDFO) {
          // There is a device at currentPtr, but it isn't an HT device.
          return;
        }
        LibAmdPciRead (AccessWidth32, CurrentPtr, &Temp, State->ConfigHandle);
      } while ((Temp & (UINT32)0xE00000FF) != (UINT32)0x00000008); // HyperTransport Slave Capability

      // Get the device's Unit ID Count.
      LibAmdPciReadBits (CurrentPtr, 25, 21, &UnitIDcnt, State->ConfigHandle);
      if ((UnitIDcnt + CurrentBUID) > 24) {
        // An error handler for the case where we run out of BUID's on a chain
        State->Status = AGESA_ERROR;
        ASSERT (FALSE);
        return;
      }
      // While we are still certain we are accessing this device, remember if it is a cave device.
      // This is found by reading EOC from the Link 1 Control Register.
      Link1ControlRegister = CurrentPtr;
      Link1ControlRegister.Address.Register += 8;
      LibAmdPciReadBits (Link1ControlRegister, 6, 6, &Temp, State->ConfigHandle);
      IsCaveDevice = ((Temp == 0) ? FALSE : TRUE);

      // Set the device's BUID
      IDS_HDT_CONSOLE (HT_TRACE, "Device found at depth=%d.\n", Depth);
      LibAmdPciWriteBits (CurrentPtr, 20, 16, &CurrentBUID, State->ConfigHandle);

      CurrentPtr.Address.Device = CurrentBUID;
      // Get the device's BUID
      LibAmdPciReadBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
      if (Temp != CurrentBUID) {
        if ((Depth == 0) && IsCaveDevice) {
          // If the chain only consists of a single cave device, that device may have retained zero
          // for it's BUID.
          CurrentPtr.Address.Device = 0;
          LibAmdPciReadBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
          if (Temp == 0) {
            // Per HyperTransport specification, devices not accepting BUID reassignment hardwire BUID to zero.
            Depth++;
            // Success!
            IDS_HDT_CONSOLE (HT_TRACE, "Compliant Cave at BUID=0.\n");
            break;
          } else if (Temp == CurrentBUID) {
            // and then, there are the other kind of devices ....
            // Restore the writable BUID field (which contains the value we just wrote) to zero.
            Temp = 0;
            LibAmdPciWriteBits (CurrentPtr, 20, 16, &Temp, State->ConfigHandle);
            Depth++;
            // Success!
            IDS_HDT_CONSOLE (HT_TRACE, "Cave left at BUID=0.\n");
            break;
          }
        }
        // An error handler for this critical error
        State->Status = AGESA_ERROR;
        ASSERT (FALSE);
        return;
      }

      IDS_HDT_CONSOLE (HT_TRACE, "Compliant Device assigned at BUID=%d.\n", CurrentBUID);
      Depth++;
      CurrentBUID += UnitIDcnt;
    }
    // Provide information on automatic device results
    State->HtBlock->Depth = Depth;
  }
}

/***************************************************************************
 ***                           HT Reset Initialize                    ***
 ***************************************************************************/

/**
 * A constructor for the HyperTransport input structure.
 *
 * Sets inputs to valid, basic level, defaults.
 *
 * @param[in]     StdHeader             Config handle
 * @param[in,out] AmdHtResetInterface   HT Interface structure to initialize.
 *
 * @retval AGESA_SUCCESS      Constructors are not allowed to fail
*/
AGESA_STATUS
AmdHtResetConstructor (
  IN       AMD_CONFIG_PARAMS         *StdHeader,
  IN       AMD_HT_RESET_INTERFACE    *AmdHtResetInterface
  )
{
  AmdHtResetInterface->ManualBuidSwapList = HtOptionResetDefaults.ManualBuidSwapList;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Initialize HT for Reset, Boot Blocks.
 *
 * This is the top level external interface for Hypertransport Reset Initialization.
 * Create our initial internal state and initialize the non-coherent chain to the
 * southbridge.  This interface must be executed by both normal and recovery boot paths.
 *
 * @param[in]  StdHeader                 Interface structure
 * @param[in]  AmdHtResetInterface       our interface and inputs
 *
 * @retval AGESA_SUCCESS Successful init
 * @retval AGESA_ERROR   Device Error, BUID max exceed error.
 *
 */
AGESA_STATUS
AmdHtInitReset (
  IN       AMD_CONFIG_PARAMS        *StdHeader,
  IN       AMD_HT_RESET_INTERFACE   *AmdHtResetInterface
  )
{
  HTR_STATE_DATA State;
  AGESA_STATUS IgnoredStatus;

  State.Status = AGESA_SUCCESS;
  if (IsBsp (StdHeader, &IgnoredStatus)) {
    State.ConfigHandle = (AMD_CONFIG_PARAMS *)StdHeader;
    State.HtBlock = AmdHtResetInterface;
    HtrEnableRoutingTables (&State);

    HtrProcessLink (&State);
  }
  return State.Status;
}
