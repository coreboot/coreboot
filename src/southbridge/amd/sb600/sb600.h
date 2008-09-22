/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SB600_H
#define SB600_H

#include "chip.h"

#define PCI_DEVICE_ID_ATI_SB600_LPC 0x438D
#define PCI_DEVICE_ID_ATI_SB600_SATA 0x4380
#define PCI_DEVICE_ID_ATI_SB600_IDE 0x438C
#define PCI_DEVICE_ID_ATI_SB600_HDA 0x4383
#define PCI_DEVICE_ID_ATI_SB600_ACI 0x4382
#define PCI_DEVICE_ID_ATI_SB600_MCI 0x438E
#define PCI_DEVICE_ID_ATI_SB600_USB2 0x4386
#define PCI_DEVICE_ID_ATI_SB600_PCI  0x4384
#define PCI_DEVICE_ID_ATI_SB600_SM 0x4385
#define PCI_DEVICE_ID_ATI_SB600_USB_0 0x4387
#define PCI_DEVICE_ID_ATI_SB600_USB_1 0x4388
#define PCI_DEVICE_ID_ATI_SB600_USB_2 0x4389
#define PCI_DEVICE_ID_ATI_SB600_USB_3 0x438A
#define PCI_DEVICE_ID_ATI_SB600_USB_4 0x438B
extern void pm_iowrite(u8 reg, u8 value);
extern u8 pm_ioread(u8 reg);
extern void pm2_iowrite(u8 reg, u8 value);
extern u8 pm2_ioread(u8 reg);
extern void set_sm_enable_bits(device_t sm_dev, u32 reg_pos, u32 mask, u32 val);

void sb600_enable(device_t dev);

#endif /* SB600_H */
