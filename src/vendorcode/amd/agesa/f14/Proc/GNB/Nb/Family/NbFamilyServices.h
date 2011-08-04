/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific service routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 47490 $   @e \$Date: 2011-02-22 08:34:28 -0700 (Tue, 22 Feb 2011) $
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

#ifndef _NBFAMILYSERVICES_H_
#define _NBFAMILYSERVICES_H_

/// Fuse field entry
typedef struct {
  UINT8                 FieldOffset;              ///< Field offset in fuse register
  UINT8                 FieldWidth;               ///< Width of field
  UINT16                FuseOffset;               ///< destination offset in translation table
} FUSE_REGISTER_ENTRY;

/// Fuse register entry
typedef struct {
  UINT32                Register;                 ///< FCR register address
  UINT8                 FuseRegisterTableLength;  ///< Length of field table for this register
  FUSE_REGISTER_ENTRY   *FuseRegisterTable;       ///< Pointer to field table
} FUSE_TABLE_ENTRY;

/// Fuse translation table
typedef struct {
  UINT8                 FuseTableLength;          ///< Length of translation table
  FUSE_TABLE_ENTRY      *FuseTable;               ///< Pointer to register table
} FUSE_TABLE;

/// NB power gate configuration
typedef struct {
  struct {
    UINT32  GmcPowerGate:1;               ///< Power Gate GMC
    UINT32  GfxPowerGate:1;               ///< Power gate GFX
    UINT32  UvdPowerGate:1;               ///< Power gate UVD
  } Services;                             ///< Power gate services
  POWER_GATE_DATA   Gmc;                  ///< Gmc Power gating Data
  POWER_GATE_DATA   Uvd;                  ///< Uvd Power gating Data
} NB_POWERGATE_CONFIG;

VOID
NbFmNbClockGating (
  IN OUT   VOID  *NbClkGatingCtrl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbFmClumpUnitID (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

FUSE_TABLE*
NbFmGetFuseTranslationTable (
  VOID
  );

VOID
NbFmFuseAdjustFuseTablePatch (
  IN OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

UINT32
NbFmDpmStateBootupInit (
  IN       UINT32              LclkDpmValid,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
NbFmInitLclkDpmRcActivity (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );
#endif

