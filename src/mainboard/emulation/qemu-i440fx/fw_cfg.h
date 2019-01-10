/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef FW_CFG_H
#define FW_CFG_H
#include "fw_cfg_if.h"

void fw_cfg_get(uint16_t entry, void *dst, int dstlen);
int fw_cfg_check_file(FWCfgFile *file, const char *name);
int fw_cfg_max_cpus(void);
unsigned long fw_cfg_smbios_tables(int *handle, unsigned long *current);
uintptr_t fw_cfg_tolud(void);

#endif /* FW_CFG_H */
