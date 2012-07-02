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
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
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


