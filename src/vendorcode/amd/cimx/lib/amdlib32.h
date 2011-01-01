#ifndef _AMDLIB32_H_
#define _AMDLIB32_H_

#include "cbtypes.h"
#include "Amd.h"

UINT8 ReadIo8 (IN UINT16 port);
UINT16 ReadIo16 (IN UINT16 port);
UINT32 ReadIo32 (IN UINT16 port);
VOID WriteIo8 (IN UINT16 port, IN UINT8 value);
VOID WriteIo16 (IN UINT16 port, IN UINT16 value);
VOID WriteIo32 (IN UINT16 port, IN UINT32 value);
UINT64 ReadTSC(VOID);
VOID CpuidRead(IN UINT32 op, IN OUT SB_CPUID_DATA* Data);
UINT8 ReadNumberOfCpuCores(VOID);
#endif //_AMDLIB32_H_
