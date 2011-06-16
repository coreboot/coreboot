/* $NoKeywords:$ */
/**
 * @file
 *
 * Link Optimization Routines.
 *
 * Contains routines for determining width, frequency, and other
 * Link features
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 47937 $   @e \$Date: 2011-03-02 13:35:06 +0800 (Wed, 02 Mar 2011) $
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
#include "IdsHt.h"
#include "htInterface.h"
#include "htNb.h"
#include "htFeatOptimization.h"
#include "htNotify.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_FEATURES_HTFEATOPTIMIZATION_FILECODE

extern CONST PF_HtIdsGetPortOverride ROMDATA pf_HtIdsGetPortOverride;

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PCI_CONFIG_COMMAND_REG04  4
#define PCI_CONFIG_REVISION_REG08 8

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
 ***                            Link Optimization                        ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Given the bits set in the register field, return the width it represents.
 *
 * As invalid width values or encodings are rare except during debug, catch those using
 * ASSERT().  This means theoretically we are returning an incorrect result if that
 * happens.  The default chosen for the result is arbitrarily 8 bits.  This is likely
 * not to be the actual correct width and may cause a crash, hang, or incorrect operation.
 * Hardware often ignores writes of invalid width encodings.
 *
 * @note This routine is used for CPUs as well as IO devices, as all comply to the
 * "HyperTransport I/O Link Specification ".
 *
 * @param[in]     Value The bits for the register
 *
 * @return The width
 */
