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

#ifndef __ROMCC__
#include <device/device.h>
void mcp55_enable(struct device *dev);
extern struct pci_operations mcp55_pci_ops;
#endif

void enable_fid_change_on_sb(unsigned int sbbusn, unsigned int sbdn);
void enable_smbus(void);

/* Concflict declarations with <device/smbus.h>. */
#if !ENV_RAMSTAGE
int smbus_recv_byte(unsigned int device);
int smbus_send_byte(unsigned int device, unsigned char val);
int smbus_read_byte(unsigned int device, unsigned int address);
int smbus_write_byte(unsigned int device, unsigned int address, unsigned char val);
int smbusx_recv_byte(unsigned int smb_index, unsigned int device);
int smbusx_send_byte(unsigned int smb_index, unsigned int device, unsigned char val);
int smbusx_read_byte(unsigned int smb_index, unsigned int device, unsigned int address);
int smbusx_write_byte(unsigned int smb_index, unsigned int device, unsigned int address,
		unsigned char val);
#endif /* !ENV_RAMSTAGE */

#endif
