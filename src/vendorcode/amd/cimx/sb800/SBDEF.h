/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */


//AMD Library Routines (AMDLIB.C)
UINT8 getNumberOfCpuCores (OUT VOID);
UINT32  readAlink (IN UINT32 Index);
VOID  writeAlink (IN UINT32 Index, IN UINT32 Data);
VOID  rwAlink (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask);

//AMD Library Routines (LEGACY.C)
UINT32   GetFixUp (OUT VOID);

//AMD Library Routines (IOLIB.C)
VOID  ReadIO (IN UINT16  Address, IN UINT8 OpFlag, IN VOID *Value);
VOID  WriteIO (IN UINT16 Address, IN UINT8 OpFlag, IN VOID *Value);
VOID  RWIO (IN UINT16 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32 Data);



//AMD Library Routines (MEMLIB.C)
VOID  ReadMEM (IN UINT32 Address, IN UINT8 OpFlag, IN VOID* Value);
VOID  WriteMEM (IN UINT32 Address, IN UINT8 OpFlag, IN VOID* Value);
VOID  RWMEM (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32 Data);

//AMD Library Routines (PCILIB.C)
VOID  ReadPCI (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value);
VOID  WritePCI (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value);
VOID  RWPCI (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32  Data);

//AMD Library Routines (SBPELIB.C)
/**
 * Read Southbridge Revision ID cie Base
 *
 *
 * @retval  0xXXXXXXXX   Revision ID
 *
 */
UINT8 getRevisionID (OUT VOID);

/**
 * programPciByteTable - Program PCI register by table (8 bits data)
 *
 *
 *
 * @param[in] pPciByteTable    - Table data pointer
 * @param[in] dwTableSize      - Table length
 *
 */
VOID  programPciByteTable (IN REG8MASK* pPciByteTable, IN UINT16 dwTableSize);

/**
 * programSbAcpiMmioTbl - Program SB ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 *
 */
VOID  programSbAcpiMmioTbl (IN AcpiRegWrite  *pAcpiTbl);

/**
 * getChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - SB800 chip Strap Status
 *    @li <b>0001</b> - Not USED FWH
 *    @li <b>0002</b> - Not USED LPC ROM
 *    @li <b>0004</b> - EC enabled
 *    @li <b>0008</b> - Reserved
 *    @li <b>0010</b> - Internal Clock mode
 *
 */
VOID getChipSysMode (IN VOID* Value);

/**
 * Read Southbridge CIMx configuration structure pointer
 *
 *
 *
 * @retval  0xXXXXXXXX   CIMx configuration structure pointer.
 *
 */
AMDSBCFG* getConfigPointer (OUT VOID);

//AMD Library Routines (PMIOLIB.C)
/**
 * Read PMIO
 *
 *
 *
 * @param[in] Address  - PMIO Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 *
 */
VOID  ReadPMIO (IN UINT8 Address, IN UINT8 OpFlag, IN VOID* Value);

/**
 * Write PMIO
 *
 *
 *
 * @param[in] Address  - PMIO Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 *
 */
VOID  WritePMIO (IN UINT8 Address, IN UINT8 OpFlag, IN VOID* Value);

/**
 * RWPMIO - Read/Write PMIO
 *
 *
 *
 * @param[in] Address    - PMIO Offset value
 * @param[in] OpFlag     - Access sizes
 * @param[in] AndMask    - Data And Mask 32 bits
 * @param[in] OrMask     - Data OR Mask 32 bits
 *
 */
VOID  RWPMIO (IN UINT8 Address, IN UINT8 OpFlag, IN UINT32 AndMask, IN UINT32 OrMask);

//AMD Library Routines (PMIO2LIB.C)

/**
 * Read PMIO2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 *
 */
VOID  ReadPMIO2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID* Value);

/**
 * Write PMIO 2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 *
 */
VOID  WritePMIO2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID* Value);

/**
 * RWPMIO2 - Read/Write PMIO2
 *
 *
 *
 * @param[in] Address    - PMIO2 Offset value
 * @param[in] OpFlag     - Access sizes
 * @param[in] AndMask    - Data And Mask 32 bits
 * @param[in] OrMask     - Data OR Mask 32 bits
 *
 */
VOID  RWPMIO2 (IN UINT8 Address, IN UINT8  OpFlag, IN UINT32 AndMask, IN UINT32 OrMask);
//AMD Library Routines (ECLIB.C)
// ECLIB Routines

// #ifndef NO_EC_SUPPORT

/**
 * EnterEcConfig - Force EC into Config mode
 *
 *
 *
 *
 */
VOID  EnterEcConfig (VOID);

/**
 * ExitEcConfig - Force EC exit Config mode
 *
 *
 *
 *
 */
VOID  ExitEcConfig (VOID);

/**
 * ReadEC8 - Read EC register data
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Read Data Buffer
 *
 */
VOID  ReadEC8 (IN UINT8 Address, IN UINT8* Value);

/**
 * WriteEC8 - Write date into EC register
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Write Data Buffer
 *
 */
VOID  WriteEC8 (IN UINT8 Address, IN UINT8* Value);

/**
 * RWEC8 - Read/Write EC register
 *
 *
 *
 * @param[in] Address    - EC Register Offset Value
 * @param[in] AndMask    - Data And Mask 8 bits
 * @param[in] OrMask     - Data OR Mask 8 bits
 *
 */
VOID  RWEC8 (IN UINT8 Address, IN UINT8 AndMask, IN UINT8 OrMask);

/**
 * IsZoneFuncEnable - check every zone support function with BitMap from user define
 *
 */
BOOLEAN IsZoneFuncEnable ( UINT16 Flag, UINT8  func, UINT8  Zone);

VOID sbECfancontrolservice (IN AMDSBCFG* pConfig);
VOID SBIMCFanInitializeS3 (VOID);
VOID GetSbAcpiMmioBase (OUT UINT32* AcpiMmioBase);
VOID GetSbAcpiPmBase (OUT  UINT16* AcpiPmBase);

// #endif

