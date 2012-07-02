/* $NoKeywords:$ */
/**
 * @file
 *
 * mmConditionalPso.c
 *
 * Functions to support conditional entries in the Platform Specific Override Table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMCONDITIONALPSO_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

#define PSO_TYPE    0
#define PSO_LENGTH  1
#define PSO_DATA    2

typedef enum _PSO_STATE {
  PSO_FIND_CONDITION = 100,  // Searching for initial Condition statement
  PSO_FIND_ACTION,           // Searching for initial Action Statement
  PSO_MATCH_ACTION,          // Trying to find an action that matches the caller's request
  PSO_CHECK_CONDITION,       // Checking the condition that preceded the found action
  PSO_DO_ACTION,             // Performing Action
  PSO_COMPLETE               // Completed processing of this request
} PSO_STATE;

typedef struct _D3_CMP_CAL {
  UINT32 D3Cmp0NCal   :3;
  UINT32 Reserved34   :2;
  UINT32 D3Cmp0PCal   :3;
  UINT32 Reserved89   :2;
  UINT32 D3Cmp1NCal   :3;
  UINT32 Reserved1314 :2;
  UINT32 D3Cmp1PCal   :3;
  UINT32 Reserved1819 :2;
  UINT32 D3Cmp2NCal   :3;
  UINT32 Reserved2324 :2;
  UINT32 D3Cmp2PCal   :3;
  UINT32 Reserved2831 :2;
} D3_CMP_CAL;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
 STATIC
 MemPSODoActionODT (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionAddrTmg (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionODCControl (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionSlewRate (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

BOOLEAN
STATIC
MemPSODoActionGetFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

BOOLEAN
STATIC
MemCheckRankType (
  IN       CH_DEF_STRUCT *CurrentChannel,
  IN       UINT16 RankType
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *      Process Conditional Platform Specific Overrides
 *
 *      @param[in] PlatformMemoryConfiguration - Pointer to Platform config table
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] PsoAction - Action type
 *      @param[in] Dimm - Dimm Number
 *
 *      @return          BOOLEAN -            TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemProcessConditionalOverrides (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 PsoAction,
  IN       UINT8 Dimm
  )
{
  BOOLEAN Result;
  MEM_TECH_BLOCK *TechPtr;
  UINT8 *Buffer;
  UINT8 *ConditionStartPtr;
  UINT8 *ActionStartPtr;
  UINT8 *SpdBufferPtr;
  UINT8 i;
  UINT8 DimmMask;
  UINT8 CurDimmMask;
  BOOLEAN Condition;
  BOOLEAN TmpCond;
  PSO_STATE State;
  ASSERT (PlatformMemoryConfiguration != NULL);
  ASSERT (NBPtr != NULL);
  ASSERT ((PsoAction >= PSO_ACTION_MIN) && (PsoAction <= PSO_ACTION_MAX));
  //
  // Set up local data
  //
  TechPtr = NBPtr->TechPtr;
  Buffer = PlatformMemoryConfiguration;
  State = PSO_FIND_CONDITION;
  ConditionStartPtr = NULL;
  ActionStartPtr = NULL;
  Condition = FALSE;
  DimmMask = 0xFF;
  CurDimmMask = 0xFF;
  Result = FALSE;

  if (Dimm != 0xFF) {
    DimmMask = ( 1 << Dimm);
  }
  DimmMask &= (UINT8) (NBPtr->ChannelPtr->ChDimmValid & 0xFF);
  if (DimmMask == 0) {
    return Result;
  }

  //
  // Search for Condition Entry
  //
  while (State != PSO_COMPLETE) {
    switch (State) {
    //
    // Searching for initial Condition statement
    //
    case PSO_FIND_CONDITION:
      ASSERT (Buffer != NULL);
      while (Buffer[PSO_TYPE] != PSO_CONDITION_AND) {
        //
        // If end of table is reached, Change state to complete and break.
        //
        if (Buffer[PSO_TYPE] == PSO_END) {
          State = PSO_COMPLETE;
          break;
        }
        //
        // Otherwise, increment Buffer Pointer to the next PSO entry.
        //
        Buffer += (Buffer[PSO_LENGTH] + 2);
      }
      //
      // If Condition statement has been found, save the Condition Start Pointer,
      //  and change to next state
      //
      if (State != PSO_COMPLETE) {
        ASSERT (Buffer != NULL);
        State = PSO_FIND_ACTION;
        ConditionStartPtr = Buffer;
        Buffer += (Buffer[PSO_LENGTH] + 2);
      }
      break;
    //
    // Searching for an action that matches the caller's request
    //
    case PSO_FIND_ACTION:
      ASSERT (Buffer != NULL);
      while (Buffer[PSO_TYPE] != PsoAction) {
        //
        // If non-conditional entry, change state to complete and break.
        //
        if ((Buffer[PSO_TYPE] < CONDITIONAL_PSO_MIN) || (Buffer[PSO_TYPE] > CONDITIONAL_PSO_MAX)) {
          State = PSO_COMPLETE;
          break;
        }
        //
        // Check for the Start of a new condition block
        //
        if (Buffer[PSO_TYPE] == PSO_CONDITION_AND) {
          ConditionStartPtr = Buffer;
        }
        //
        // Otherwise, increment buffer pointer to the next PSO entry.
        //
        Buffer += (Buffer[PSO_LENGTH] + 2);
      }
      //
      // If Action statement has been found, Save the Action Start Pointer, Reset Buffer to Condition Start
      //  and Change to next state.
      //
      if (State != PSO_COMPLETE) {
        State = PSO_CHECK_CONDITION;
        ASSERT (Buffer != NULL);
        ActionStartPtr = Buffer;
        Buffer = ConditionStartPtr;
        Condition = TRUE;
      }
      break;
    //
    // Checking the condition that preceded the found action
    //
    case PSO_CHECK_CONDITION:
      ASSERT (Buffer != NULL);
      //
      // Point to the next Condition
      //
      Buffer += (Buffer[PSO_LENGTH] + 2);
      ASSERT ((Buffer[PSO_TYPE] >= CONDITIONAL_PSO_MIN) && (Buffer[PSO_TYPE] <= CONDITIONAL_PSO_MAX));
      //
      // This section has already been checked for invalid statements so just exit on ACTION_xx
      //
      if ((Buffer[PSO_TYPE] >= PSO_ACTION_MIN) && (Buffer[PSO_TYPE] <= PSO_ACTION_MAX)) {
        if (Condition) {
          ASSERT (Buffer != NULL);
          State = PSO_DO_ACTION;              // Perform the Action
        } else {
          State = PSO_FIND_CONDITION;         // Go back and look for another condition/action
        }
        Buffer = ActionStartPtr;              // Restore Action Pointer
        break;
      }
      switch (Buffer[PSO_TYPE]) {

      case PSO_CONDITION_AND:
        //
        // Additional CONDITION_AND is ORed with Previous ones, so if Previous result is TRUE
        //  just restore action pointer and perform the action.
        //
        if (Condition) {
          State = PSO_DO_ACTION;
          Buffer = ActionStartPtr;
        } else {
          //
          // If its false, Start over and evaluate next cond.
          // reset the Current Dimm Mask
          //
          Condition = TRUE;
          CurDimmMask = 0xFF;
        }
        break;

      case PSO_CONDITION_LOC:
        //
        // Condition location
        //
        CurDimmMask = Buffer[4];
        Condition &= ( ((Buffer[2] & (1 << (NBPtr->MCTPtr->SocketId))) != 0) &&
                       ((Buffer[3] & (1 << (NBPtr->ChannelPtr->ChannelID))) != 0) &&
                       ((CurDimmMask & DimmMask) != 0) );
        break;

      case PSO_CONDITION_SPD:
        //
        // Condition SPD
        //
        TmpCond = FALSE;
        for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i ++) {
          if ( ((DimmMask & CurDimmMask) & ((UINT16) (1 << i))) != 0) {
            if (TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, i)) {
              TmpCond |= ( (SpdBufferPtr[Buffer[2]] & Buffer[3]) == Buffer[4]);
            }
          }
        }
        Condition &= TmpCond;
        break;

      case PSO_CONDITION_REG:
        //
        // Condition Register - unsupported at this time
        //
        break;

      default:
        ASSERT (FALSE);
      } // End Condition Switch
    break;

    case PSO_DO_ACTION:
      ASSERT (Buffer != NULL);
      //
      // Performing Action
      //
      if ((Buffer[PSO_TYPE] < PSO_ACTION_MIN) || (Buffer[PSO_TYPE] > PSO_ACTION_MAX)) {
        State = PSO_COMPLETE;
      }
      if (Buffer[PSO_TYPE] == PsoAction) {
        switch (Buffer[PSO_TYPE]) {
        case PSO_ACTION_ODT:
          Result = MemPSODoActionODT (NBPtr, &Buffer[PSO_DATA]);
          break;
        case PSO_ACTION_ADDRTMG:
          Result = MemPSODoActionAddrTmg (NBPtr, &Buffer[PSO_DATA]);
          break;
        case PSO_ACTION_ODCCONTROL:
          Result = MemPSODoActionODCControl (NBPtr, &Buffer[PSO_DATA]);
          break;
        case PSO_ACTION_SLEWRATE:
          Result = MemPSODoActionSlewRate (NBPtr, &Buffer[PSO_DATA]);
          break;
        case PSO_ACTION_SPEEDLIMIT:
          Result = MemPSODoActionGetFreqLimit (NBPtr, &Buffer[PSO_DATA]);
          break;
        case PSO_ACTION_REG:
          break;
        default:
          ASSERT (FALSE);
        } // End Action Switch
        //
        // If Action was performed, mark complete.
        //
        if (Result) {
          State = PSO_COMPLETE;
        }
    }// End Action

      //
      // Point to the next PSO Entry
      //
      Buffer += (Buffer[PSO_LENGTH] + 2);
      break;

    case PSO_COMPLETE:
      //
      // Completed processing of this request
      //
      break;

    default:
      ASSERT (FALSE);
    } // End State Switch

  } // End While

  return Result;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *      Perform ODT Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return         BOOLEAN -             TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionODT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  BOOLEAN Result;
  UINT32 Speed;
  UINT8 Dimms;
  UINT8 i;
  UINT8 QR_Dimms;
  Result = FALSE;
  Speed = ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66));
  Dimms = NBPtr->ChannelPtr->Dimms;
  QR_Dimms = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if (((NBPtr->ChannelPtr->DimmQrPresent & (UINT16) (1 << i)) != 0) && (i < 2)) {
      QR_Dimms ++;
    }
  }
  if ((Speed & ((UINT32 *) Buffer)[0]) != 0) {
    if ((((UINT8) (1 << (Dimms - 1)) & Buffer[4]) != 0) || (Buffer[4] == ANY_NUM)) {
      if (((QR_Dimms == 0) && (Buffer[5] == NO_DIMM)) ||
          ((QR_Dimms > 0) && (((UINT8) (1 << (QR_Dimms - 1)) & Buffer[5]) != 0)) ||
           (Buffer[5] == ANY_NUM)) {
        NBPtr->PsPtr->DramTerm = Buffer[6];
        NBPtr->PsPtr->QR_DramTerm = Buffer[7];
        NBPtr->PsPtr->DynamicDramTerm = Buffer[8];
        Result = TRUE;
        IDS_HDT_CONSOLE (MEM_FLOW, "      Platform Override: DramTerm:%02x, QRDramTerm:%02x, DynDramTerm:%02x\n", Buffer[6], Buffer[7], Buffer[8]);
      }
    }
  }
  return Result;
 }

 /* -----------------------------------------------------------------------------*/
