/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 49532 $   @e \$Date: 2011-03-25 02:54:43 +0800 (Fri, 25 Mar 2011) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieFamServices.h"
#include  "LlanoDefinitions.h"
#include  "LlanoComplexData.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_FAMILY_LN_F12PCIECOMPLEXCONFIG_FILECODE
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
 * Get total number of silicons/wrappers/engines for this complex
 *
 *
 * @param[in]  SocketId         Socket ID.
 * @param[out] Length           Length of configuration info block
 * @param[in]  StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    Configuration data length is correct
 */
AGESA_STATUS
PcieFmGetComplexDataLength (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  *Length = sizeof (ComplexData);
  return  AGESA_SUCCESS;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Build configuration
 *
 *
 * @param[in]  SocketId         Socket ID
 * @param[out] Buffer           Pointer to buffer to build internal complex data structure
 * @param[in]  StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    Configuration data build successfully
 */
AGESA_STATUS
PcieFmBuildComplexConfiguration (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  LibAmdMemCopy (Buffer, &ComplexData, sizeof (ComplexData), StdHeader);
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 *  get native PHY lane bitmap
 *
 *
 * @param[in]  PhyLaneBitmap  Package PHY lane bitmap
 * @param[in]  Engine         Standard configuration header.
 * @retval     Native PHY lane bitmap
 */
UINT32
PcieFmGetNativePhyLaneBitmap (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  )
{
  return PhyLaneBitmap;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Get SB port info
 *
 *
 * @param[out] SocketId         Socket ID
 * @param[out] SbPort           Pointer to SB configuration descriptor
 * @param[in]  StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    SB configuration determined successfully
 */
AGESA_STATUS
PcieFmGetSbConfigInfo (
  IN       UINT8                         SocketId,
     OUT   PCIe_PORT_DESCRIPTOR          *SbPort,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  LibAmdMemCopy (SbPort, &DefaultSbPort, sizeof (PCIe_PORT_DESCRIPTOR), StdHeader);
  return AGESA_SUCCESS;
}

