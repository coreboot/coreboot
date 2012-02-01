/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/



#include	"Platform.h"

void configureAzaliaPinCmd (AMDSBCFG* pConfig, UINT32 ddBAR0, UINT8 dbChannelNum);
void configureAzaliaSetConfigD4Dword(CODECENTRY* tempAzaliaCodecEntryPtr, UINT32 ddChannelNum, UINT32 ddBAR0);

//Pin Config for ALC880, ALC882 and ALC883:
CODECENTRY AzaliaCodecAlc882Table[] = {
	{0x14, 0x01014010},
	{0x15, 0x01011012},
	{0x16, 0x01016011},
	{0x17, 0x01012014},
	{0x18, 0x01A19030},
	{0x19, 0x411111F0},
	{0x1a, 0x01813080},
	{0x1b, 0x411111F0},
	{0x1C, 0x411111F0},
	{0x1d, 0x411111F0},
	{0x1e, 0x01441150},
	{0x1f, 0x01C46160},
	{0xff, 0xffffffff}
};


//Pin Config for ALC262
CODECENTRY AzaliaCodecAlc262Table[] = {
	  {0x14, 0x01014010},
	  {0x15, 0x411111F0},
	  {0x16, 0x411111F0},
//	  {0x17, 0x01012014},
	  {0x18, 0x01A19830},
	  {0x19, 0x02A19C40},
	  {0x1a, 0x01813031},
	  {0x1b, 0x02014C20},
	  {0x1c, 0x411111F0},
	  {0x1d, 0x411111F0},
	  {0x1e, 0x0144111E},
	  {0x1f, 0x01C46150},
	  {0xff, 0xffffffff}
};

//Pin Config for  ALC0861:
CODECENTRY AzaliaCodecAlc861Table[] = {
	  {0x01, 0x8086C601},
	  {0x0B, 0x01014110},
	  {0x0C, 0x01813140},
	  {0x0D, 0x01A19941},
	  {0x0E, 0x411111F0},
	  {0x0F, 0x02214420},
	  {0x10, 0x02A1994E},
	  {0x11, 0x99330142},
	  {0x12, 0x01451130},
	  {0x1F, 0x411111F0},
	  {0x20, 0x411111F0},
	  {0x23, 0x411111F0},
	  {0xff, 0xffffffff}
};

//Pin Config for  ADI1984:
CODECENTRY AzaliaCodecAd1984Table[] = {
	  {0x11, 0x0221401F},
	  {0x12, 0x90170110},
	  {0x13, 0x511301F0},
	  {0x14, 0x02A15020},
	  {0x15, 0x50A301F0},
	  {0x16, 0x593301F0},
	  {0x17, 0x55A601F0},
	  {0x18, 0x55A601F0},
	  {0x1A, 0x91F311F0},
	  {0x1B, 0x014511A0},
	  {0x1C, 0x599301F0},
	  {0xff, 0xffffffff}
};


CODECENTRY FrontPanelAzaliaCodecTableList[] = {
	  {0x19, 0x02A19040},
	  {0x1b, 0x02214020},
	  {0xff, 0xffffffff}
};


CODECTBLLIST azaliaCodecTableList[] = {
	 {0x010ec0880, &AzaliaCodecAlc882Table[0]},
	 {0x010ec0882, &AzaliaCodecAlc882Table[0]},
	 {0x010ec0883, &AzaliaCodecAlc882Table[0]},
	 {0x010ec0885, &AzaliaCodecAlc882Table[0]},
	 {0x010ec0262, &AzaliaCodecAlc262Table[0]},
	 {0x010ec0861, &AzaliaCodecAlc861Table[0]},
	 {0x011d41984, &AzaliaCodecAd1984Table[0]},
	 {(UINT32)0x0FFFFFFFF, (CODECENTRY*)0xFFFFFFFF}
};


