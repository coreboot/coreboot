/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef FW_CFG_H
#define FW_CFG_H
#include "fw_cfg_if.h"

void fw_cfg_get(uint16_t entry, void *dst, int dstlen);
int fw_cfg_check_file(FWCfgFile *file, const char *name);
int fw_cfg_max_cpus(void);
unsigned long fw_cfg_smbios_tables(int *handle, unsigned long *current);
uintptr_t fw_cfg_tolud(void);

#endif /* FW_CFG_H */
