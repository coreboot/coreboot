/* $NoKeywords:$ */
/**
 * @file
 *
 * NB power management features
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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

#ifndef _NBPOWERMGMT_H_
#define _NBPOWERMGMT_H_


AGESA_STATUS
NbInitPowerManagement (
  IN      GNB_PLATFORM_CONFIG *Gnb
  );

///Control structure for clock gating feature
typedef struct {
  BOOLEAN Smu_Sclk_Gating;    ///<Control Smu SClk gating 1 Enable 0 Disable
  BOOLEAN Smu_Lclk_Gating;    ///<Control Smu LClk gating 1 Enable 0 Disable
  BOOLEAN Orb_Sclk_Gating;    ///<Control ORB SClk gating 1 Enable 0 Disable
  BOOLEAN Orb_Lclk_Gating;    ///<Control ORB LClk gating 1 Enable 0 Disable
  BOOLEAN Ioc_Sclk_Gating;    ///<Control IOC SClk gating 1 Enable 0 Disable
  BOOLEAN Ioc_Lclk_Gating;    ///<Control IOC LClk gating 1 Enable 0 Disable
  BOOLEAN Bif_Sclk_Gating;    ///<Control BIF SClk gating 1 Enable 0 Disable
  BOOLEAN Gmc_Sclk_Gating;    ///<Control GMC SClk gating 1 Enable 0 Disable
  BOOLEAN Dce_Sclk_Gating;    ///<Control DCE SClk gating 1 Enable 0 Disable
  BOOLEAN Dce_Dispclk_Gating;    ///<Control DCE dispaly gating 1 Enable 0 Disable
} NB_CLK_GATING_CTRL;

#endif