/*-------------------------------------------------------------------------------
; Procedure:	azaliaInitAfterPciEnum
;
; Description:	This routine detects Azalia and, if present, initializes Azalia
;		This routine is called from atiSbAfterPciInit
;
;
; Exit:		None
;
; Modified:	None
;
;-----------------------------------------------------------------------------
*/
void azaliaInitAfterPciEnum (AMDSBCFG* pConfig){
	UINT8	i, dbEnableAzalia=0, dbPinRouting, dbChannelNum=0, dbTempVariable = 0;
	UINT16	dwTempVariable = 0;
	UINT32	ddBAR0, ddTempVariable = 0;

	if	(pConfig->AzaliaController == 1) return;

	if	(pConfig->AzaliaController != 1){
		RWPCI((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG04, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT1, BIT1);
		ReadPCI((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG10, AccWidthUint32, &ddBAR0);

		if (ddBAR0 != 0){		//Keep the flag as disabled if BAR is 0 or all "F"s.
			if (ddBAR0 != 0xFFFFFFFF){
				ddBAR0 &=  ~(0x03FFF);
				dbEnableAzalia = 1;
				TRACE((DMSG_SB_TRACE, "CIMxSB - Enabling Azalia controller (BAR setup is ok) \n"));
			}
		}
	}

	if (dbEnableAzalia){		//if Azalia is enabled
		//Get SDIN Configuration
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGF8, AccWidthUint32 | S3_SAVE, 0, ddTempVariable);
		ddTempVariable |= (pConfig->AzaliaSdin3 << 6);
		ddTempVariable |= (pConfig->AzaliaSdin2 << 4);
		ddTempVariable |= (pConfig->AzaliaSdin1 << 2);
		ddTempVariable |= pConfig->AzaliaSdin0;
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGFC, AccWidthUint8 | S3_SAVE, 0, (ddTempVariable & 0xFF));
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG60+3, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT2+BIT1+BIT0), 0);

		i=11;
		do{
			ReadMEM( ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
			dbTempVariable |= BIT0;
			WriteMEM(ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
			Stall(1000);
			ReadMEM(ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
			i--;
		}	while ( (!(dbTempVariable & BIT0)) && (i > 0) );

		if (i==0){
			TRACE((DMSG_SB_TRACE, "CIMxSB - Problem in resetting Azalia controller\n"));
			return;
		}

		Stall(1000);
		ReadMEM( ddBAR0+SB_AZ_BAR_REG0E, AccWidthUint16, &dwTempVariable);
		if (dwTempVariable & 0x0F){
			TRACE((DMSG_SB_TRACE, "CIMxSB - Atleast One Azalia CODEC found \n"));
			//atleast one azalia codec found
			ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGFC, AccWidthUint8, &dbPinRouting);
			do{
				if	( ( !(dbPinRouting & BIT0) ) && (dbPinRouting & BIT1) )
					configureAzaliaPinCmd(pConfig, ddBAR0, dbChannelNum);
				dbPinRouting >>= 2;
				dbChannelNum++;
			}	while (dbChannelNum != 4);
		}
		else{
			TRACE((DMSG_SB_TRACE, "CIMxSB - Azalia CODEC NOT found \n"));
			//No Azalia codec found
			if	(pConfig->AzaliaController != 2)
				dbEnableAzalia = 0;		//set flag to disable Azalia
		}
	}

	if (dbEnableAzalia){
		//redo clear reset
		do{
			dwTempVariable = 0;
			WriteMEM( ddBAR0+SB_AZ_BAR_REG0C, AccWidthUint16 | S3_SAVE, &dwTempVariable);
			ReadMEM(ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
			dbTempVariable &= ~(UINT8)(BIT0);
			WriteMEM(ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
			ReadMEM(ddBAR0+SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
		}	while (dbTempVariable & BIT0);

		if	(pConfig->AzaliaSnoop == 1)
			RWPCI((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG42, AccWidthUint8 | S3_SAVE, 0xFF, BIT1+BIT0);
	}
	else{
		//disable Azalia controller
		RWPCI((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG04, AccWidthUint16 | S3_SAVE, 0, 0);
		RWPMIO(SB_PMIO_REG59, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT3, 0);
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGFC, AccWidthUint8 | S3_SAVE, 0, 0x55);
	}
}


void configureAzaliaPinCmd (AMDSBCFG* pConfig, UINT32 ddBAR0, UINT8 dbChannelNum){
	UINT32	ddTempVariable, ddChannelNum;
	CODECTBLLIST*	ptempAzaliaOemCodecTablePtr;
	CODECENTRY*	tempAzaliaCodecEntryPtr;

	if ((pConfig->AzaliaPinCfg) != 1)
		return;

	ddChannelNum = dbChannelNum << 28;
	ddTempVariable = 0xF0000;
	ddTempVariable |= ddChannelNum;
	WriteMEM(ddBAR0 + SB_AZ_BAR_REG60, AccWidthUint32 | S3_SAVE, &ddTempVariable);
	Stall(60);
	ReadMEM(ddBAR0 + SB_AZ_BAR_REG64, AccWidthUint32 | S3_SAVE, &ddTempVariable);

	if	( ((pConfig->pAzaliaOemCodecTablePtr) == NULL) || ((pConfig->pAzaliaOemCodecTablePtr) == ((CODECTBLLIST*) 0xFFFFFFFF)) )
		ptempAzaliaOemCodecTablePtr = (CODECTBLLIST*) FIXUP_PTR(&azaliaCodecTableList[0]);
	else
		ptempAzaliaOemCodecTablePtr = (CODECTBLLIST*) pConfig->pAzaliaOemCodecTablePtr;

	TRACE((DMSG_SB_TRACE, "CIMxSB - Azalia CODEC table pointer is %x \n", (UINT32)ptempAzaliaOemCodecTablePtr));

	while ( ptempAzaliaOemCodecTablePtr->CodecID != 0xFFFFFFFF){
		if ( ptempAzaliaOemCodecTablePtr->CodecID == ddTempVariable)
			break;
		else
			++ptempAzaliaOemCodecTablePtr;
	}

	if ( ptempAzaliaOemCodecTablePtr->CodecID != 0xFFFFFFFF){
		TRACE((DMSG_SB_TRACE, "CIMxSB - Matching CODEC ID found \n"));
		tempAzaliaCodecEntryPtr = (CODECENTRY*) ptempAzaliaOemCodecTablePtr->CodecTablePtr;
		TRACE((DMSG_SB_TRACE, "CIMxSB - Matching Azalia CODEC table pointer is %x \n", (UINT32)tempAzaliaCodecEntryPtr));

		if	( ((pConfig->pAzaliaOemCodecTablePtr) == NULL) || ((pConfig->pAzaliaOemCodecTablePtr) == ((CODECTBLLIST*) 0xFFFFFFFF)) )
			tempAzaliaCodecEntryPtr = (CODECENTRY*) FIXUP_PTR(tempAzaliaCodecEntryPtr);

		configureAzaliaSetConfigD4Dword(tempAzaliaCodecEntryPtr, ddChannelNum, ddBAR0);
		if	 (pConfig->AzaliaFrontPanel != 1){
			if ( (pConfig->AzaliaFrontPanel == 2) || (pConfig->FrontPanelDetected == 1) ){
				if ( ((pConfig->pAzaliaOemFpCodecTableptr) == NULL) || ((pConfig->pAzaliaOemFpCodecTableptr) == 0xFFFFFFFF))
					tempAzaliaCodecEntryPtr = (CODECENTRY*) FIXUP_PTR(&FrontPanelAzaliaCodecTableList[0]);
				else
					tempAzaliaCodecEntryPtr = (CODECENTRY*) pConfig->pAzaliaOemFpCodecTableptr;
				configureAzaliaSetConfigD4Dword(tempAzaliaCodecEntryPtr, ddChannelNum, ddBAR0);
			}
		}
	}
}


void configureAzaliaSetConfigD4Dword(CODECENTRY* tempAzaliaCodecEntryPtr, UINT32 ddChannelNum, UINT32 ddBAR0){
	UINT8 dbtemp1,dbtemp2, i;
	UINT32 ddtemp=0,ddtemp2=0;

	while ((tempAzaliaCodecEntryPtr->Nid) != 0xFF){
		dbtemp1=0x20;
		if ((tempAzaliaCodecEntryPtr->Nid) == 0x1)
			dbtemp1=0x24;
		ddtemp =  tempAzaliaCodecEntryPtr->Nid;
		ddtemp &= 0xff;
		ddtemp <<= 20;
		ddtemp |= ddChannelNum;
		ddtemp |= (0x700 << 8);
		for(i=4; i>0; i--){
			do{
				ReadMEM(ddBAR0 + SB_AZ_BAR_REG68, AccWidthUint32, &ddtemp2);
			} while (ddtemp2 & BIT0);
			dbtemp2 = ( (tempAzaliaCodecEntryPtr->Byte40) >> ((4-i) * 8 ) ) & 0xff;
			ddtemp =  (ddtemp & 0xFFFF0000)+ ((dbtemp1 - i) << 8) + dbtemp2;
			WriteMEM(ddBAR0 + SB_AZ_BAR_REG60, AccWidthUint32 | S3_SAVE, &ddtemp);
			Stall(60);
		}
		++tempAzaliaCodecEntryPtr;
	}
}