UINT8
STATIC
ConvertBitsToWidth (
  IN       UINT8       Value
  )
{
  UINT8 Result;

  Result = 0;

  switch (Value) {

  case  1:
    Result = 16;
    break;

  case  0:
    Result = 8;
    break;

  case 3:
    Result = 32;
    break;

  case  5:
    Result = 4;
    break;

  case  4:
    Result = 2;
    break;

  default:
    ASSERT (FALSE);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Translate a desired width setting to the bits to set in the register field.
 *
 * As invalid width values or encodings are rare except during debug, catch those using
 * ASSERT().  This means theoretically we are returning an incorrect result if that
 * happens.  The default chosen for the result is arbitrarily 8 bits.  This is likely
 * not to be the actual correct width and may cause a crash, hang, or incorrect operation.
 * Hardware often ignores writes of invalid width encodings.
 *
 * @note This routine is used for CPUs as well as IO devices, as all comply to the
 * "HyperTransport I/O Link Specification ".
 *
 * @param[in]     Value   the width Value
 *
 * @return The bits for the register
 */
UINT8
ConvertWidthToBits (
  IN       UINT8       Value
  )
{
  UINT8 Result;

  Result = 8;

  switch (Value) {

  case 16:
    Result = 1;
    break;

  case 8:
    Result = 0;
    break;

  case 32:
    Result = 3;
    break;

  case 4:
    Result = 5;
    break;

  case 2:
    Result = 4;
    break;

  default:
    ASSERT (FALSE);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Access HT Link Control Register.
 *
 * @HtFeatMethod{::F_SET_HT_CONTROL_REGISTER_BITS}
 *
 * Provide a common routine for accessing the HT Link Control registers (84, a4, c4,
 * e4), to enforce not clearing the HT CRC error bits.  Replaces direct use of
 * AmdPCIWriteBits().
 *
 * @note: This routine is called for CPUs as well as IO Devices! All comply to the
 * "HyperTransport I/O Link Specification ".
 *
 * @param[in]     Reg      the PCI config address the control register
 * @param[in]     HiBit    the high bit number
 * @param[in]     LoBit    the low bit number
 * @param[in]     Value    the value to write to that bit range. Bit 0 => loBit.
 * @param[in]     State    Our state, config handle for lib
 */
VOID
SetHtControlRegisterBits (
  IN       PCI_ADDR    Reg,
  IN       UINT8       HiBit,
  IN       UINT8       LoBit,
  IN       UINT32      *Value,
  IN       STATE_DATA  *State
  )
{
  UINT32 Temp;
  UINT32 mask;

  ASSERT ((HiBit < 32) && (LoBit < 32) && (HiBit >= LoBit) && ((Reg.AddressValue & 0x3) == 0));
  ASSERT ((HiBit < 8) || (LoBit > 9));

  // A 1 << 32 == 1 << 0 due to x86 SHL instruction, so skip if that is the case
  if ((HiBit - LoBit) != 31) {
    mask = (((UINT32)1 << (HiBit - LoBit + 1)) - 1);
  } else {
    mask = (UINT32)0xFFFFFFFF;
  }

  LibAmdPciRead (AccessWidth32, Reg, &Temp, State->ConfigHandle);
  Temp &= ~(mask << LoBit);
  Temp |= (*Value & mask) << LoBit;
  Temp &= (UINT32)HT_CONTROL_CLEAR_CRC;
  LibAmdPciWrite (AccessWidth32, Reg, &Temp, State->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set HT Frequency register for IO Devices
 *
 * Provide a common routine for accessing the HT Link Frequency registers at offset 8
 * and 0x10, to enforce not clearing the HT Link error bits.  Replaces direct use of
 * AmdPCIWriteBits().
 *
 * @note This routine is called for IO Devices only!! All comply to the
 * "HyperTransport I/O Link Specification ".
 *
 * @param[in]    Reg       the PCI config address the control register
 * @param[in]    Hibit     the high bit number
 * @param[in]    Lobit     the low bit number
 * @param[in]    Value     the value to write to that bit range. Bit 0 => loBit.
 * @param[in]    State     Our state, config handle for lib
 */
VOID
STATIC
SetHtIoFrequencyRegisterBits (
  IN       PCI_ADDR    Reg,
  IN       UINT8       Hibit,
  IN       UINT8       Lobit,
  IN       UINT32      *Value,
  IN       STATE_DATA  *State
  )
{
  UINT32 Mask;
  UINT32 Temp;

  ASSERT ((Hibit < 32) && (Lobit < 32) && (Hibit >= Lobit) && ((Reg.AddressValue & 0x3) == 0));
  ASSERT ((Hibit < 12) || (Lobit > 14));

  // A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case
  if ((Hibit - Lobit) != 31) {
    Mask = (((UINT32)1 << ((Hibit - Lobit) + 1)) - 1);
  } else {
    Mask = (UINT32)0xFFFFFFFF;
  }

  LibAmdPciRead (AccessWidth32, Reg, &Temp, State->ConfigHandle);
  Temp &= ~(Mask << Lobit);
  Temp |= (*Value & Mask) << Lobit;
  Temp &= (UINT32)HT_FREQUENCY_CLEAR_LINK_ERRORS;
  LibAmdPciWrite (AccessWidth32, Reg, &Temp, State->ConfigHandle);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Link features into system data structure.
 *
 * @HtFeatMethod{::F_GATHER_LINK_DATA}
 *
 * For all discovered Links, populate the port list with the frequency and width
 * capabilities. Gather support data for:
 * - Unit ID Clumping
 *
 * @param[in]     State our global state, port list
 */
VOID
GatherLinkData (
  IN       STATE_DATA *State
  )
{
  UINT8 i;
  PCI_ADDR LinkBase;
  PCI_ADDR Reg;
  UINT32 Bits;
  UINT8 Revision;

  // Get the capability base for whatever device type the link port is on
  for (i = 0; i < (State->TotalLinks * 2); i++) {
    if ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) {
      LinkBase = State->Nb->MakeLinkBase ((*State->PortList)[i].NodeID, (*State->PortList)[i].Link, State->Nb);
      (*State->PortList)[i].Pointer = LinkBase;
    } else {
      LinkBase = (*State->PortList)[i].Pointer;
      if ((*State->PortList)[i].Link == 1) {
        LinkBase.Address.Register += HTSLAVE_LINK01_OFFSET;
      }
    }

    // Getting the Width is standard across device types
    Reg = LinkBase;
    Reg.Address.Register += HTSLAVE_LINK_CONTROL_0_REG;
    LibAmdPciReadBits (Reg, 22, 20, &Bits, State->ConfigHandle);
    (*State->PortList)[i].PrvWidthOutCap = ConvertBitsToWidth ((UINT8)Bits);

    LibAmdPciReadBits (Reg, 18, 16, &Bits, State->ConfigHandle);
    (*State->PortList)[i].PrvWidthInCap = ConvertBitsToWidth ((UINT8)Bits);

    // Get Frequency and other device type specific features
    if ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) {
      State->Nb->GatherLinkFeatures (&(*State->PortList)[i], State->HtInterface, State->PlatformConfiguration, State->Nb);
    } else {
      Reg = LinkBase;
      Reg.Address.Register += HTSLAVE_FREQ_REV_0_REG;
      LibAmdPciReadBits (Reg, 31, 16, &Bits, State->ConfigHandle);
      (*State->PortList)[i].PrvFrequencyCap = Bits;

      // Unit ID Clumping Support
      if (State->IsUsingUnitIdClumping) {
        if (DoesDeviceHaveHtSubtypeCap (LinkBase, HT_UNITID_CAPABILITY, &Reg, State)) {
          Reg.Address.Register += HTUNIT_SUPPORT_REG;
          LibAmdPciReadBits (Reg, 31, 0, &Bits, State->ConfigHandle);
        } else {
          // Not there, that's ok, we don't know that it should have one.
          // Check for Passive support. (Bit 0 won't be set if full support is implemented,
          // so we can use it to indicate passive support in our portlist struct).
          Reg = LinkBase;
          Reg.Address.Register += HTSLAVE_FEATURECAP_REG;
          Bits = 1;
          LibAmdPciWriteBits (Reg, 5, 5, &Bits, State->ConfigHandle);
          LibAmdPciReadBits (Reg, 5, 5, &Bits, State->ConfigHandle);
        }
        (*State->PortList)[i].ClumpingSupport = Bits;
      } else {
        (*State->PortList)[i].ClumpingSupport = HT_CLUMPING_DISABLE;
      }

      Reg = LinkBase;
      Reg.Address.Register = PCI_CONFIG_REVISION_REG08;
      LibAmdPciReadBits ( LinkBase, 7, 0, &Bits, State->ConfigHandle);
      Revision = (UINT8) Bits;

      LinkBase.Address.Register = 0;
      LibAmdPciRead (AccessWidth32, LinkBase, &Bits, State->ConfigHandle);

      State->HtInterface->GetDeviceCapOverride ((*State->PortList)[i].NodeID,
                           (*State->PortList)[i].HostLink,
                           (*State->PortList)[i].HostDepth,
                           (*State->PortList)[i].Pointer,
                           Bits,
                           Revision,
                           (*State->PortList)[i].Link,
                           &((*State->PortList)[i].PrvWidthInCap),
                           &((*State->PortList)[i].PrvWidthOutCap),
                           &((*State->PortList)[i].PrvFrequencyCap),
                           &((*State->PortList)[i].ClumpingSupport),
                           State);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Optimize Links.
 *
 * @HtFeatMethod{::F_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY}
 *
 * For all Links:
 * Examine both sides of a Link and determine the optimal frequency and width,
 * taking into account externally provided limits and enforcing any other limit
 * or matching rules as applicable except subLink balancing. Update the port
 * list data with the optimal settings.
 *
 * @note no hardware state changes in this routine.
 *
 * @param[in,out]    State         Process and update portlist
 */
VOID
SelectOptimalWidthAndFrequency (
  IN OUT   STATE_DATA   *State
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 Freq;
  UINT32 Temp;
  UINT32 CbPcbFreqLimit;
  UINT8 CbPcbABDownstreamWidth;
  UINT8 CbPcbBAUpstreamWidth;

  for (i = 0; i < (State->TotalLinks * 2); i += 2) {
    CbPcbFreqLimit = HT_FREQUENCY_NO_LIMIT;
    CbPcbABDownstreamWidth = HT_WIDTH_16_BITS;
    CbPcbBAUpstreamWidth = HT_WIDTH_16_BITS;

    if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) && ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_CPU)) {
      State->HtInterface->GetCpu2CpuPcbLimits ((*State->PortList)[i].NodeID,
                           (*State->PortList)[i].Link,
                           (*State->PortList)[i + 1].NodeID,
                           (*State->PortList)[i + 1].Link,
                           &CbPcbABDownstreamWidth,
                           &CbPcbBAUpstreamWidth,
                           &CbPcbFreqLimit,
                           State
        );
    } else {
      State->HtInterface->GetIoPcbLimits ((*State->PortList)[i + 1].NodeID,
                      (*State->PortList)[i + 1].HostLink,
                      (*State->PortList)[i + 1].HostDepth,
                      &CbPcbABDownstreamWidth,
                      &CbPcbBAUpstreamWidth,
                      &CbPcbFreqLimit,
                      State
        );
    }

    Temp = (*State->PortList)[i].PrvFrequencyCap;
    Temp &= (*State->PortList)[i + 1].PrvFrequencyCap;
    Temp &= CbPcbFreqLimit;
    (*State->PortList)[i].CompositeFrequencyCap = (UINT32)Temp;
    (*State->PortList)[i + 1].CompositeFrequencyCap = (UINT32)Temp;

    ASSERT (Temp != 0);
    Freq = LibAmdBitScanReverse (Temp);
    (*State->PortList)[i].SelFrequency = Freq;
    (*State->PortList)[i + 1].SelFrequency = Freq;

    Temp = (*State->PortList)[i].PrvWidthOutCap;
    if ((*State->PortList)[i + 1].PrvWidthInCap < Temp) {
      Temp = (*State->PortList)[i + 1].PrvWidthInCap;
    }
    if (CbPcbABDownstreamWidth < Temp) {
      Temp = CbPcbABDownstreamWidth;
    }
    (*State->PortList)[i].SelWidthOut = (UINT8)Temp;
    (*State->PortList)[i + 1].SelWidthIn = (UINT8)Temp;

    Temp = (*State->PortList)[i].PrvWidthInCap;
    if ((*State->PortList)[i + 1].PrvWidthOutCap < Temp) {
      Temp = (*State->PortList)[i + 1].PrvWidthOutCap;
    }
    if (CbPcbBAUpstreamWidth < Temp) {
      Temp = CbPcbBAUpstreamWidth;
    }
    (*State->PortList)[i].SelWidthIn = (UINT8)Temp;
    (*State->PortList)[i + 1].SelWidthOut = (UINT8)Temp;
  }
  // Calculate unit id clumping
  //
  // Find the root of each IO Chain, process the chain for clumping support.
  // The root is always the first link of the chain in the port list.
  // Clumping is not device link specific, so we can just look at the upstream ports (j+1). Use ASSERTs to sanity
  // check the downstream ports (j).  If any device on the chain does not support clumping, the entire chain will be
  // disabled for clumping.
  // After analyzing the clumping support on the chain the CPU's portlist has the enable mask.  Update all the
  // IO Devices on the chain with the enable mask.  If any device's only have passive support, that is already enabled.
  //
  if (State->IsUsingUnitIdClumping) {
    for (i = 0; i < (State->TotalLinks * 2); i += 2) {
      if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) && ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_IO)) {
        (*State->PortList)[i].ClumpingSupport = HT_CLUMPING_DISABLE;
        if ((*State->PortList)[i + 1].ClumpingSupport != HT_CLUMPING_DISABLE) {
          (*State->PortList)[i].ClumpingSupport |= (*State->PortList)[i + 1].ClumpingSupport;
          for (j = i + 2; j < (State->TotalLinks * 2); j += 2) {
            if (((*State->PortList)[j].Type == PORTLIST_TYPE_IO) && ((*State->PortList)[j + 1].Type == PORTLIST_TYPE_IO)) {
              if (((*State->PortList)[i].NodeID == (*State->PortList)[j + 1].NodeID) &&
                  ((*State->PortList)[i].Link == (*State->PortList)[j + 1].HostLink)) {
                ASSERT (((*State->PortList)[i].NodeID == (*State->PortList)[j + 1].NodeID) &&
                        ((*State->PortList)[i].Link == (*State->PortList)[j].HostLink));
                if ((*State->PortList)[j + 1].ClumpingSupport != HT_CLUMPING_DISABLE) {
                  ASSERT ((((*State->PortList)[j + 1].ClumpingSupport & HT_CLUMPING_PASSIVE) == 0) ||
                          (((*State->PortList)[j + 1].ClumpingSupport & ~(HT_CLUMPING_PASSIVE)) == 0));
                  (*State->PortList)[i].ClumpingSupport |= (*State->PortList)[j + 1].ClumpingSupport;
                } else {
                  (*State->PortList)[i].ClumpingSupport = HT_CLUMPING_DISABLE;
                  break;
                }
              }
            }
          }
          if ((*State->PortList)[i + 1].ClumpingSupport != HT_CLUMPING_PASSIVE) {
            (*State->PortList)[i + 1].ClumpingSupport = (*State->PortList)[i].ClumpingSupport;
          }
          for (j = i + 2; j < (State->TotalLinks * 2); j += 2) {
            if (((*State->PortList)[j].Type == PORTLIST_TYPE_IO) && ((*State->PortList)[j + 1].Type == PORTLIST_TYPE_IO)) {
              if (((*State->PortList)[i].NodeID == (*State->PortList)[j + 1].NodeID) &&
                  ((*State->PortList)[i].Link == (*State->PortList)[j + 1].HostLink)) {
                if ((*State->PortList)[j + 1].ClumpingSupport != HT_CLUMPING_PASSIVE) {
                  (*State->PortList)[j + 1].ClumpingSupport = (*State->PortList)[i].ClumpingSupport;
                  // The downstream isn't really passive, just mark it so in order to write the device only once.
                  (*State->PortList)[j].ClumpingSupport = HT_CLUMPING_PASSIVE;
                }
              }
            }
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 * @HtFeatMethod{::F_SET_LINK_DATA}
 *
 * @param[in]     State   our global state, port list
 */
VOID
SetLinkData (
  IN       STATE_DATA *State
  )
{
  UINT8 i;
  PCI_ADDR LinkBase;
  PCI_ADDR Reg;
  UINT32 Temp;
  UINT32 Widthin;
  UINT32 Widthout;
  UINT32 Bits;
  PCI_ADDR CurrentPtr;
  HTIDS_PORT_OVERRIDE_LIST PortOverrides;

  PortOverrides = NULL;

  for (i = 0; i < (State->TotalLinks * 2); i++) {

    ASSERT ((*State->PortList)[i & 0xFE].SelWidthOut == (*State->PortList)[ (i & 0xFE) + 1].SelWidthIn);
    ASSERT ((*State->PortList)[i & 0xFE].SelWidthIn == (*State->PortList)[ (i & 0xFE) + 1].SelWidthOut);
    ASSERT ((*State->PortList)[i & 0xFE].SelFrequency == (*State->PortList)[ (i & 0xFE) + 1].SelFrequency);

    if ((*State->PortList)[i].SelRegang) {
      ASSERT ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU);
      ASSERT ((*State->PortList)[i].Link < 4);
      State->Nb->SetLinkRegang (
        (*State->PortList)[i].NodeID,
        (*State->PortList)[i].Link,
        State->Nb
        );
    }

    //
    // IDS port override for CPUs and IO Devices
    //
    pf_HtIdsGetPortOverride ((BOOLEAN) ((i & 1) == 0), &(*State->PortList)[i], &(*State->PortList)[i + 1], &PortOverrides, State);

    LinkBase = (*State->PortList)[i].Pointer;
    if (((*State->PortList)[i].Type == PORTLIST_TYPE_IO) && ((*State->PortList)[i].Link == 1)) {
      LinkBase.Address.Register += HTSLAVE_LINK01_OFFSET;
    }

    // HT CRC Feature, set if configured.  The default is not to set it, because with some chipsets it
    // will lock up if done here.
    if (State->IsSetHtCrcFlood) {
      Temp = 1;
      Reg = LinkBase;
      Reg.Address.Register += HTHOST_LINK_CONTROL_REG;
      State->HtFeatures->SetHtControlRegisterBits (Reg, 1, 1, &Temp, State);
      if ((*State->PortList)[i].Type == PORTLIST_TYPE_IO) {
        // IO Devices also need to have SERR enabled.
        Reg = LinkBase;
        Reg.Address.Register = PCI_CONFIG_COMMAND_REG04;
        LibAmdPciWriteBits (Reg, 8, 8, &Temp, State->ConfigHandle);
      }
    }

    // Some IO devices don't work properly when setting widths, so write them in a single operation,
    // rather than individually.
    //
    Widthout = ConvertWidthToBits ((*State->PortList)[i].SelWidthOut);
    ASSERT (Widthout == 1 || Widthout == 0 || Widthout == 5 || Widthout == 4);
    Widthin = ConvertWidthToBits ((*State->PortList)[i].SelWidthIn);
    ASSERT (Widthin == 1 || Widthin == 0 || Widthin == 5 || Widthin == 4);

    Temp = (Widthin & 7) | ((Widthout & 7) << 4);
    Reg = LinkBase;
    Reg.Address.Register += HTHOST_LINK_CONTROL_REG;
    State->HtFeatures->SetHtControlRegisterBits (Reg, 31, 24, &Temp, State);

    Temp = (*State->PortList)[i].SelFrequency;
    IDS_HDT_CONSOLE (HT_TRACE, "Link Frequency: Node %02d: Link %02d: is running at %2d00MHz\n",
        (*State->PortList)[i].NodeID, (*State->PortList)[i].Link,
        (Temp < HT_FREQUENCY_800M) ? Temp + 2 : ((Temp < HT_FREQUENCY_2800M) ? 2 * (Temp - 1) : 2 * (Temp - 3)));

    if ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) {
      State->Nb->SetLinkFrequency (
        (*State->PortList)[i].NodeID,
        (*State->PortList)[i].Link,
        (UINT8)Temp,
        State->Nb
        );
    } else {
      ASSERT (Temp <= HT_FREQUENCY_2600M);
      // Write the frequency setting
      Reg = LinkBase;
      Reg.Address.Register += HTSLAVE_FREQ_REV_0_REG;
      SetHtIoFrequencyRegisterBits (Reg, 11, 8, &Temp, State);

      // Handle additional HT3 frequency requirements, if needed,
      // or clear them if switching down to ht1 on a warm reset.
      // Gen1 = 200Mhz -> 1000MHz, Gen3 = 1200MHz -> 2600MHz
      //
      // Even though we assert if debugging, we need to check that the capability was
      // found always, since this is an unknown hardware device, also we are taking
      // unqualified frequency from the external interface (could be trying to do ht3
      // on an ht1 IO device).
      //

      if (Temp > HT_FREQUENCY_1000M) {
        // Enabling features if gen 3
        Bits = 1;
      } else {
        // Disabling features if gen 1
        Bits = 0;
      }

      // Retry Enable
      if (DoesDeviceHaveHtSubtypeCap (LinkBase, HT_RETRY_CAPABILITY, &CurrentPtr, State)) {
        ASSERT ((*State->PortList)[i].Link < 2);
        CurrentPtr.Address.Register += HTRETRY_CONTROL_REG;
        LibAmdPciWriteBits (CurrentPtr,
                            ((*State->PortList)[i].Link * 16),
                            ((*State->PortList)[i].Link * 16),
                            &Bits,
                            State->ConfigHandle);
      } else {
        //  If we are turning it off, that may mean the device was only ht1 capable,
        // so don't complain that we can't do it.
        //
        if (Bits != 0) {
          NotifyWarningOptRequiredCapRetry ((*State->PortList)[i].NodeID,
                                            (*State->PortList)[i].HostLink,
                                            (*State->PortList)[i].HostDepth,
                                            State);
        }
      }

      // Scrambling enable
      if (DoesDeviceHaveHtSubtypeCap (LinkBase, HT_GEN3_CAPABILITY, &CurrentPtr, State)) {
        ASSERT ((*State->PortList)[i].Link < 2);
        CurrentPtr.Address.Register = CurrentPtr.Address.Register +
          HTGEN3_LINK_TRAINING_0_REG +
          ((*State->PortList)[i].Link * HTGEN3_LINK01_OFFSET);
        LibAmdPciWriteBits (CurrentPtr, 3, 3, &Bits, State->ConfigHandle);
      } else {
        //  If we are turning it off, that may mean the device was only ht1 capable,
        // so don't complain that we can't do it.
        //
        if (Bits != 0) {
          NotifyWarningOptRequiredCapGen3 ((*State->PortList)[i].NodeID,
                                           (*State->PortList)[i].HostLink,
                                           (*State->PortList)[i].HostDepth,
                                           State);
        }
      }
    }
    // Enable Unit ID Clumping if supported.
    if (State->IsUsingUnitIdClumping) {
      if (((*State->PortList)[i].ClumpingSupport != HT_CLUMPING_PASSIVE) &&
          ((*State->PortList)[i].ClumpingSupport != HT_CLUMPING_DISABLE)) {
        Bits = (*State->PortList)[i].ClumpingSupport;
        if ((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) {
          State->Nb->SetLinkUnitIdClumping (
            (*State->PortList)[i].NodeID,
            (*State->PortList)[i].Link,
            (*State->PortList)[i].ClumpingSupport,
            State->Nb
            );
        } else {
          if (DoesDeviceHaveHtSubtypeCap (LinkBase, HT_UNITID_CAPABILITY, &Reg, State)) {
            Reg.Address.Register += HTUNIT_ENABLE_REG;
            LibAmdPciWriteBits (Reg, 31, 0, &Bits, State->ConfigHandle);
          } else {
            // If we found one when gathering support, we have to find one now.
            ASSERT (FALSE);
          }
        }
      }
    }
  }
}

/*------------------------------------------------------------------------------------------*/
/**
 * Find a specific HT capability type.
 *
 * Search all the PCI Config space capabilities on any type of device for an
 * HT capability of the specific subtype.
 *
 * @param[in]  DevicePointer     A PCI Config address somewhere in the device config space
 * @param[in]  CapSubType        The HT capability subtype to find
 * @param[out] CapabilityBase    The Config space base address of the capability, if found.
 * @param[in]  State             Our State
 *
 * @retval TRUE     the capability was found
 * @retval FALSE    the capability was not found
 */
BOOLEAN
DoesDeviceHaveHtSubtypeCap (
  IN       PCI_ADDR         DevicePointer,
  IN       UINT8            CapSubType,
     OUT   PCI_ADDR         *CapabilityBase,
  IN       STATE_DATA       *State
  )
{
  BOOLEAN IsFound;
  BOOLEAN IsDone;
  PCI_ADDR Reg;
  UINT32 Temp;
  UINT32 RegSubType;
  UINT32 RegSubTypeMask;

  // Set the PCI Config Space base and the match value.
  IsFound = FALSE;
  IsDone = FALSE;
  Reg = DevicePointer;
  Reg.Address.Register = 0;
  if (CapSubType < (HT_HOST_CAPABILITY + 1)) {
    // HT Interface sub type
    RegSubType = ((UINT32) (CapSubType << 29) | (UINT32)8);
    RegSubTypeMask = HT_INTERFACE_CAP_SUBTYPE_MASK;
  } else {
    // Other HT capability subtype
    RegSubType = ((UINT32) (CapSubType << 27) | (UINT32)8);
    RegSubTypeMask = HT_CAP_SUBTYPE_MASK;
  }
  (*CapabilityBase).AddressValue = (UINT32)ILLEGAL_SBDFO;

  // Find it
  do {
    LibAmdPciFindNextCap (&Reg, State->ConfigHandle);
    if (Reg.AddressValue != (UINT32)ILLEGAL_SBDFO) {
      LibAmdPciRead (AccessWidth32, Reg, &Temp, State->ConfigHandle);
      // HyperTransport and subtype capability ?
      if ((Temp & RegSubTypeMask) == RegSubType) {
        *CapabilityBase = Reg;
        IsFound = TRUE;
      }
      // Some other capability, keep looking
    } else {
      //  Not there
      IsDone = TRUE;
    }
  } while (!IsFound && !IsDone);

  return IsFound;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Retry must be enabled on all coherent links if it is enabled on any coherent links.
 *
 * @HtFeatMethod{::F_SET_LINK_DATA}
 *
 * Effectively, this means HT3 on some links cannot be mixed with HT1 on others.
 * Scan the CPU to CPU links for this condition and limit those frequencies to HT1
 * if it is detected.
 * (Non-coherent links are independent.)
 *
 * @param[in,out]   State       global state, port frequency settings.
 *
 * @retval          TRUE        Fixup occurred, all coherent links HT1
 * @retval          FALSE       No changes
 */
BOOLEAN
IsCoherentRetryFixup (
  IN       STATE_DATA       *State
  )
{
  UINT8 Freq;
  UINT8 i;
  UINT8 DetectedFrequencyState;
  BOOLEAN IsMixed;
  UINT32 Temp;

  //
  // detectedFrequencyState:
  //   0 - initial state
  //   1 - HT1 Frequencies detected
  //   2 - HT3 Frequencies detected
  //
  IsMixed = FALSE;
  DetectedFrequencyState = 0;

  // Scan coherent links for a mix of HT3 / HT1
  for (i = 0; i < (State->TotalLinks * 2); i += 2) {
    if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) && ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_CPU)) {
      // At this point, Frequency of port [i+1] must equal [i], so just check one of them.
      switch (DetectedFrequencyState) {
      case 0:
        // Set current state to indicate what link frequency we found first
        if ((*State->PortList)[i].SelFrequency > HT_FREQUENCY_1000M) {
          // HT3 frequencies
          DetectedFrequencyState = 2;
        } else {
          // HT1 frequencies
          DetectedFrequencyState = 1;
        }
        break;
      case 1:
        // If HT1 frequency detected, fail any HT3 frequency
        if ((*State->PortList)[i].SelFrequency > HT_FREQUENCY_1000M) {
          IsMixed = TRUE;
        }
        break;
      case 2:
        // If HT3 frequency detected, fail any HT1 frequency
        if ((*State->PortList)[i].SelFrequency <= HT_FREQUENCY_1000M) {
          IsMixed = TRUE;
        }
        break;
      default:
        ASSERT (FALSE);
      }
      if (IsMixed) {
        // Don't need to keep checking after we find a mix.
        break;
      }
    }
  }

  if (IsMixed) {
    for (i = 0; i < (State->TotalLinks * 2); i += 2) {
      if (((*State->PortList)[i].Type == PORTLIST_TYPE_CPU) && ((*State->PortList)[i + 1].Type == PORTLIST_TYPE_CPU)) {
        // Limit coherent links to HT 1 frequencies.
        Temp = (*State->PortList)[i].CompositeFrequencyCap & (*State->PortList)[i + 1].CompositeFrequencyCap;
        Temp &= HT_FREQUENCY_LIMIT_HT1_ONLY;
        ASSERT (Temp != 0);
        (*State->PortList)[i].CompositeFrequencyCap = Temp;
        (*State->PortList)[i + 1].CompositeFrequencyCap = Temp;
        Freq = LibAmdBitScanReverse (Temp);
        (*State->PortList)[i].SelFrequency = Freq;
        (*State->PortList)[i + 1].SelFrequency = Freq;
      }
    }
  }
  return (IsMixed);
}
