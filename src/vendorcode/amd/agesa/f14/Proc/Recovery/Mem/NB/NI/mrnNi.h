/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnNi.h
 *
 * Northbridge Ni Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 34897 $ @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
 *
 **/
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

#ifndef _MRNNI_H_
#define _MRNNI_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_RJ8    ((UINT32)4 << (30 - 8))
#define MTRR_VALID  11

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemRecConstructNBBlockNi (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

VOID
MemRecNSwitchDctNi (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  );

VOID
MemRecNSwitchChannelNi (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  );

#endif  /* _MRNNI_H_ */