/**
 *      Perform Address Timing Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -             TRUE : Action was performed
 *                                             FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionAddrTmg (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  BOOLEAN Result;
  CH_DEF_STRUCT *ChannelPtr;
  UINT32 Speed;
  UINT16 DimmConfig;

  Result = FALSE;
  ChannelPtr = NBPtr->ChannelPtr;
  Speed = ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66));
  DimmConfig = *(UINT16 *) &(Buffer[4]);

  if ((Speed & ((UINT32 *) Buffer)[0]) != 0) {
    if (MemCheckRankType (ChannelPtr, DimmConfig)) {
      ChannelPtr->DctAddrTmg = *(UINT32*) &(Buffer[6]);
      Result = TRUE;
      IDS_HDT_CONSOLE (MEM_FLOW, "      Platform Override: Address Timing:%08x\n", *(UINT32*) &(Buffer[6]));
    }
  }
  return Result;
 }

  /* -----------------------------------------------------------------------------*/
/**
 *      Perform Drive Strength Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -             TRUE : Action was performed
 *                                             FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionODCControl (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  BOOLEAN Result;
  CH_DEF_STRUCT *ChannelPtr;
  UINT32 Speed;
  UINT16 DimmConfig;

  Result = FALSE;
  ChannelPtr = NBPtr->ChannelPtr;
  Speed = ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66));
  DimmConfig = *(UINT16 *) &(Buffer[4]);

  if ((Speed & ((UINT32 *) Buffer)[0]) != 0) {
    if (MemCheckRankType (ChannelPtr, DimmConfig)) {
      ChannelPtr->DctOdcCtl = *(UINT32*) &(Buffer[6]);
      Result = TRUE;
      IDS_HDT_CONSOLE (MEM_FLOW, "      Platform Override: ODC Control:%08x\n", *(UINT32*)&(Buffer[6]));
    }
  }
  return Result;
 }

  /* -----------------------------------------------------------------------------*/
