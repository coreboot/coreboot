/**
 * @file
 *
 * NB IOAPIC Initialization.
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*! \var APIC_DEVICE_INFO gDefaultApicDeviceInfoTable[]
 *  \brief  Default IO APIC interrupt mapping
 *  \details
 *  @li Interrupt Info for HT referenced as gDefaultApicDeviceInfoTable[0]
 *  @li Interrupt Info for IOMMU referenced as gDefaultApicDeviceInfoTable[1]
 *  @li Interrupt Info for device 2 referenced as gDefaultApicDeviceInfoTable[2]
 *  @li Interrupt Info for device 3 referenced as gDefaultApicDeviceInfoTable[3]
 *  @li ...
 *  @li Interrupt Info for device 13 can be referenced as gDefaultApicDeviceInfoTable[13]
 */
CONST APIC_DEVICE_INFO gDefaultApicDeviceInfoTable[] = {
// Group  Swizzling   Port Int Pin
  {0,     0,          31},   //HT
  {0,     0,          31},   //IOMMU
  {0,     ABCD,       28},   //Dev2  Grp0 [Int - 0..3]
  {1,     ABCD,       28},   //Dev3  Grp1 [Int - 4..7]
  {5,     ABCD,       28},   //Dev4  Grp5 [Int - 20..23]
  {5,     CDAB,       28},   //Dev5  Grp5 [Int - 20..23]
  {6,     BCDA,       29},   //Dev6  Grp6 [Int - 24..27]
  {6,     CDAB,       29},   //Dev7  Grp6 [Int - 24..27]
  {0,     0,          0 },   // Reserved
  {6,     ABCD,       29},   //Dev9  Grp6 [Int - 24..27]
  {5,     BCDA,       30},   //Dev10 Grp5 [Int - 20..23]
  {2,     ABCD,       30},   //Dev11 Grp2 [Int - 8..11]
  {3,     ABCD,       30},   //Dev12 Grp3 [Int - 12..15]
  {4,     ABCD,       30}    //Dev13 Grp4 [Int - 16..19]
};

CONST APIC_REGISTER_INFO gApicRegisterInfoTable[] = {
  {0,   NB_IOAPICCFG_REG03, 0,  NB_IOAPICCFG_REG06},  //Dev2
  {8,   NB_IOAPICCFG_REG03, 8,  NB_IOAPICCFG_REG06},  //Dev3
  {16,  NB_IOAPICCFG_REG03, 16, NB_IOAPICCFG_REG06},  //Dev4
  {24,  NB_IOAPICCFG_REG03, 24, NB_IOAPICCFG_REG06},  //Dev5
  {0,   NB_IOAPICCFG_REG04, 0,  NB_IOAPICCFG_REG07},  //Dev6
  {8,   NB_IOAPICCFG_REG04, 8,  NB_IOAPICCFG_REG07},  //Dev7
  {0,   0,                  0,  0                 },  //Dev8
  {16,  NB_IOAPICCFG_REG04, 24 ,NB_IOAPICCFG_REG07},  //Dev9
  {24,  NB_IOAPICCFG_REG04, 0  ,NB_IOAPICCFG_REG08},  //Dev10
  {0,   NB_IOAPICCFG_REG05, 8  ,NB_IOAPICCFG_REG08},  //Dev11
  {8,   NB_IOAPICCFG_REG05, 16 ,NB_IOAPICCFG_REG08},  //Dev12
  {16,  NB_IOAPICCFG_REG05, 24 ,NB_IOAPICCFG_REG08},  //Dev13
};


/*----------------------------------------------------------------------------------------*/
/**
 * Configure IO APIC
 *    Enable IO APIC base address decoding. Enable default forwarding interrupt to SB
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
NbLibSetIOAPIC (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  NB_CONFIG           *pNbConfig;
  PORT                PortId;
  APIC_DEVICE_INFO    ApicDeviceInfoTable[sizeof (gDefaultApicDeviceInfoTable) / sizeof (APIC_DEVICE_INFO)];
  APIC_REGISTER_INFO  *pApicRegisterInfoTable;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  pApicRegisterInfoTable = (APIC_REGISTER_INFO*)FIX_PTR_ADDR (&gApicRegisterInfoTable[0], NULL);
  if (pNbConfig->IoApicBaseAddress != 0 ) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "[NB]NbLibSetIOAPIC\n"));
    //Copy default routing to local memory buffer
    LibAmdMemCopy (&ApicDeviceInfoTable, (APIC_DEVICE_INFO*)FIX_PTR_ADDR (&gDefaultApicDeviceInfoTable[0], NULL), sizeof (ApicDeviceInfoTable), (AMD_CONFIG_PARAMS *)&(pNbConfig->sHeader));
    //Callback to platform BIOS to update
    LibNbCallBack (PHCB_AmdUpdateApicInterruptMapping, (UINTN)&ApicDeviceInfoTable, pConfig);
    //Setup base address
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    Apic Base %x\n", (UINT32)pNbConfig->IoApicBaseAddress & 0xffffff00));
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX, NB_IOAPICCFG_REG01, AccessS3SaveWidth32, (UINT32) (0xff), (UINT32)pNbConfig->IoApicBaseAddress & 0xffffff00, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX, NB_IOAPICCFG_REG02, AccessS3SaveWidth32, 0x0, ((UINT32*)&pNbConfig->IoApicBaseAddress)[1] , pConfig);
    //Setup interrupt mapping
    for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
      PCI_ADDR Port;
      APIC_REGISTER_INFO RegisterInfo;
      APIC_DEVICE_INFO  PortInfo;
      PORT  NativePortId;
      if (PortId == 8 || !PcieLibIsValidPortId (PortId, pConfig)) {
        continue;
      }
      NativePortId = PcieLibNativePortId (PortId, pConfig);
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      RegisterInfo = pApicRegisterInfoTable[NativePortId - MIN_PORT_ID];
      PortInfo = ApicDeviceInfoTable[Port.Address.Device];
      //Setup routing for EP
      LibNbPciIndexRMW (
        pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX,
        RegisterInfo.EpRoutingRegister,
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        (PortInfo.Group | (PortInfo.Swizzle << 4)) << RegisterInfo.EpRoutingOffset,
        pConfig
        );
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    EP Routing Dev[%d] NativePortId[%d] PortId[%d] Group - %d Swizzle - %d\n", Port.Address.Device, NativePortId, PortId, PortInfo.Group, PortInfo.Swizzle));
      //Setup routing for RC
      LibNbPciIndexRMW (
        pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX,
        RegisterInfo.RcRoutingRegister,
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        (PortInfo.Pin) << RegisterInfo.RcRoutingOffset,
        pConfig
        );
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NB_TRACE), "    RC Routing Dev[%d] NativeDev[%d] Pin - %d \n", Port.Address.Device, NativePortId, PortInfo.Pin));

    }
    LibNbPciIndexRMW (
      pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX,
      NB_IOAPICCFG_REG09,
      AccessS3SaveWidth32,
      0x0,
      ApicDeviceInfoTable[0].Pin | (ApicDeviceInfoTable[1].Pin << 8),
      pConfig
      );
    //Enable IO API MMIO decoding and configure features
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_IOAPICCFG_INDEX, NB_IOAPICCFG_REG00, AccessS3SaveWidth32, 0x0 , 0x1f , pConfig);
  }
}
