/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define SMBUS_MEM_DEVICE_START 0x50
#define SMBUS_MEM_DEVICE_END 0x53
#define SMBUS_MEM_DEVICE_INC 1

static void print_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0); 
		dev <= PCI_DEV(0, 0x1f, 0x7); 
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		printk_debug("PCI: %02x:%02x.%02x", (dev >> 20) & 0xff,
			(dev >> 15) & 0x1f, (dev >> 12) & 7);
		printk_debug(" [%04x:%04x]\r\n", id &0xffff, id >> 16);
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;

	printk_debug("PCI: %02x:%02x.%02x\r\n", (dev >> 20) & 0xff, (dev >> 15) & 0x1f, (dev >> 12) & 7);

	for(i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			printk_debug("%02x:", i);
		}
		val = pci_read_config8(dev, i);
		printk_debug(" %02x", val);
		if ((i & 0x0f) == 0x0f) {
			printk_debug("\r\n");
		}
	}
}

static void dump_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0); 
		dev <= PCI_DEV(0, 0x1f, 0x7); 
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}

void dump_spd_registers(void)
{
        unsigned device;
        device = SMBUS_MEM_DEVICE_START;
        while(device <= SMBUS_MEM_DEVICE_END) {
                int status = 0;
                int i;
        	printk_debug("\r\ndimm %02x", device);
		
                for(i = 0; (i < 256) ; i++) {
                        if ((i % 16) == 0) {
				printk_debug("\r\n%02x: ", i);
                        }
			status = smbus_read_byte(device, i);
                        if (status < 0) {
			         printk_debug("bad device: %02x\r\n", -status);
			         break; 
			}
			printk_debug("%02x ", status);
		}
		device += SMBUS_MEM_DEVICE_INC;
		printk_debug("\r\n");
	}
}

static void dump_mem(unsigned start, unsigned end)
{
        unsigned i;
	print_debug("dump_mem:");
        for(i=start;i<end;i++) {
		if((i & 0xf)==0) {
#if CONFIG_USE_INIT
			printk_debug("\r\n%08x:", i);
#else	
			print_debug("\r\n");
			print_debug_hex32(i);
			print_debug(":");
#endif
		}
#if CONFIG_USE_INIT	
		printk_debug(" %02x", (unsigned char)*((unsigned char *)i));
#else
		print_debug(" ");
             	print_debug_hex8((unsigned char)*((unsigned char *)i));
#endif
        }
        print_debug("\r\n");
 }
