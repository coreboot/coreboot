/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe port indirect register
 * space.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "PciePortRegAcc.h"
#include  "GnbCommonLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEPORTREGACC_FILECODE
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


/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe port indirect register.
 *
 * Support for unify register access through index/data pair on PCIe port
 *
 * @param[in]  Engine      Pointer to Engine descriptor for this port
 * @param[in]  Address     Register address
 * @param[in]  Pcie        Pointer to internal configuration data area
 * @retval                 Register Value
 */

UINT32
PciePortRegisterRead (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT16                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  GnbLibPciWrite (Engine->Type.Port.Address.AddressValue | 0xE0, AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciRead (Engine->Type.Port.Address.AddressValue | 0xE4, AccessWidth32, &Value, GnbLibGetHeader (Pcie));
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe Port Indirect register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Engine      Pointer to Engine descriptor for this port
 * @param[in]  Address     Register address
 * @param[in]  Value       New register value
 * @param[in]  S3Save      Save for S3 flag
 * @param[in]  Pcie        Pointer to internal configuration data area
 */
VOID
PciePortRegisterWrite (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT16                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  ASSERT (S3Save == TRUE || S3Save == FALSE);

  IDS_HDT_CONSOLE (PCIE_PORTREG_TRACE, "    *WR PCIEIND_P (%d:%d:%d):0x%04x = 0x%08x\n",
    Engine->Type.Port.Address.Address.Bus,
    Engine->Type.Port.Address.Address.Device,
    Engine->Type.Port.Address.Address.Function,
    Address,
    Value
    );
  GnbLibPciWrite (Engine->Type.Port.Address.AddressValue | 0xE0, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciWrite (Engine->Type.Port.Address.AddressValue | 0xE4, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Value, GnbLibGetHeader (Pcie));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe Port Indirect register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Engine      Pointer to Engine descriptor for this port
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  S3Save      Save for S3 flag
 * @param[in]  Value       New register value
 * @param[in]  Pcie        Pointer to internal configuration data area
 */

VOID
PciePortRegisterWriteField (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT16                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Data;
  UINT32  Mask;
  Data = PciePortRegisterRead (Engine, Address, Pcie);
  Mask = (1 << FieldWidth) - 1;
  Value &= Mask;
  Data &= (~(Mask << FieldOffset));
  PciePortRegisterWrite (Engine, Address, Data | (Value << FieldOffset), S3Save, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe Port Indirect register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Engine      Pointer to Engine descriptor for this port
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  Pcie        Pointer to internal configuration data area
 * @retval                 Register Field Value.
 */

UINT32
PciePortRegisterReadField (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT16                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PciePortRegisterRead (Engine, Address, Pcie);
  Value = (Value >> FieldOffset) & ((1 << FieldWidth) - 1);
  return  Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCIe port register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Engine      Pointer to Engine descriptor for this port
 * @param[in]  Address     Register address
 * @param[in]  AndMask     Value & (~AndMask)
 * @param[in]  OrMask      Value | OrMask
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */

VOID
PciePortRegisterRMW (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT16                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PciePortRegisterRead (Engine, Address, Pcie);
  Value = (Value & (~AndMask)) | OrMask;
  PciePortRegisterWrite (Engine, Address, Value, S3Save, Pcie);
}

