/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ronald G. Minnich
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
 * @param offset_dev pci device offset. This can be useful on e.g. k8
 *        we have a number of similar devices which need the same setups
 *        we can use one map for more than one device. NOTE: 
 *        offset_dev IS NOT ASSUMED TO BE OFFSET BY FN (i.e. it is not << 3)
 * @param offset_pciio added to the OR value for setting up PCI IO
 * @param offset_io offset from the io base in the resource map
 */

/* NOTE: By doing the config write in this manner we guarantee that this
 * will work in stage1 or stage2.
 */
#define pci_config_read32(bus, dev, where) pci_cf8_conf1.read32(NULL, r->pcm.bus, dev, where)
#define pci_config_write32(bus, dev, where, what) pci_cf8_conf1.write32(NULL, r->pcm.bus, dev, where, what)

void setup_resource_map_x_offset(const rmap *rm, u32 max,
                                 u32 offset_dev, u32 offset_pciio, 
                                 u32 offset_io)
{
	u32 i;

	printk(BIOS_DEBUG, "setting up resource map ex offset....\n");

	for(i = 0; i < max; i++, rm++) {
          switch (rm->type){
		case PCIRM: 
			{
                          u32 dev;
                          unsigned where;
                          unsigned long reg;
			  printk(BIOS_DEBUG, "(%x,%x+%x,%x,%x) & %08x | %08x+%08x\n", rm->pcm.bus,rm->pcm.dev+offset_dev,
                                 rm->pcm.fn,rm->pcm.reg,
				 rm->pcm.and,rm->pcm.or, offset_pciio);
                          dev = rm->pcm.dev;
                          dev += offset_dev;
                          where = rm->pcm.reg;
                          dev <<= 3;
                          dev |= rm->pcm.fn;
                          reg = pci_config_read32(rm->pcm.bus, dev, where);
                          reg &= rm->pcm.and;
                          reg |= rm->pcm.or + offset_pciio; 
                          pci_config_write32(rm->pcm.bus, dev, where, reg);
			}
			break;
		case IO8:
			{
                          u32 where;
                          u8 reg;
                          printk(BIOS_DEBUG, "(%04x+%04x) &  %02x | %02xx\n", rm->port, offset_io, rm->pcm.and,rm->pcm.or);
                          where = rm->port + offset_io;
                          reg = inb(where);
                          reg &= rm->and;
                          reg |= rm->or;
                          outb(reg, where);
			}
			break;
		case IO32:
			{
                          u32 where;
                          u32 reg;
                          printk(BIOS_DEBUG, "(%04x+%04x) &  %02x | %02xx\n", rm->port, offset_io, rm->pcm.and,rm->pcm.or);
                          where = rm->port + offset_io;
                          reg = inl(where);
                          reg &= rm->and;
                          reg |= rm->or;
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

  setup_resource_map_x_offset(rm, max);
}

