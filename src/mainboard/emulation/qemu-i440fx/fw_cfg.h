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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

void fw_cfg_get(int entry, void *dst, int dstlen);
int fw_cfg_check_file(const char *name);
void fw_cfg_load_file(const char *name, void *dst);
int fw_cfg_max_cpus(void);
unsigned long fw_cfg_smbios_tables(int *handle, unsigned long *current);
