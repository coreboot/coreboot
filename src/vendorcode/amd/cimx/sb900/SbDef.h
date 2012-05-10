/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/


//AMD Library Routines (AMDLIB.C)
void  InitSerialOut (void);
unsigned char getNumberOfCpuCores (OUT void);
unsigned int  readAlink (IN unsigned int Index);
void  writeAlink (IN unsigned int Index, IN unsigned int Data);
void  rwAlink (IN unsigned int Index, IN unsigned int AndMask, IN unsigned int OrMask);

//AMD Library Routines (LEGACY.C)
unsigned int   GetFixUp (OUT void);

//AMD Library Routines (IOLIB.C)
void  ReadIO (IN unsigned short  Address, IN unsigned char OpFlag, IN void *Value);
void  WriteIO (IN unsigned short Address, IN unsigned char OpFlag, IN void *Value);
void  RWIO (IN unsigned short Address, IN unsigned char OpFlag, IN unsigned int Mask, IN unsigned int Data);

/// CPUID data received registers format
typedef struct _SB_CPUID_DATA {
  IN OUT   unsigned int          EAX_Reg;                ///< CPUID instruction result in EAX
  IN OUT   unsigned int          EBX_Reg;                ///< CPUID instruction result in EBX
  IN OUT   unsigned int          ECX_Reg;                ///< CPUID instruction result in ECX
  IN OUT   unsigned int          EDX_Reg;                ///< CPUID instruction result in EDX
} SB_CPUID_DATA;

//AMD Library Routines (AMDLIB32.ASM)
unsigned char ReadIo8 (IN unsigned short Address);
unsigned short ReadIo16 (IN unsigned short Address);
unsigned int ReadIo32 (IN unsigned short Address);
void WriteIo8 (IN unsigned short Address, IN unsigned char Data);
void WriteIo16 (IN unsigned short Address, IN unsigned short Data);
void WriteIo32 (IN unsigned short Address, IN unsigned int Data);
unsigned char ReadNumberOfCpuCores (void);
unsigned long long  ReadTSC (void);
void CpuidRead (IN unsigned int CpuidFcnAddress, OUT SB_CPUID_DATA  *Value);


//AMD Library Routines (MEMLIB.C)
void  ReadMEM (IN unsigned int Address, IN unsigned char OpFlag, IN void* Value);
void  WriteMEM (IN unsigned int Address, IN unsigned char OpFlag, IN void* Value);
void  RWMEM (IN unsigned int Address, IN unsigned char OpFlag, IN unsigned int Mask, IN unsigned int Data);
void
MemoryCopy (
  IN       unsigned char *Dest,
  IN       unsigned char *Source,
  IN       unsigned int Size
  );

//AMD Library Routines (PCILIB.C)
void  ReadPCI (IN unsigned int Address, IN unsigned char OpFlag, IN void *Value);
void  WritePCI (IN unsigned int Address, IN unsigned char OpFlag, IN void *Value);
void  RWPCI (IN unsigned int Address, IN unsigned char OpFlag, IN unsigned int Mask, IN unsigned int  Data);

//AMD Library Routines (SBPELIB.C)
/**
 * Read Southbridge Revision ID cie Base
 *
 *
 * @retval  0xXXXXXXXX   Revision ID
 *
 */
unsigned char getRevisionID (OUT void);

//AMD Library Routines (SBPELIB.C)
/**
 * Is SB A11?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsSbA11 (OUT void);

//AMD Library Routines (SBPELIB.C)
/**
 * Is SB A12?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsSbA12 (OUT void);

//AMD Library Routines (SBPELIB.C)
/**
 * Is SB A12 Plus?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsSbA12Plus (OUT void);

//AMD Library Routines (SBPELIB.C)
/**
 * Is SB A13 Plus?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsSbA13Plus (OUT void);

/**
 * Is LPC Rom?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsExternalClockMode (OUT void);

/**
 * Is External Clock Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsLpcRom (OUT void);

/**
 * Is GCPU?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char
IsGCPU (
  OUT void
  );

//AMD Library Routines (SBPELIB.C)
/**
 * Assert/deassert Hudson-2 pins used to toggle SB GPP reset or NB PCIE reset
 *
 *
 * @param[in] ResetBlock - PCIE reset for SB GPP or NB PCIE
 * @param[in] ResetOp    - Assert or deassert PCIE reset
 *
 */
void SbResetPcie (IN RESET_BLOCK  ResetBlock, IN RESET_OP ResetOp);

/**
 * sbGppTogglePcieReset - Toggle PCIE_RST2#
 *
 *
 * @param[in] pConfig
 *
 */
void sbGppTogglePcieReset (IN AMDSBCFG* pConfig);

