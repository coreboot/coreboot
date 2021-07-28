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

#include <libpayload.h>
#include <pci.h>

u8 pci_read_config8(pcidev_t dev, u16 reg)
{
	outl(dev | (reg & ~3), 0xCF8);
	return inb(0xCFC + (reg & 3));
}

u16 pci_read_config16(pcidev_t dev, u16 reg)
{
	outl(dev | (reg & ~3), 0xCF8);
	return inw(0xCFC + (reg & 3));
}

u32 pci_read_config32(pcidev_t dev, u16 reg)
{
	outl(dev | (reg & ~3), 0xCF8);
	return inl(0xCFC + (reg & 3));
}

void pci_write_config8(pcidev_t dev, u16 reg, u8 val)
{
	outl(dev | (reg & ~3), 0xCF8);
	outb(val, 0xCFC + (reg & 3));
}

void pci_write_config16(pcidev_t dev, u16 reg, u16 val)
{
	outl(dev | (reg & ~3), 0xCF8);
	outw(val, 0xCFC + (reg & 3));
}

void pci_write_config32(pcidev_t dev, u16 reg, u32 val)
{
	outl(dev | (reg & ~3), 0xCF8);
	outl(val, 0xCFC + (reg & 3));
}
