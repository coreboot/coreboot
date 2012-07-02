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

