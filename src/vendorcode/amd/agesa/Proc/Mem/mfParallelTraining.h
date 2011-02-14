/* $NoKeywords:$ */
/**
 * @file
 *
 * mfParallelTraining.h
 *
 * Header file for the parallel training feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
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


#ifndef _MFPARALLELTRAINING_H_
#define _MFPARALLELTRAINING_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

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

typedef BOOLEAN (*REMOTE_NBBLOCK_CONSTRUCTOR) (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       DIE_STRUCT *MCTPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr
);

///< This structure defines the environment on the AP for parallel training
typedef struct _REMOTE_TRAINING_ENV {
  IN OUT AMD_CONFIG_PARAMS StdHeader;                ///< Config pointer of BSP
  IN OUT AGESA_STATUS (*GetPlatformCfg[MAX_PLATFORM_TYPES]) (struct _MEM_DATA_STRUCT *MemData, UINT8 SocketID, CH_DEF_STRUCT *CurrentChannel); ///< look-up platform info
  IN OUT BOOLEAN (*ErrorHandling)(struct _DIE_STRUCT *MCTPtr, UINT8 DCT, UINT16 ChipSelMask, AMD_CONFIG_PARAMS *StdHeader); ///< Error Handling
  IN REMOTE_NBBLOCK_CONSTRUCTOR NBBlockCtor;         ///< NB Block constructor
  IN MEM_FEAT_BLOCK_NB *FeatPtr;                     ///< Feature block pointer
  IN UINT8 *TableBasedAlterations;                   ///< Point to an array of data bytes describing desired modifications to register settings
  IN PSO_TABLE *PlatformMemoryConfiguration;                 ///< Point to platform config table
  IN UINT32 HoleBase;                                ///< Used for Memtyping
  IN UINT32 UmaSize;                               ///< Used for Memtyping
  IN UINT16 BottomIo;                                ///< Used for Memtyping
  IN UINT32 SysLimit;                                ///< Used for Memtyping
  IN UINT8  BspSocket;                               ///< Socket number of BSP
  IN UINT8  BspCore;                                 ///< Core number of BSP
  IN DIE_STRUCT DieStruct;                           ///< Remote copy of Die Struct
} REMOTE_TRAINING_ENV;

///< This structure defines Die information
typedef struct _DIE_INFO {
  IN OUT UINT8 Socket;                               ///< Socket number
  IN OUT UINT8 Core;                                 ///< Core number
  IN OUT BOOLEAN Training;                           ///< Training Flag, 1 = Training has been started on this core
} DIE_INFO;


/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemFParallelTraining (
  IN OUT   REMOTE_TRAINING_ENV *EnvPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
 );

#endif  /* _MFPARALLELTRAINING_H_ */


