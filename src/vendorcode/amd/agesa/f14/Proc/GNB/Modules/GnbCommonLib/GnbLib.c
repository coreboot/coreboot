/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB register access services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-13 19:07:10 -0700 (Tue, 13 Jul 2010) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Porting.h"
#include  "AMD.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbLibIoAcc.h"
#include  "GnbLibPciAcc.h"
#include  "GnbLibMemAcc.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GnbLibPciIndirectReadField (
  IN       UINT32       Address,
  IN       UINT32       IndirectAddress,
  IN       UINT8        FieldOffset,
  IN       UINT8        FieldWidth,
     OUT   UINT32       *Value,
  IN       VOID         *Config
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Read GNB indirect registers
 *
 *
 *
 * @param[in] Address         PCI address of indirect register
 * @param[in] IndirectAddress Offset of indirect register
 * @param[in] Width           Width
 * @param[out] Value           Pointer to value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibPciIndirectRead (
  IN       UINT32       Address,
  IN       UINT32       IndirectAddress,
  IN       ACCESS_WIDTH Width,
     OUT   VOID         *Value,
  IN       VOID         *Config
  )
{
  UINT32  IndexOffset;
  IndexOffset = LibAmdAccessWidth (Width);
  GnbLibPciWrite (Address, Width, &IndirectAddress, Config);
  GnbLibPciRead (Address + IndexOffset, Width, Value, Config);
}
/*----------------------------------------------------------------------------------------*/
/**
 * Read GNB indirect registers field
 *
 *
 *
 * @param[in] Address         PCI address of indirect register
 * @param[in] IndirectAddress Offset of indirect register
 * @param[in] FieldOffset     Field offset
 * @param[in] FieldWidth      Field width
 * @param[out] Value          Pointer to value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibPciIndirectReadField (
  IN       UINT32       Address,
  IN       UINT32       IndirectAddress,
  IN       UINT8        FieldOffset,
  IN       UINT8        FieldWidth,
     OUT   UINT32       *Value,
  IN       VOID         *Config
  )
{
  UINT32  Mask;
  GnbLibPciIndirectRead (Address, IndirectAddress, AccessWidth32, Value, Config);
  Mask = (1 << FieldWidth) - 1;
  *Value = (*Value >> FieldOffset) & Mask;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write GNB indirect registers
 *
 *
 *
 * @param[in] Address         PCI address of indirect register
 * @param[in] IndirectAddress Offset of indirect register
 * @param[in] Width           Width
 * @param[in] Value           Pointer to value
 * @param[in] Config          Pointer to standard header
 */

