/*
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
#include <stdint.h>

typedef u32 pcidev_t;

/* Device config space registers. */
#define REG_VENDOR_ID           0x00
#define REG_DEVICE_ID           0x02
#define REG_COMMAND             0x04
#define REG_STATUS              0x06
#define REG_REVISION_ID         0x08
#define REG_PROG_IF             0x09
#define REG_SUBCLASS            0x0A
#define REG_CLASS               0x0B
#define REG_CACHE_LINE_SIZE     0x0C
#define REG_LATENCY_TIMER       0x0D
#define REG_HEADER_TYPE         0x0E
#define REG_BIST                0x0F
#define REG_BAR0                0x10
#define REG_BAR1                0x14
#define REG_BAR2                0x18
#define REG_BAR3                0x1C
#define REG_BAR4                0x20
#define REG_BAR5                0x24
#define REG_CARDBUS_CIS_POINTER 0x28
#define REG_SUBSYS_VENDOR_ID    0x2C
#define REG_SUBSYS_ID           0x2E
#define REG_DEV_OPROM_BASE      0x30
#define REG_CAP_POINTER         0x34
#define REG_INTERRUPT_LINE      0x3C
#define REG_INTERRUPT_PIN       0x3D
#define REG_MIN_GRANT           0x3E
#define REG_MAX_LATENCY         0x3F

/* Bridge config space registers. */
#define REG_PRIMARY_BUS         0x18
#define REG_SECONDARY_BUS       0x19
#define REG_SUBORDINATE_BUS     0x1A
#define REG_SECONDARY_LATENCY   0x1B
#define REG_IO_BASE             0x1C
#define REG_IO_LIMIT            0x1D
#define REG_SECONDARY_STATUS    0x1E
#define REG_MEMORY_BASE         0x20
#define REG_MEMORY_LIMIT        0x22
#define REG_PREFETCH_MEM_BASE   0x24
#define REG_PREFETCH_MEM_LIMIT  0x26
#define REG_PREFETCH_BASE_UPPER 0x28
#define REG_PREFETCH_LIMIT_UPPER 0x2C
#define REG_IO_BASE_UPPER       0x30
#define REG_IO_LIMIT_UPPER      0x32
#define REG_BRIDGE_OPROM_BASE   0x38
#define REG_BRIDGE_CONTROL      0x3C

#define REG_COMMAND_IO  (1 << 0)
#define REG_COMMAND_MEM (1 << 1)
#define REG_COMMAND_BM  (1 << 2)

#define HEADER_TYPE_NORMAL        0
#define HEADER_TYPE_BRIDGE        1
#define HEADER_TYPE_CARDBUS       2
#define HEADER_TYPE_MULTIFUNCTION 0x80

#define PCI_DEV(_bus, _dev, _fn) (0x80000000 | \
(uint32_t)(_bus << 16) | (uint32_t)(_dev << 11) | (uint32_t)(_fn << 8))

#define PCI_ADDR(_bus, _dev, _fn, _reg) \
(PCI_DEV(_bus, _dev, _fn) | (uint8_t)(_reg & ~3))

#define PCI_BUS(_d)  ((_d >> 16) & 0xff)
#define PCI_SLOT(_d) ((_d >> 11) & 0x1f)
#define PCI_FUNC(_d) ((_d >> 8) & 0x7)

uintptr_t pci_map_bus(pcidev_t dev);

u8 pci_read_config8(pcidev_t dev, u16 reg);
u16 pci_read_config16(pcidev_t dev, u16 reg);
u32 pci_read_config32(pcidev_t dev, u16 reg);

void pci_write_config8(pcidev_t dev, u16 reg, u8 val);
void pci_write_config16(pcidev_t dev, u16 reg, u16 val);
void pci_write_config32(pcidev_t dev, u16 reg, u32 val);

int pci_find_device(u16 vid, u16 did, pcidev_t *dev);
u32 pci_read_resource(pcidev_t dev, int bar);

void pci_set_bus_master(pcidev_t dev);

#endif
