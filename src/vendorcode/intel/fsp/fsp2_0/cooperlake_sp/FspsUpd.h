#ifndef __FSPSUPD_H__
#define __FSPSUPD_H__

#pragma pack(1)

#include <FspUpd.h>

typedef struct {
  uint8_t padding[54];
} FSPS_CONFIG;

typedef struct {
  FSP_UPD_HEADER              FspUpdHeader;
  FSPS_CONFIG                 FspsConfig;
  uint16_t                    UpdTerminator;
} FSPS_UPD;

#pragma pack(1)

#endif
