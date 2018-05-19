/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOUTHBRIDGE_NVIDIA_MCP55_MCP55_H
#define SOUTHBRIDGE_NVIDIA_MCP55_MCP55_H

#if CONFIG_HT_CHAIN_END_UNITID_BASE != 0x20
#define MCP55_DEVN_BASE CONFIG_HT_CHAIN_END_UNITID_BASE
#else
#define MCP55_DEVN_BASE CONFIG_HT_CHAIN_UNITID_BASE
#endif

#ifndef __PRE_RAM__
#include "chip.h"
void mcp55_enable(struct device *dev);
extern struct pci_operations mcp55_pci_ops;
#else
void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn);
void enable_smbus(void);
int smbus_recv_byte(unsigned device);
int smbus_send_byte(unsigned device, unsigned char val);
int smbus_read_byte(unsigned device, unsigned address);
int smbus_write_byte(unsigned device, unsigned address, unsigned char val);
int smbusx_recv_byte(unsigned smb_index, unsigned device);
int smbusx_send_byte(unsigned smb_index, unsigned device, unsigned char val);
int smbusx_read_byte(unsigned smb_index, unsigned device, unsigned address);
int smbusx_write_byte(unsigned smb_index, unsigned device, unsigned address,
		unsigned char val);
#endif

#endif