/**
 *      Perform Slew Rate Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *     @return          BOOLEAN -             TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionSlewRate (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  BOOLEAN Result;
  CH_DEF_STRUCT *ChannelPtr;
  UINT32 Speed;
  UINT16 DimmConfig;

  Result = FALSE;
  ChannelPtr = NBPtr->ChannelPtr;
  Speed = ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66));
  DimmConfig = *(UINT16 *) &(Buffer[4]);

  if ((Speed & ((UINT32 *) Buffer)[0]) != 0) {
    if (MemCheckRankType (ChannelPtr, DimmConfig)) {
      MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp0NCal );
      MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp0PCal );
      MemNSetBitFieldNb (NBPtr, BFD3Cmp1NCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp1NCal );
      MemNSetBitFieldNb (NBPtr, BFD3Cmp1PCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp1PCal );
      MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp2NCal );
      MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, ((D3_CMP_CAL *) &(Buffer[6]))->D3Cmp2PCal );
      Result = TRUE;
      IDS_HDT_CONSOLE (MEM_FLOW, "      Platform Override: Slew Rate:%08x\n", *(UINT32 *) &(Buffer[6]));
    }
  }
  return Result;
 }

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the  POR supported speed for a specific config
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -            TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 */
BOOLEAN
STATIC
MemPSODoActionGetFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  BOOLEAN Result;
  CH_DEF_STRUCT *ChannelPtr;
  DCT_STRUCT *DCTPtr;
  UINT16 DimmConfig;
  UINT16 SpeedLimit;

  Result = FALSE;
  ChannelPtr = NBPtr->ChannelPtr;
  DCTPtr = NBPtr->DCTPtr;
  DimmConfig = *(UINT16*) &(Buffer[0]);
  SpeedLimit = 0;
  //
  // Match number of dimms, then Rank Type
  //
  if (ChannelPtr->Dimms == Buffer[2]) {
    if (MemCheckRankType (ChannelPtr, DimmConfig)) {
      //
      // Select speed based on current voltage
      //
      if (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) {
        SpeedLimit = *(UINT16*) &(Buffer[3]);
      } else if (NBPtr->RefPtr->DDR3Voltage == VOLT1_25) {
        SpeedLimit = *(UINT16*) &(Buffer[7]);
      } else {
        SpeedLimit = *(UINT16*) &(Buffer[5]);
      }
      //
      // Set the Speed limit
      //
      if (DCTPtr->Timings.TargetSpeed > SpeedLimit) {
        DCTPtr->Timings.TargetSpeed = SpeedLimit;
      }
      Result = TRUE;
      IDS_HDT_CONSOLE (MEM_FLOW, "      Platform Override: Max Memory Speed for Channel %d: %d\n", NBPtr->Channel, SpeedLimit);
    }
  }
  return Result;
}

 /* -----------------------------------------------------------------------------*/
/**
 *
 *    This function matches a particular Rank Type Mask to the installed
 *    DIMM configuration on the provided channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *     @param[in]       RankType              Mask of rank type to match
 *
 *     @return          BOOLEAN -             TRUE : Rank types match
 *                                            FALSE: Rank types do not match
 *
 */
BOOLEAN
STATIC
MemCheckRankType (
  IN       CH_DEF_STRUCT *CurrentChannel,
  IN       UINT16 RankType
  )
{
  BOOLEAN Result;
  UINT8 i;
  UINT16 DIMMRankType;

  DIMMRankType = MemAGetPsRankType (CurrentChannel);
  Result = TRUE;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if ( ((DIMMRankType & (0x0F << (i << 2))) + (RankType & (0x0F << (i << 2)))) != 0) {
      Result &= (((DIMMRankType & (0x0F << (i << 2))) & ( RankType & ( 0x0F << ( i << 2)))) != 0);
    }
    if (!Result) {
      break;
    }
  }
  return Result;
}