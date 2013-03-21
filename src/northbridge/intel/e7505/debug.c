
#include <device/pci_def.h>
#include <console/console.h>
#include <stdlib.h>
#include <arch/io.h>
#include <spd.h>

#include "raminit.h"
#include "debug.h"

/*
 * generic debug code, used by mainboard specific romstage.c
 *
 */

void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 16) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 11) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 8) & 7);
}

void print_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		print_debug("\n");
	}
}

void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);

	for(i = 0; i < 256; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
#if !defined(__ROMCC__)
                        printk(BIOS_DEBUG, "\n%02x:",i);
#else
			print_debug("\n");
			print_debug_hex8(i);
			print_debug_char(':');
#endif
		}
		val = pci_read_config8(dev, i);
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, " %02x", val);
#else
		print_debug_char(' ');
		print_debug_hex8(val);
#endif
	}
	print_debug("\n");
}

void dump_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0);
		dev <= PCI_DEV(0xff, 0x1f, 0x7);
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

void dump_pci_devices_on_bus(unsigned busn)
{
        device_t dev;
        for(dev = PCI_DEV(busn, 0, 0);
                dev <= PCI_DEV(busn, 0x1f, 0x7);
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

void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
#if !defined(__ROMCC__)
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
#else
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".0: ");
			print_debug_hex8(device);
#endif
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
#if !defined(__ROMCC__)
					printk(BIOS_DEBUG, "\n%02x: ", j);
#else
					print_debug("\n");
					print_debug_hex8(j);
					print_debug(": ");
#endif
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
#if !defined(__ROMCC__)
				printk(BIOS_DEBUG, "%02x ", byte);
#else
				print_debug_hex8(byte);
				print_debug_char(' ');
#endif
			}
			print_debug("\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
#if !defined(__ROMCC__)
                        printk(BIOS_DEBUG, "dimm: %02x.1: %02x", i, device);
#else
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".1: ");
			print_debug_hex8(device);
#endif
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
#if !defined(__ROMCC__)
                                        printk(BIOS_DEBUG, "\n%02x: ", j);
#else
					print_debug("\n");
					print_debug_hex8(j);
					print_debug(": ");
#endif
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
#if !defined(__ROMCC__)
                                printk(BIOS_DEBUG, "%02x ", byte);
#else
				print_debug_hex8(byte);
				print_debug_char(' ');
#endif
			}
			print_debug("\n");
		}
	}
}
void dump_smbus_registers(void)
{
	unsigned device;
        print_debug("\n");
        for(device = 1; device < 0x80; device++) {
                int j;
		if( spd_read_byte(device, 0) < 0 ) continue;
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, "smbus: %02x", device);
#else
                print_debug("smbus: ");
                print_debug_hex8(device);
#endif
                for(j = 0; j < 256; j++) {
                	int status;
                        unsigned char byte;
                        status = spd_read_byte(device, j);
                        if (status < 0) {
				break;
                        }
                        if ((j & 0xf) == 0) {
#if !defined(__ROMCC__)
				printk(BIOS_DEBUG, "\n%02x: ",j);
#else
                	        print_debug("\n");
                                print_debug_hex8(j);
                                print_debug(": ");
#endif
                        }
                        byte = status & 0xff;
#if !defined(__ROMCC__)
                        printk(BIOS_DEBUG, "%02x ", byte);
#else
                        print_debug_hex8(byte);
                        print_debug_char(' ');
#endif
                }
                print_debug("\n");
	}
}

void dump_io_resources(unsigned port)
{

	int i;
#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "%04x:\n", port);
#else
        print_debug_hex16(port);
        print_debug(":\n");
#endif
        for(i=0;i<256;i++) {
                uint8_t val;
                if ((i & 0x0f) == 0) {
#if !defined(__ROMCC__)
			printk(BIOS_DEBUG, "%02x:", i);
#else
                        print_debug_hex8(i);
                        print_debug_char(':');
#endif
                }
                val = inb(port);
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, " %02x",val);
#else
                print_debug_char(' ');
                print_debug_hex8(val);
#endif
                if ((i & 0x0f) == 0x0f) {
                        print_debug("\n");
                }
		port++;
        }
}

void dump_mem(unsigned start, unsigned end)
{
        unsigned i;
	print_debug("dump_mem:");
        for(i=start;i<end;i++) {
		if((i & 0xf)==0) {
#if !defined(__ROMCC__)
			printk(BIOS_DEBUG, "\n%08x:", i);
#else
			print_debug("\n");
			print_debug_hex32(i);
			print_debug(":");
#endif
		}
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, " %02x", (unsigned char)*((unsigned char *)i));
#else
		print_debug(" ");
             	print_debug_hex8((unsigned char)*((unsigned char *)i));
#endif
        }
        print_debug("\n");
}