VOID
GnbLibPciIndirectWrite (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *Config
  )
{
  UINT32  IndexOffset;
  IndexOffset = LibAmdAccessWidth (Width);
  GnbLibPciWrite (Address, Width, &IndirectAddress, Config);
  GnbLibPciWrite (Address + IndexOffset, Width, Value, Config);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write GNB indirect registers field
 *
 *
 *
 * @param[in] Address         PCI address of indirect register
 * @param[in] IndirectAddress Offset of indirect register
 * @param[in] FieldOffset     Field offset
 * @param[in] FieldWidth      Field width
 * @param[in] Value           Pointer to value
 * @param[in] S3Save          Save for S3 (TRUE/FALSE)
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibPciIndirectWriteField (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      UINT8         FieldOffset,
  IN      UINT8         FieldWidth,
  IN      UINT32        Value,
  IN      BOOLEAN       S3Save,
  IN      VOID          *Config
  )
{
  UINT32  Data;
  UINT32  Mask;
  GnbLibPciIndirectRead (Address, IndirectAddress, AccessWidth32, &Data, Config);
  Mask = (1 << FieldWidth) - 1;
  Data &= (~(Mask << FieldOffset));
  Data |= ((Value & Mask) << FieldOffset);
  GnbLibPciIndirectWrite (Address, IndirectAddress, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Data, Config);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write GNB indirect registers field
 *
 *
 *
 * @param[in] Address         PCI address of indirect register
 * @param[in] IndirectAddress Offset of indirect register
 * @param[in] Width           Width
 * @param[in] Mask            And Mask
 * @param[in] Value           Or Value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibPciIndirectRMW (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  )
{
  UINT32  Data;
  GnbLibPciIndirectRead (
    Address,
    IndirectAddress,
    (Width >= AccessS3SaveWidth8) ? (Width - (AccessS3SaveWidth8 - AccessWidth8)) : Width,
    &Data,
    Config
    );
  Data = (Data & Mask) | Value;
  GnbLibPciIndirectWrite (Address, IndirectAddress, Width, &Data, Config);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCI registers
 *
 *
 *
 * @param[in] Address         PCI address
 * @param[in] Width           Access width
 * @param[in] Mask            AND Mask
 * @param[in] Value           OR Value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibPciRMW (
  IN      UINT32        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  )
{
  UINT32  Data;
  GnbLibPciRead (Address, Width, &Data, Config);
  Data = (Data & Mask) | Value;
  GnbLibPciWrite (Address, Width, &Data, Config);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write I/O registers
 *
 *
 *
 * @param[in] Address         I/O Port
 * @param[in] Width           Access width
 * @param[in] Mask            AND Mask
 * @param[in] Value           OR Mask
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibIoRMW (
  IN      UINT16        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  )
{
  UINT32  Data;
  GnbLibIoRead (Address, Width, &Data, Config);
  Data = (Data & Mask) | Value;
  GnbLibIoWrite (Address, Width, &Data, Config);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write MMIO registers
 *
 *
 *
 * @param[in] Address         Physical address
 * @param[in] Width           Access width
 * @param[in] Mask            AND Mask
 * @param[in] Value           OR Value
 * @param[in] Config          Pointer to standard header
 */
VOID
GnbLibMemRMW (
  IN      UINT64        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  )
{
  UINT32  Data;
  GnbLibMemRead (Address, Width, &Data, Config);
  Data = (Data & Mask) | Value;
  GnbLibMemWrite (Address, Width, &Data, Config);
}
/*----------------------------------------------------------------------------------------*/
/**
 * Get number of sockets
 *
 *
 *
 * @param[in]   StdHeader     Standard configuration header
 * @retval                    Total number of socket on platform
 */

UINT32
GnbGetNumberOfSockets (
  IN      AMD_CONFIG_PARAMS *StdHeader
  )
{
  return 1;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of Silicons on the socket
 *
 *
 *
 * @param[in]   SiliconId     Socket ID
 * @param[in]   StdHeader     Standard configuration header
 * @retval                    Number of silicons/modules in device in socket
 */

UINT32
GnbGetNumberOfSiliconsOnSocket (
  IN      UINT32              SiliconId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return 1;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get PCI Address
 *
 *
 *
 * @param[in]   SocketId      Socket ID
 * @param[in]   SiliconId     Silicon device Id
 * @param[in]   StdHeader     Standard configuration header
 * @retval                    PCI address of GNB for a given socket/silicon.
 */

PCI_ADDR
GnbGetPciAddress (
  IN      UINT32              SocketId,
  IN      UINT32              SiliconId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PCI_ADDR  Gnb;
  Gnb.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  return Gnb;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if anything plugged in socket
 *
 *
 *
 * @param[in]   SocketId      Socket ID
 * @param[in]   StdHeader     Standard configuration header
 * @retval      TRUE          CPU present in socket.
 */

BOOLEAN
GnbIsDevicePresentInSocket (
  IN      UINT32              SocketId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return TRUE;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Claculate power of number
 *
 *
 *
 * @param[in]   Value         Number
 * @param[in]   Power         Power
 */

UINT32
GnbLibPowerOf (
  IN      UINT32  Value,
  IN      UINT32  Power
  )
{
  UINT32  Result;
  if (Power == 0) {
    return 1;
  }
  Result = Value;
  while ((--Power) > 0) {
    Result *= Value;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Search buffer for pattern
 *
 *
 * @param[in]  Buf1           Pointer to source buffer which will be subject of search
 * @param[in]  Buf1Length     Length of the source buffer
 * @param[in]  Buf2           Pointer to pattern buffer
 * @param[in]  Buf2Length     Length of the pattern buffer
 * @retval     Pointer on first accurance of Buf2 in Buf1 or NULL
 */

VOID*
GnbLibFind (
  IN      UINT8               *Buf1,
  IN      UINTN               Buf1Length,
  IN      UINT8               *Buf2,
  IN      UINTN               Buf2Length
  )
{
  UINT8 *CurrentBuf1Ptr;
  CurrentBuf1Ptr = Buf1;
  while (CurrentBuf1Ptr < (Buf1 + Buf1Length - Buf2Length)) {
    UINT8 *SourceBufPtr;
    UINT8 *PatternBufPtr;
    UINTN PatternBufLength;
    SourceBufPtr = CurrentBuf1Ptr;
    PatternBufPtr = Buf2;
    PatternBufLength = Buf2Length;
    while ((*SourceBufPtr++ == *PatternBufPtr++) && (PatternBufLength-- != 0));
    if (PatternBufLength == 0) {
      return CurrentBuf1Ptr;
    }
    CurrentBuf1Ptr++;
  }
  return NULL;
}


