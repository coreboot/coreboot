/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <spd.h>

static void print_reg(unsigned char index)
{
        unsigned char data;

        outb(index, 0x2e);
        data = inb(0x2f);
	printk(BIOS_DEBUG, "0x%02x: 0x%02x\n", index, data);
        return;
}

static inline void xbus_en(void)
{
        /* select the XBUS function in the SIO */
        outb(0x07, 0x2e);
        outb(0x0f, 0x2f);
        outb(0x30, 0x2e);
        outb(0x01, 0x2f);
	return;
}

static void setup_func(unsigned char func)
{
        /* select the function in the SIO */
        outb(0x07, 0x2e);
        outb(func, 0x2f);
        /* print out the regs */
        print_reg(0x30);
        print_reg(0x60);
        print_reg(0x61);
        print_reg(0x62);
        print_reg(0x63);
        print_reg(0x70);
        print_reg(0x71);
        print_reg(0x74);
        print_reg(0x75);
        return;
}

static inline void siodump(void)
{
        int i;
        unsigned char data;

	 printk(BIOS_DEBUG, "\n*** SERVER I/O REGISTERS ***\n");
        for (i=0x10; i<=0x2d; i++) {
                print_reg((unsigned char)i);
        }
#if 0
        printk(BIOS_DEBUG, "\n*** XBUS REGISTERS ***\n");
        setup_func(0x0f);
        for (i=0xf0; i<=0xff; i++) {
                print_reg((unsigned char)i);
        }

        printk(BIOS_DEBUG, "\n***  SERIAL 1 CONFIG REGISTERS ***\n");
        setup_func(0x03);
        print_reg(0xf0);

        printk(BIOS_DEBUG, "\n***  SERIAL 2 CONFIG REGISTERS ***\n");
        setup_func(0x02);
        print_reg(0xf0);

#endif
        printk(BIOS_DEBUG, "\n***  GPIO REGISTERS ***\n");
        setup_func(0x07);
        for (i=0xf0; i<=0xf8; i++) {
                print_reg((unsigned char)i);
        }
        printk(BIOS_DEBUG, "\n***  GPIO VALUES ***\n");
        data = inb(0x68a);
	printk(BIOS_DEBUG, "\nGPDO 4: 0x%02x", data);
        data = inb(0x68b);
	printk(BIOS_DEBUG, "\nGPDI 4: 0x%02x\n", data);

#if 0

        printk(BIOS_DEBUG, "\n***  WATCHDOG TIMER REGISTERS ***\n");
        setup_func(0x0a);
        print_reg(0xf0);

        printk(BIOS_DEBUG, "\n***  FAN CONTROL REGISTERS ***\n");
        setup_func(0x09);
        print_reg(0xf0);
        print_reg(0xf1);

        printk(BIOS_DEBUG, "\n***  RTC REGISTERS ***\n");
        setup_func(0x10);
        print_reg(0xf0);
        print_reg(0xf1);
        print_reg(0xf3);
        print_reg(0xf6);
        print_reg(0xf7);
        print_reg(0xfe);
        print_reg(0xff);

        printk(BIOS_DEBUG, "\n***  HEALTH MONITORING & CONTROL REGISTERS ***\n");
        setup_func(0x14);
        print_reg(0xf0);
#endif
        return;
}

static inline void dump_bar14(unsigned dev)
{
	int i;
	unsigned long bar;

	printk(BIOS_DEBUG, "BAR 14 Dump\n");

	bar = pci_read_config32(dev, 0x14);
	for(i = 0; i <= 0x300; i+=4) {
#if 0
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x:", i);
		val = pci_read_config8(dev, i);
#endif
		if((i%4)==0)
			printk(BIOS_DEBUG, "\n%04x ", i);
		printk(BIOS_DEBUG, "%08x ", read32(bar + i));
	}
	printk(BIOS_DEBUG, "\n");
}

static inline void dump_spd_registers(void)
{
        unsigned device;
        device = DIMM0;
        while(device <= DIMM7) {
                int status = 0;
                int i;
                printk(BIOS_DEBUG, "\ndimm %02x", device);

                for(i = 0; (i < 256) ; i++) {
                        if ((i % 16) == 0)
				printk(BIOS_DEBUG, "\n%02x: ", i);
			status = smbus_read_byte(device, i);
                        if (status < 0) {
			         printk(BIOS_DEBUG, "bad device: %d\n", -status);
			         break;
			}
			printk(BIOS_DEBUG, "%02x ", status);
		}
		device++;
		printk(BIOS_DEBUG, "\n");
	}
}

static inline void dump_ipmi_registers(void)
{
        unsigned device;
        device = 0x42;
        while(device <= 0x42) {
                int status = 0;
                int i;
                printk(BIOS_DEBUG, "\nipmi %02x", device);

                for(i = 0; (i < 8) ; i++) {
			status = smbus_read_byte(device, 2);
                        if (status < 0) {
			         printk(BIOS_DEBUG, "bad device: %d\n", -status);
			         break;
			}
			printk(BIOS_DEBUG, "%02x ", status);
		}
		device++;
		printk(BIOS_DEBUG, "\n");
	}
}
