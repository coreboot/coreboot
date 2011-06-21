/* $NoKeywords:$ */
/**
 * @file
 *
 * Implements the workaround for encrypted microcode patch loading.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x14/ON
 * @e \$Revision: 37004 $   @e \$Date: 2010-08-28 02:23:00 +0800 (Sat, 28 Aug 2010) $
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

VOID
F14NbBufferAllocationAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );


/// define NB buffer allocation setting
typedef struct _NB_BUFFER_ALLOCATION {
  D18F3x6C_STRUCT D18F3x6C; ///< reg for D18F3x6C
  D18F3x74_STRUCT D18F3x74; ///< reg for D18F3x74
  D18F3x7C_STRUCT D18F3x7C; ///< reg for D18F3x7C
  D18F3x17C_STRUCT D18F3x17C; ///< reg for D18F3x17C
} NB_BUFFER_ALLOCATION;

/// enum for ON Erratum 463 wrokaround
typedef enum {
  ON_ERRATUM463_WORKAROUND_DISABLE = 0, ///< work around disable
  ON_ERRATUM463_WORKAROUND_ENABLE = 1, ///< work around enable
} ON_ERRATUM463_WORKAROUND;

