/**
 * @file
 *
 * mrtsdi3.c
 *
 * Technology Software DRAM Init for DDR3 Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mru.h"
#include "mt.h"
#include "mrt3.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_TECH_DDR3_MRTSDI3_FILECODE
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

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */



/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates software DRAM init
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTDramInitSw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 ChipSel;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  // 3.Program F2x[1,0]7C[EnDramInit]=1
  NBPtr->SetBitField (NBPtr, BFEnDramInit, 1);

  // 4.wait 200us
  MemRecUWait10ns (20000, MemPtr);

  NBPtr->SetBitField (NBPtr, BFDeassertMemRstX, 1);

  // 6.wait 500us
  MemRecUWait10ns (50000, MemPtr);

  // 7.NOP or deselect & take CKE high
  NBPtr->SetBitField (NBPtr, BFAssertCke, 1);

  // 8.wait 360ns
  MemRecUWait10ns (36, MemPtr);

  // The following steps are performed with registered DIMMs only and
  // must be done for each chip select pair:
  //
  if (NBPtr->ChannelPtr->RegDimmPresent != 0) {
    MemRecTDramControlRegInit3 (TechPtr);
  }

  //
  // In recovery mode, we only need to initialize one chipsel for UDIMMs.
  //
  NBPtr->SetDramOdtRec (NBPtr, MISSION_MODE, 0, 0);

  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
    if ((NBPtr->DCTPtr->Timings.CsPresent & (UINT16) 1 << ChipSel) != 0) {
      NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);
      // 13.Send EMRS(2)
      MemRecTEMRS23 (TechPtr);
      NBPtr->SendMrsCmd (NBPtr);

      // 14.Send EMRS(3). Ordinarily at this time, MrsAddress[2:0]=000b
      MemRecTEMRS33 (TechPtr);
      NBPtr->SendMrsCmd (NBPtr);

      // 15.Send EMRS(1).
      MemRecTEMRS13 (TechPtr);
      NBPtr->SendMrsCmd (NBPtr);

      // 16.Send MRS with MrsAddress[8]=1(reset the DLL)
      MemRecTMRS3 (TechPtr);
      NBPtr->SendMrsCmd (NBPtr);

      //wait 500us
      MemRecUWait10ns (50000, MemPtr);

      if (NBPtr->ChannelPtr->RegDimmPresent == 0) {
        break;
      }
    }
  }

  // 17.Send two ZQCL commands (to even then odd chip select)
  NBPtr->sendZQCmd (NBPtr);
  NBPtr->sendZQCmd (NBPtr);

  // 18.Program F2x[1,0]7C[EnDramInit]=0
  NBPtr->SetBitField (NBPtr, BFEnDramInit, 0);

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS1 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTEMRS13 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT16 MrsAddress;
  UINT8 DramTerm;

  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=0,BA0=1
  NBPtr->SetBitField (NBPtr, BFMrsBank, 1);

  MrsAddress = 0;

  // program MrsAddress[5,1]=output driver impedance control (DIC):
  // based on F2x[1,0]84[DrvImpCtrl], which is 2'b01
  MrsAddress |= ((UINT16) 1 << 1);

  // program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
  // based on F2x[1,0]84[DramTerm], which is 3'b001 (60 Ohms)
  DramTerm = (UINT8) NBPtr->GetBitField (NBPtr, BFDramTerm);
  if ((DramTerm & 1) != 0) {
    MrsAddress |= ((UINT16) 1 << 2);
  }
  if ((DramTerm & 2) != 0) {
    MrsAddress |= ((UINT16) 1 << 6);
  }
  if ((DramTerm & 4) != 0) {
    MrsAddress |= ((UINT16) 1 << 9);
  }

  // program MrsAddress[12]=output disable (QOFF):
  // based on F2x[1,0]84[Qoff], which is 1'b0

  // program MrsAddress[11]=TDQS:
  // based on F2x[1,0]94[RDqsEn], which is 1'b0

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS2 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTEMRS23 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT16 MrsAddress;
  UINT8 DramTermDyn;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=1,BA0=0
  NBPtr->SetBitField (NBPtr, BFMrsBank, 2);

  // program MrsAddress[5:3]=CAS write latency (CWL):
  // based on F2x[1,0]84[Tcwl], which is 3'b000
  //
  MrsAddress = 0;

  // program MrsAddress[6]=auto self refresh method (ASR):
  // based on F2x[1,0]84[ASR], which is 1'b1
  // program MrsAddress[7]=self refresh temperature range (SRT):
  // based on F2x[1,0]84[SRT], which is also 1'b0
  //
  MrsAddress |= (UINT16) 1 << 6;

  // program MrsAddress[10:9]=dynamic termination during writes (RTT_WR):
  // based on F2x[1,0]84[DramTermDyn]
  //
  DramTermDyn = (UINT8) NBPtr->GetBitField (NBPtr, BFDramTermDyn);
  MrsAddress |= (UINT16) DramTermDyn << 9;

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS3 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTEMRS33 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=1,BA0=1
  NBPtr->SetBitField (NBPtr, BFMrsBank, 3);

  // program MrsAddress[1:0]=multi purpose register address location
  // (MPR Location):based on F2x[1,0]84[MprLoc], which is 0
  // program MrsAddress[2]=multi purpose register
  // (MPR):based on F2x[1,0]84[MprEn], which is also 0
  //
  NBPtr->SetBitField (NBPtr, BFMrsAddress, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This sets MSS value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTMRS3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT16 MrsAddress;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=0,BA0=0
  NBPtr->SetBitField (NBPtr, BFMrsBank, 0);

  // program MrsAddress[1:0]=burst length and control method
  // (BL):based on F2x[1,0]84[BurstCtrl], which is 1'b0
  //
  MrsAddress = 0;

  // program MrsAddress[3]=1 (BT):interleaved
  MrsAddress |= (UINT16) 1 << 3;

  // program MrsAddress[6:4,2]=read CAS latency
  // (CL):based on F2x[1,0]88[Tcl], which is 4'b0010
  MrsAddress |= (UINT16) 2 << 4;

  // program MrsAddress[11:9]=write recovery for auto-precharge
  // (WR):based on F2x[1,0]84[Twr], which is 3'b010
  //
  MrsAddress |= (UINT16) 2 << 9;

  // program MrsAddress[12]=0 (PPD):slow exit

  // program MrsAddress[8]=1 (DLL):DLL reset
  MrsAddress |= (UINT16) 1 << 8;   // just issue DLL reset at first time

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}