/**
 * sbSpiUnlock - Sb SPI Unlock
 *
 *
 * @param[in] pConfig
 *
 */
void sbSpiUnlock (IN AMDSBCFG* pConfig);

/**
 * sbSpilock - Sb SPI lock
 *
 *
 * @param[in] pConfig
 *
 */
void sbSpilock (IN AMDSBCFG* pConfig);

/**
 * programPciByteTable - Program PCI register by table (8 bits data)
 *
 *
 *
 * @param[in] pPciByteTable    - Table data pointer
 * @param[in] dwTableSize      - Table length
 *
 */
void  programPciByteTable (IN REG8MASK* pPciByteTable, IN unsigned short dwTableSize);

/**
 * programSbAcpiMmioTbl - Program SB ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 *
 */
void  programSbAcpiMmioTbl (IN AcpiRegWrite  *pAcpiTbl);

/**
 * getChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - Hudson-2 chip Strap Status
 *    @li <b>0001</b> - Not USED FWH
 *    @li <b>0002</b> - Not USED LPC ROM
 *    @li <b>0004</b> - EC enabled
 *    @li <b>0008</b> - Reserved
 *    @li <b>0010</b> - Internal Clock mode
 *
 */
void getChipSysMode (IN void* Value);

/**
 * isImcEnabled - Is IMC Enabled
 * @retval  TRUE for IMC Enabled; FALSE for IMC Disabled
 */
unsigned char isImcEnabled (void);

/**
 * Read Southbridge CIMx configuration structure pointer
 *
 *
 *
 * @retval  0xXXXXXXXX   CIMx configuration structure pointer.
 *
 */
AMDSBCFG* getConfigPointer (OUT void);

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
void  ReadPMIO (IN unsigned char Address, IN unsigned char OpFlag, IN void* Value);

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
void  WritePMIO (IN unsigned char Address, IN unsigned char OpFlag, IN void* Value);

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
void  RWPMIO (IN unsigned char Address, IN unsigned char OpFlag, IN unsigned int AndMask, IN unsigned int OrMask);

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
void  ReadPMIO2 (IN unsigned char Address, IN unsigned char OpFlag, IN void* Value);

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
void  WritePMIO2 (IN unsigned char Address, IN unsigned char OpFlag, IN void* Value);

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
void  RWPMIO2 (IN unsigned char Address, IN unsigned char  OpFlag, IN unsigned int AndMask, IN unsigned int OrMask);
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
void  EnterEcConfig (void);

/**
 * ExitEcConfig - Force EC exit Config mode
 *
 *
 *
 *
 */
void  ExitEcConfig (void);

/**
 * ReadEC8 - Read EC register data
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Read Data Buffer
 *
 */
void  ReadEC8 (IN unsigned char Address, IN unsigned char* Value);

/**
 * WriteEC8 - Write date into EC register
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Write Data Buffer
 *
 */
void  WriteEC8 (IN unsigned char Address, IN unsigned char* Value);

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
void  RWEC8 (IN unsigned char Address, IN unsigned char AndMask, IN unsigned char OrMask);

unsigned char IsZoneFuncEnable (IN unsigned short Flag, IN unsigned char func, IN unsigned char Zone);
void sbECfancontrolservice (IN AMDSBCFG* pConfig);
void hwmImcInit (IN AMDSBCFG* pConfig);
void GetSbAcpiMmioBase (OUT unsigned int* AcpiMmioBase);
void GetSbAcpiPmBase (OUT  unsigned short* AcpiPmBase);
void SetAcpiPma (IN  unsigned char pmaControl);
void imcEnableSurebootTimer (IN AMDSBCFG* pConfig);
void imcDisableSurebootTimer (IN AMDSBCFG* pConfig);
void imcDisarmSurebootTimer (IN AMDSBCFG* pConfig);
void hwmSbtsiAutoPolling (IN AMDSBCFG* pConfig);
void hwmSbtsiAutoPollingOff (IN AMDSBCFG* pConfig);
void hwmSbtsiAutoPollingPause (IN AMDSBCFG* pConfig);
void imcSleep (IN AMDSBCFG* pConfig);
void imcWakeup (IN AMDSBCFG* pConfig);
void imcIdle (IN AMDSBCFG* pConfig);
void imcThermalZoneEnable (IN AMDSBCFG* pConfig);
void ValidateFchVariant (IN AMDSBCFG* pConfig);
void CheckEfuse (IN AMDSBCFG* pConfig);

/**
 * Is UMI One Lane GEN1 Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
unsigned char IsUmiOneLaneGen1Mode ( OUT void );

/**
 * Record SMI Status
 *
 *
 * @retval  Nothing
 *
 */
void RecordSmiStatus ( OUT void );
// #endif

