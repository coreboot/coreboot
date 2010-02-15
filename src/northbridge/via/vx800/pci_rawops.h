/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef ARCH_I386_PCI_RAWOPS_H
# define ARCH_I386_PCI_RAWOPS_H 1
#include <stdint.h>

#define PCI_RAWDEV(SEGBUS, DEV, FN) ( \
        (((SEGBUS) & 0xFFF) << 20) | \
        (((DEV) & 0x1F) << 15) | \
        (((FN)  & 0x07) << 12))
struct  VIA_PCI_REG_INIT_TABLE {
	u8 ChipRevisionStart;
	u8 ChipRevisionEnd;
	u8 Bus;
	u8 Device;
	u8 Function;
        u32 Register;
	u8 Mask;
	u8 Value;
};
typedef unsigned device_t_raw; /* pci and pci_mmio need to have different ways to have dev */

/* FIXME: We need to make the coreboot to run at 64bit mode, So when read/write memory above 4G,
 * We don't need to set %fs, and %gs anymore
 * Before that We need to use %gs, and leave %fs to other RAM access
 */
 uint8_t pci_io_rawread_config8(device_t_raw dev, unsigned where)
{
	unsigned addr;
#if PCI_IO_CFG_EXT == 0
	addr = (dev>>4) | where;
#else
	addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16); //seg == 0
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

#if MMCONF_SUPPORT
 uint8_t pci_mmio_rawread_config8(device_t_raw dev, unsigned where)
{
        unsigned addr;
        addr = dev | where;
        return read8x(addr);
}
#endif
 uint8_t pci_rawread_config8(device_t_raw dev, unsigned where)
{
#if MMCONF_SUPPORT
	return pci_mmio_rawread_config8(dev, where);
#else
	return pci_io_rawread_config8(dev, where);
#endif
}

 uint16_t pci_io_rawread_config16(device_t_raw dev, unsigned where)
{
	unsigned addr;
#if PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

#if MMCONF_SUPPORT
 uint16_t pci_mmio_rawread_config16(device_t_raw dev, unsigned where)
{
        unsigned addr;
        addr = dev | where;
        return read16x(addr);
}
#endif

 uint16_t pci_rawread_config16(device_t_raw dev, unsigned where)
{
#if MMCONF_SUPPORT
	return pci_mmio_rawread_config16(dev, where);
#else
        return pci_io_rawread_config16(dev, where);
#endif
}


 uint32_t pci_io_rawread_config32(device_t_raw dev, unsigned where)
{
	unsigned addr;
#if PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

#if MMCONF_SUPPORT
 uint32_t pci_mmio_rawread_config32(device_t_raw dev, unsigned where)
{
        unsigned addr;
        addr = dev | where;
        return read32x(addr);
}
#endif

 uint32_t pci_rawread_config32(device_t_raw dev, unsigned where)
{
#if MMCONF_SUPPORT
	return pci_mmio_rawread_config32(dev, where);
#else
        return pci_io_rawread_config32(dev, where);
#endif
}

 void pci_io_rawwrite_config8(device_t_raw dev, unsigned where, uint8_t value)
{
	unsigned addr;
#if PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

#if MMCONF_SUPPORT
 void pci_mmio_rawwrite_config8(device_t_raw dev, unsigned where, uint8_t value)
{
        unsigned addr;
        addr = dev | where;
        write8x(addr, value);
}
#endif

 void pci_rawwrite_config8(device_t_raw dev, unsigned where, uint8_t value)
{
#if MMCONF_SUPPORT
	pci_mmio_rawwrite_config8(dev, where, value);
#else
        pci_io_rawwrite_config8(dev, where, value);
#endif
}


 void pci_io_rawwrite_config16(device_t_raw dev, unsigned where, uint16_t value)
{
        unsigned addr;
#if PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outw(value, 0xCFC + (addr & 2));
}

#if MMCONF_SUPPORT
 void pci_mmio_rawwrite_config16(device_t_raw dev, unsigned where, uint16_t value)
{
        unsigned addr;
        addr = dev | where;
        write16x(addr, value);
}
#endif

 void pci_rawwrite_config16(device_t_raw dev, unsigned where, uint16_t value)
{
#if MMCONF_SUPPORT
	pci_mmio_rawwrite_config16(dev, where, value);
#else
	pci_io_rawwrite_config16(dev, where, value);
#endif
}


 void pci_io_rawwrite_config32(device_t_raw dev, unsigned where, uint32_t value)
{
	unsigned addr;
#if PCI_IO_CFG_EXT == 0
        addr = (dev>>4) | where;
#else
        addr = (dev>>4) | (where & 0xff) | ((where & 0xf00)<<16);
#endif
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#if MMCONF_SUPPORT
 void pci_mmio_rawwrite_config32(device_t_raw dev, unsigned where, uint32_t value)
{
        unsigned addr;
        addr = dev | where;
        write32x(addr, value);
}
#endif

 void pci_rawwrite_config32(device_t_raw dev, unsigned where, uint32_t value)
{
#if MMCONF_SUPPORT
	pci_mmio_rawwrite_config32(dev, where, value);
#else
        pci_io_rawwrite_config32(dev, where, value);
#endif
}


void pci_rawmodify_config8(device_t_raw dev, unsigned where, u8 orval,u8 mask)
{     u8 data=pci_rawread_config8(dev,where);
       data&=(~mask);
       data|=orval;
       pci_rawwrite_config8(dev,where,data);
}
void pci_rawmodify_config16(device_t_raw dev, unsigned where, uint16_t orval,uint16_t mask)
{     uint16_t data=pci_rawread_config16(dev,where);
       data&=(~mask);
       data|=orval;
       pci_rawwrite_config16(dev,where,data);
}
void pci_rawmodify_config32(device_t_raw dev, unsigned where, uint32_t orval,uint32_t mask)
{     uint32_t data=pci_rawread_config32(dev,where);
       data&=(~mask);
       data|=orval;
       pci_rawwrite_config32(dev,where,data);
}

void io_rawmodify_config8(u16 where, uint8_t orval,uint8_t mask)
{
	u8 data=inb(where);
       data&=(~mask);
       data|=orval;
       outb(data,where);
}

void via_pci_inittable(u8 chipversion,struct VIA_PCI_REG_INIT_TABLE* initdata)
{
        u8 i=0;
	device_t_raw devbxdxfx;
	for(i=0;;i++)	{
			if((initdata[i].Mask==0)&&(initdata[i].Value==0)&&(initdata[i].Bus==0)&&(initdata[i].ChipRevisionEnd==0xff)&&(initdata[i].ChipRevisionStart==0)&&(initdata[i].Device==0)&&(initdata[i].Function==0)&&(initdata[i].Register==0))
				break;
			if((chipversion>=initdata[i].ChipRevisionStart)&&(chipversion<=initdata[i].ChipRevisionEnd)){
				devbxdxfx=PCI_RAWDEV(initdata[i].Bus,initdata[i].Device,initdata[i].Function);
				pci_rawmodify_config8(devbxdxfx, initdata[i].Register,initdata[i].Value,initdata[i].Mask);
			}
	}
}
#endif
