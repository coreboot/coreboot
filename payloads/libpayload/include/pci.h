/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#ifndef _PCI_H_
#define _PCI_H_

typedef unsigned short pcidev_t;

#define REG_VENDOR_ID   0x00
#define REG_DEVICE_ID   0x04
#define REG_HEADER_TYPE 0x0E
#define REG_PRIMARY_BUS 0x18

#define HEADER_TYPE_NORMAL  0
#define HEADER_TYPE_BRIDGE  1
#define HEADER_TYPE_CARDBUS 2

#define PCIDEV(_b, _d) ((((_b) & 0xFF) << 8) | ((_d) & 0xFF))

#define PCIDEV_BUS(_d) (((_d) >> 8) & 0xFF)
#define PCIDEV_DEVFN(_d) ((_d) & 0xFF)

#define PCI_ADDR(_bus, _dev, _reg) \
(0x80000000 | (_bus << 16) | (_dev << 8) | (_reg & ~3))

void pci_read_dword(unsigned int bus, unsigned int devfn,
		    unsigned int reg, unsigned int *val);

void pci_read_byte(unsigned int bus, unsigned int devfn,
		   unsigned int reg, unsigned char *val);

int pci_find_device(unsigned short vid, unsigned short did, pcidev_t *dev);
unsigned int pci_read_resource(pcidev_t dev, int bar);

#endif
