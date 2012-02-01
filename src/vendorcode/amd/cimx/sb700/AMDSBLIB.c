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


#include "Platform.h"


void
ReadPMIO        (
UINT8   Address,
UINT8   OpFlag,
void* Value
)
{
        UINT8   i;

        OpFlag = OpFlag & 0x7f;
        if (OpFlag == 0x02) OpFlag = 0x03;
        for (i=0;i<=OpFlag;i++){
                WriteIO(0xCD6, AccWidthUint8, &Address);                // SB_IOMAP_REGCD6
                Address++;
                ReadIO(0xCD7, AccWidthUint8, (UINT8 *)Value+i);                 // SB_IOMAP_REGCD7
        }
}


void
WritePMIO       (
UINT8   Address,
UINT8   OpFlag,
void* Value
)
{
        UINT8   i;

        OpFlag = OpFlag & 0x7f;
        if (OpFlag == 0x02) OpFlag = 0x03;
        for (i=0;i<=OpFlag;i++){
                WriteIO(0xCD6, AccWidthUint8, &Address);                // SB_IOMAP_REGCD6
                Address++;
                WriteIO(0xCD7, AccWidthUint8, (UINT8 *)Value+i);                        // SB_IOMAP_REGCD7
        }
}


void
RWPMIO  (
UINT8 Address,
UINT8   OpFlag,
UINT32 AndMask,
UINT32 OrMask
)
{
        UINT32 Result;

        OpFlag = OpFlag & 0x7f;
        ReadPMIO(Address,OpFlag,&Result);
        Result = (Result & AndMask)| OrMask;
        WritePMIO(Address,OpFlag,&Result);
}


void
ReadPMIO2       (
UINT8   Address,
UINT8   OpFlag,
void* Value
)
{
        UINT8   i;

        OpFlag = OpFlag & 0x7f;
        if (OpFlag == 0x02) OpFlag = 0x03;
        for (i=0;i<=OpFlag;i++){
                WriteIO(0xCD0, AccWidthUint8, &Address);                // SB_IOMAP_REGCD0
                Address++;
                ReadIO(0xCD1, AccWidthUint8, (UINT8 *)Value+i);                 // SB_IOMAP_REGCD1
        }
}


void
WritePMIO2      (
UINT8   Address,
UINT8   OpFlag,
void* Value
)
{
        UINT8   i;

        OpFlag = OpFlag & 0x7f;
        if (OpFlag == 0x02) OpFlag = 0x03;
        for (i=0;i<=OpFlag;i++){
                WriteIO(0xCD0, AccWidthUint8, &Address);                // SB_IOMAP_REGCD0
                Address++;
                WriteIO(0xCD1, AccWidthUint8, (UINT8 *)Value+i);                        // SB_IOMAP_REGCD1
        }
}


void
RWPMIO2 (
UINT8 Address,
UINT8   OpFlag,
UINT32 AndMask,
UINT32 OrMask
)
{
        UINT32 Result;

        OpFlag = OpFlag & 0x7f;
        ReadPMIO2(Address,OpFlag,&Result);
        Result = (Result & AndMask)| OrMask;
        WritePMIO2(Address,OpFlag,&Result);
}


void
EnterEcConfig()
{
        UINT16  dwEcIndexPort;

        ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
        dwEcIndexPort &= ~(UINT16)(BIT0);
        RWIO(dwEcIndexPort, AccWidthUint8, 0x00, 0x5A);
}

void
ExitEcConfig()
{
        UINT16  dwEcIndexPort;

        ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
        dwEcIndexPort &= ~(UINT16)(BIT0);
        RWIO(dwEcIndexPort, AccWidthUint8, 0x00, 0xA5);
}


void
ReadEC8 (
UINT8 Address,
UINT8* Value
)
{
        UINT16  dwEcIndexPort;

        ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
        dwEcIndexPort &= ~(UINT16)(BIT0);
        WriteIO(dwEcIndexPort, AccWidthUint8, &Address);                // SB_IOMAP_REGCD6
        ReadIO(dwEcIndexPort+1, AccWidthUint8, Value);                  // SB_IOMAP_REGCD7
}


void
WriteEC8        (
UINT8 Address,
UINT8* Value
)
{
        UINT16  dwEcIndexPort;

        ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
        dwEcIndexPort &= ~(UINT16)(BIT0);
        WriteIO(dwEcIndexPort, AccWidthUint8, &Address);                // SB_IOMAP_REGCD6
        WriteIO(dwEcIndexPort+1, AccWidthUint8, Value);                 // SB_IOMAP_REGCD7
}


void
RWEC8   (
UINT8 Address,
UINT8 AndMask,
UINT8 OrMask
)
{
        UINT8 Result;
        ReadEC8(Address,&Result);
        Result = (Result & AndMask)| OrMask;
        WriteEC8(Address, &Result);
}


void
programPciByteTable     (
REG8MASK* pPciByteTable,
UINT16 dwTableSize
)
{
        UINT8   i, dbBusNo, dbDevFnNo;
        UINT32  ddBDFR;

        dbBusNo = pPciByteTable->bRegIndex;
        dbDevFnNo = pPciByteTable->bANDMask;
        pPciByteTable++;
        for (i = 1; i < dwTableSize; i++){
                if ( (pPciByteTable->bRegIndex==0xFF) && (pPciByteTable->bANDMask==0xFF) && (pPciByteTable->bORMask==0xFF) ){
                        pPciByteTable++;
                        dbBusNo = pPciByteTable->bRegIndex;
                        dbDevFnNo = pPciByteTable->bANDMask;
                }
                else{
                        ddBDFR = (dbBusNo << 24) + (dbDevFnNo << 16) + (pPciByteTable->bRegIndex) ;
                        TRACE((DMSG_SB_TRACE, "PFA=%X  AND=%X, OR=%X\n", ddBDFR, pPciByteTable->bANDMask, pPciByteTable->bORMask));
                        RWPCI(ddBDFR, AccWidthUint8 | S3_SAVE, pPciByteTable->bANDMask, pPciByteTable->bORMask);
                        pPciByteTable++;
                }
        }
}


void
programPmioByteTable    (
REG8MASK* pPmioByteTable,
UINT16 dwTableSize
)
{
        UINT8   i;
        for (i = 0; i < dwTableSize; i++){
                TRACE((DMSG_SB_TRACE, "PMIO Reg = %X   AndMask = %X  OrMask = %X\n",pPmioByteTable->bRegIndex,pPmioByteTable->bANDMask, pPmioByteTable->bORMask));
                RWPMIO(pPmioByteTable->bRegIndex, AccWidthUint8 , pPmioByteTable->bANDMask, pPmioByteTable->bORMask);
                pPmioByteTable++;
        }
}


UINT8
getClockMode    (
void
)
{
        UINT8 dbTemp=0;

        RWPMIO(SB_PMIO_REGB2, AccWidthUint8, 0xFF, BIT7);
        ReadPMIO(SB_PMIO_REGB0, AccWidthUint8, &dbTemp);
        return(dbTemp&BIT4);
}


UINT16
readStrapStatus (
void
)
{
        UINT16 dwTemp=0;

        RWPMIO(SB_PMIO_REGB2, AccWidthUint8, 0xFF, BIT7);
        ReadPMIO(SB_PMIO_REGB0, AccWidthUint16, &dwTemp);
        return(dwTemp);
}
