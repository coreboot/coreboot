/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PCI_H
#define _PCI_H

#include <arch/types.h>
typedef u32 pcidev_t;

#define REG_VENDOR_ID   0x00
#define REG_COMMAND     0x04
#define REG_CLASS_DEV   0x0A
#define REG_HEADER_TYPE 0x0E
#define REG_PRIMARY_BUS 0x18
#define REG_SUBSYS_VENDOR_ID 0x2C
#define REG_SUBSYS_ID   0x2E

#define REG_COMMAND_IO  (1 << 0)
#define REG_COMMAND_MEM (1 << 1)
#define REG_COMMAND_BM  (1 << 2)

#define HEADER_TYPE_NORMAL  0
#define HEADER_TYPE_BRIDGE  1
#define HEADER_TYPE_CARDBUS 2

#define PCI_ADDR(_bus, _dev, _fn, _reg) \
(0x80000000 | (_bus << 16) | (_dev << 11) | (_fn << 8) | (_reg & ~3))

#define PCI_DEV(_bus, _dev, _fn) \
(0x80000000 | (_bus << 16) | (_dev << 11) | (_fn << 8))

#define PCI_BUS(_d)  ((_d >> 16) & 0xff)
#define PCI_SLOT(_d) ((_d >> 11) & 0x1f)
#define PCI_FUNC(_d) ((_d >> 8) & 0x7)

u8 pci_read_config8(u32 device, u16 reg);
u16 pci_read_config16(u32 device, u16 reg);
u32 pci_read_config32(u32 device, u16 reg);

void pci_write_config8(u32 device, u16 reg, u8 val);
void pci_write_config16(u32 device, u16 reg, u16 val);
void pci_write_config32(u32 device, u16 reg, u32 val);

int pci_find_device(u16 vid, u16 did, pcidev_t *dev);
u32 pci_read_resource(pcidev_t dev, int bar);

void pci_set_bus_master(pcidev_t dev);

#endif
