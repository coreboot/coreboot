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

SMMSERVICESTRUC	smmItemsTable[]={
	{SB_PMIO_REG0E, BIT2, SB_PMIO_REG0F, BIT2, (CHAR8 *)"Software SMI through SMI CMD port \n ", softwareSMIservice},
	{SB_PMIO_REG00, BIT4, SB_PMIO_REG01, BIT4, (CHAR8 *)"Software initiated SMI \n ", NULL},
	{SB_PMIO_REG02, 0xFF, SB_PMIO_REG05, 0xFF, (CHAR8 *)"SMI on IRQ15-8 \n ", NULL},
	{SB_PMIO_REG03, 0xFF, SB_PMIO_REG06, 0xFF, (CHAR8 *)"SMI on IRQ7-0 \n ", NULL},
	{SB_PMIO_REG04, 0xFF, SB_PMIO_REG07, 0xFF, (CHAR8 *)"SMI on legacy devices activity(Serial, FDD etc) \n ", NULL},
	{SB_PMIO_REG1C, 0xFF, SB_PMIO_REG1D, 0xFF, (CHAR8 *)"SMI on PIO 0123 \n ", NULL},
	{SB_PMIO_REGA8, 0x0F, SB_PMIO_REGA9, 0xFF, (CHAR8 *)"SMI on PIO 4567 \n ", NULL},
};


/*++

Routine Description:

  SB SMI service

Arguments:

  pConfig - SBconfiguration

Returns:

  void

--*/

void sbSmmService(AMDSBCFG* pConfig){
	UINT8	i, dbEnableValue, dbStatusValue;
	SMMSERVICESTRUC	*pSmmItems;
	SMM_SERVICE_ROUTINE	serviceRoutine;

	pSmmItems = (SMMSERVICESTRUC *)FIXUP_PTR(&smmItemsTable[0]);
	TRACE((DMSG_SB_TRACE, "CIMx - Entering SMM services \n"));
	for (i = 1; i <= (sizeof(smmItemsTable)/sizeof(SMMSERVICESTRUC)); i++){
		dbEnableValue = pSmmItems->enableRegNum;
		ReadPMIO(pSmmItems->enableRegNum, AccWidthUint8, &dbEnableValue);
		ReadPMIO(pSmmItems->statusRegNum, AccWidthUint8, &dbStatusValue);
		if ( (dbEnableValue & (pSmmItems->enableBit)) && (dbStatusValue & (pSmmItems->statusBit)) ){
			TRACE((DMSG_SB_TRACE, "\n \nSmi source is: %s \n", pSmmItems->debugMessage));
			TRACE((DMSG_SB_TRACE, "Enable Reg:%d   Value:%d\n", pSmmItems->enableRegNum, dbEnableValue));
			TRACE((DMSG_SB_TRACE, "Status Reg:%d   Value:%d\n\n", pSmmItems->statusRegNum, dbStatusValue));
			if ( (pSmmItems->serviceRoutine)!= NULL){
				serviceRoutine = (void *)FIXUP_PTR(pSmmItems->serviceRoutine);
				serviceRoutine();
			}
		}
	}
	TRACE((DMSG_SB_TRACE, "CIMx - Exiting SMM services \n"));
}


void softwareSMIservice(void){
		UINT16	dwSmiCmdPort, dwVar;
		ReadPMIO(SB_PMIO_REG2A, AccWidthUint16, &dwSmiCmdPort);
		ReadIO(dwSmiCmdPort, AccWidthUint16, &dwVar);
		TRACE((DMSG_SB_TRACE, "SMI CMD Port Address: %X SMICMD Port value is %X \n", dwSmiCmdPort, dwVar));
}
