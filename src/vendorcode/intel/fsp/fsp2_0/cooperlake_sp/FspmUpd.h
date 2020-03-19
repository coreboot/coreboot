#ifndef __FSPMUPD_H__
#define __FSPMUPD_H__

#include <FspEas.h>
#include <FspUpd.h>

#pragma pack (1)

typedef struct {
uint8_t padding[208];
} FSPM_CONFIG;

typedef struct {
  FSP_UPD_HEADER              FspUpdHeader;
  FSPM_ARCH_UPD               FspmArchUpd;
  FSPM_CONFIG                 FspmConfig;
  uint16_t                    UpdTerminator;
} FSPM_UPD;

#pragma pack(1)

#endif
