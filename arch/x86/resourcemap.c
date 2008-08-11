/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <cpu.h>
#include <io.h>
#include <config.h>
/**
 * setup a resource map. 
 * for PCRM entries, add a pci device offset, and a pci "OR value offset"
 * for IO8 and IO32 entries, add an io base offset. 
 * This function combines a bunch of seperate functions that were scattered
 * throughout v2. It may be awkward but it does allow for one rmap for 
 * all settings, which is handy. See include/arch/x86/cpu.h for usage on 
 * how to set up a resource map. 
 *
 * @param rm The resource map
 * @param max The map size
 * @param offset_bdf pci device offset. Note this is a u32 in 
 * 			busdevfn format. See PCI_BDF macro if you are not sure what that is. 
 * @param offset_pciio added to the OR value for setting up PCI IO
 * @param offset_io offset from the io base in the resource map
 */

/* NOTE: By doing the config write in this manner we guarantee that this
 * will work in stage1 or stage2.
 */
#define pci_read_config32(busdevfn, where) pci_cf8_conf1.read32(busdevfn, where)
#define pci_write_config32(busdevfn, where, what) pci_cf8_conf1.write32(busdevfn, where, what)

void setup_resource_map_x_offset(const struct rmap *rm, u32 max,
                                 u32 offset_bdf, u32 offset_pciio, 
                                 u32 offset_io)
{
	u32 i;

	printk(BIOS_DEBUG, "setting up resource map ex offset....\n");

	for(i = 0; i < max; i++, rm++) {
          switch (rm->type){
		case TPCIRM: 
			{
                          u32 dev;
                          unsigned where;
                          unsigned long reg;
			u8 offset_devfn = offset_bdf;
#warning make sure offset_bus is right for extended PCI addressing
			u32 offset_bus = offset_bdf >> 8;
			  printk(BIOS_DEBUG, "(%x+%x,%x+%x,%x+%x,%x) & %08x | %08x+%08x\n", rm->pcm.bus,
				offset_bus, rm->pcm.dev, (offset_devfn>>3),
                                 rm->pcm.fn, offset_devfn&3, rm->pcm.reg,
				 rm->pcm.and,rm->pcm.or, offset_pciio);
                          dev = rm->pcm.dev;
                        where = rm->pcm.reg;
                          dev <<= 3;
                          dev |= rm->pcm.fn;
                            dev += offset_devfn;
                          reg = pci_read_config32(PCI_BDEVFN(rm->pcm.bus + offset_bus, dev),  where);
                          reg &= rm->pcm.and;
                          reg |= rm->pcm.or + offset_pciio; 
                          pci_write_config32(PCI_BDEVFN(rm->pcm.bus + offset_bus, dev), where, reg);
			}
			break;
		case TIO8:
			{
                          u32 where;
                          u8 reg;
                          printk(BIOS_DEBUG, "(%04x+%04x) &  %02x | %02xx\n", rm->io8.port, offset_io, rm->io8.and,rm->io8.or);
                          where = rm->io8.port + offset_io;
                          reg = inb(where);
                          reg &= rm->io8.and;
                          reg |= rm->io8.or;
                          outb(reg, where);
			}
			break;
		case TIO32:
			{
                          u32 where;
                          u32 reg;
                          printk(BIOS_DEBUG, "(%04x+%04x) &  %02x | %02xx\n", rm->io32.port, offset_io, rm->io32.and,rm->io32.or);
                          where = rm->io32.port + offset_io;
                          reg = inl(where);
                          reg &= rm->io32.and;
                          reg |= rm->io32.or;
                          outl(reg, where);
			}
			break;
		} // switch


	}

	printk(BIOS_DEBUG, "done.\n");
}

/**
 * setup a resource map

 * @param rm The resource map
 * @param max The map size
 */

void setup_resource_map(const struct rmap *rm, u32 max)
{

  setup_resource_map_x_offset(rm, max, 0, 0, 0);
}

