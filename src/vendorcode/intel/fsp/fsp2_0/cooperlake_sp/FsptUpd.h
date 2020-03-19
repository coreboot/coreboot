#ifndef __FSPTUPD_H__
#define __FSPTUPD_H__

#include <FspUpd.h>

#pragma pack(1)

typedef struct {
  uint32_t                    MicrocodeRegionBase;
  uint32_t                    MicrocodeRegionLength;
  uint32_t                    CodeRegionBase;
  uint32_t                    CodeRegionLength;
  uint8_t                     Reserved1[16];
} FSPT_CORE_UPD;

typedef struct {
  uint8_t                     PcdFsptPort80RouteDisable;
  uint8_t                     ReservedTempRamInitUpd[31];
} FSPT_CONFIG;

typedef struct {
  FSP_UPD_HEADER              FspUpdHeader;
  FSPT_CORE_UPD               FsptCoreUpd;
  FSPT_CONFIG                 FsptConfig;
  uint8_t                     UnusedUpdSpace0[6];
  uint16_t                    UpdTerminator;
} FSPT_UPD;

#pragma pack()

#endif
