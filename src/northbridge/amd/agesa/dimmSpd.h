/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DIMMSPD_H_
#define _DIMMSPD_H_

AGESA_STATUS
AmdMemoryReadSPD(IN UINT32 Func, IN UINTN Data, IN OUT AGESA_READ_SPD_PARAMS *SpdData);

int hudson_readSpd(int spdAddress, char *buf, size_t len);

int smbus_readSpd(int spdAddress, char *buf, size_t len);

#endif
