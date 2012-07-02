/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe definitions
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
#ifndef _PCIECOMPLEXDATATN_H_
#define _PCIECOMPLEXDATATN_H_

#define SOCKET_ID             0

#define MAX_NUM_PHYs          2
#define MAX_NUM_LANE_PER_PHY  8

#define NUMBER_OF_PORTS     8
#define NUMBER_OF_GPP_PORTS 5
#define NUMBER_OF_GFX_PORTS 2
#define NUMBER_OF_GFX_DDIS  3
#define NUMBER_OF_DDIS      2
#define NUMBER_OF_DDIS2     1
#define NUMBER_OF_WRAPPERS  3
#define NUMBER_OF_SILICONS  1

#define GFX_WRAP_ID         1
#define GFX_NUMBER_OF_PIFs  2
#define GFX_START_PHY_LANE  8
#define GFX_END_PHY_LANE    23
#define GFX_CORE_ID         2

#define GFX_CORE_x16        ((16 << 8) | 0)
#define GFX_CORE_x8x8       ((8 << 8) | 8)

#define GPP_WRAP_ID         0
#define GPP_NUMBER_OF_PIFs  1
#define GPP_START_PHY_LANE  0
#define GPP_END_PHY_LANE    7
#define GPP_CORE_ID         1

#define GPP_CORE_x4x1x1x1x1   ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1     ((2ull << 32) | (1ull << 24) | (1ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1_ST  ((2ull << 32) | (0ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2       ((2ull << 32) | (2ull << 24) | (0ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2_ST    ((2ull << 32) | (0ull << 24) | (2ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x4         ((4ull << 32) | (0ull << 24) | (0ull << 16) | (0ull << 8) | (4ull << 0))

#define DDI_WRAP_ID           2
#define DDI_NUMBER_OF_PIFs    1
#define DDI_START_PHY_LANE    24
#define DDI_END_PHY_LANE      31

#define DDI2_WRAP_ID          3
#define DDI2_NUMBER_OF_PIFs   1
#define DDI2_START_PHY_LANE   32
#define DDI2_END_PHY_LANE     38

///Gen2 capability
typedef enum {
  OscFuses,                   ///< Not capable
  OscRO,                      ///< Gen2 with RO
  OscLC,                      ///< Gen2 with LC
  OscDefault,                 ///< Skip initialization of OSC
} OSC_MODE;

///Family specific silicon configuration
typedef struct {
  OSC_MODE  OscMode;          ///<OSC mode
  UINT8     PortDevMap[6];    ///< Device number that has beed allocated already
} TN_PCIe_SILICON_CONFIG;


/// Complex Configuration
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                ///< Silicon
  PCIe_WRAPPER_CONFIG     GfxWrapper;             ///< Graphics Wrapper
  PCIe_WRAPPER_CONFIG     GppWrapper;             ///< General Purpose Port
  PCIe_WRAPPER_CONFIG     DdiWrapper;             ///< DDI
  PCIe_WRAPPER_CONFIG     Ddi2Wrapper;            ///< DDI
  PCIe_ENGINE_CONFIG      Port2;                  ///< Port 2
  PCIe_ENGINE_CONFIG      Port3;                  ///< Port 3
  PCIe_ENGINE_CONFIG      DpB;                    ///< DPB
  PCIe_ENGINE_CONFIG      DpC;                    ///< DPC
  PCIe_ENGINE_CONFIG      DpD;                    ///< DPD
  PCIe_ENGINE_CONFIG      Port4;                  ///< Port 4
  PCIe_ENGINE_CONFIG      Port5;                  ///< Port 5
  PCIe_ENGINE_CONFIG      Port6;                  ///< Port 6
  PCIe_ENGINE_CONFIG      Port7;                  ///< Port 7
  PCIe_ENGINE_CONFIG      Port8;                  ///< Port 8
  PCIe_ENGINE_CONFIG      DpE;                    ///< DPE
  PCIe_ENGINE_CONFIG      DpF;                    ///< DPF
  PCIe_ENGINE_CONFIG      DpA;                    ///< DPA
  TN_PCIe_SILICON_CONFIG  FmSilicon;               ///< Fm Silicon
} TN_COMPLEX_CONFIG;


#endif
