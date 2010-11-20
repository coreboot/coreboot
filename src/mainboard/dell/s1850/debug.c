#include <spd.h>

static void print_reg(unsigned char index)
{
        unsigned char data;

        outb(index, 0x2e);
        data = inb(0x2f);
	print_debug("0x");
	print_debug_hex8(index);
	print_debug(": 0x");
	print_debug_hex8(data);
	print_debug("\n");
        return;
}

static void xbus_en(void)
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

static void siodump(void)
{
        int i;
        unsigned char data;

	 print_debug("\n*** SERVER I/O REGISTERS ***\n");
        for (i=0x10; i<=0x2d; i++) {
                print_reg((unsigned char)i);
        }
#if 0
        print_debug("\n*** XBUS REGISTERS ***\n");
        setup_func(0x0f);
        for (i=0xf0; i<=0xff; i++) {
                print_reg((unsigned char)i);
        }

        print_debug("\n***  SERIAL 1 CONFIG REGISTERS ***\n");
        setup_func(0x03);
        print_reg(0xf0);

        print_debug("\n***  SERIAL 2 CONFIG REGISTERS ***\n");
        setup_func(0x02);
        print_reg(0xf0);

#endif
        print_debug("\n***  GPIO REGISTERS ***\n");
        setup_func(0x07);
        for (i=0xf0; i<=0xf8; i++) {
                print_reg((unsigned char)i);
        }
        print_debug("\n***  GPIO VALUES ***\n");
        data = inb(0x68a);
	print_debug("\nGPDO 4: 0x");
	print_debug_hex8(data);
        data = inb(0x68b);
	print_debug("\nGPDI 4: 0x");
	print_debug_hex8(data);
	print_debug("\n");

#if 0

        print_debug("\n***  WATCHDOG TIMER REGISTERS ***\n");
        setup_func(0x0a);
        print_reg(0xf0);

        print_debug("\n***  FAN CONTROL REGISTERS ***\n");
        setup_func(0x09);
        print_reg(0xf0);
        print_reg(0xf1);

        print_debug("\n***  RTC REGISTERS ***\n");
        setup_func(0x10);
        print_reg(0xf0);
        print_reg(0xf1);
        print_reg(0xf3);
        print_reg(0xf6);
        print_reg(0xf7);
        print_reg(0xfe);
        print_reg(0xff);

        print_debug("\n***  HEALTH MONITORING & CONTROL REGISTERS ***\n");
        setup_func(0x14);
        print_reg(0xf0);
#endif
        return;
}

static void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 16) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 11) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 8) & 7);
}

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
		print_debug_pci_dev(dev);
		print_debug("\n");
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug("\n");

	for(i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void dump_bar14(unsigned dev)
{
	int i;
	unsigned long bar;

	print_debug("BAR 14 Dump\n");

	bar = pci_read_config32(dev, 0x14);
	for(i = 0; i <= 0x300; i+=4) {
#if 0
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
#endif
		if((i%4)==0) {
		print_debug("\n");
		print_debug_hex16(i);
		print_debug_char(' ');
		}
		print_debug_hex32(read32(bar + i));
		print_debug_char(' ');
	}
	print_debug("\n");
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
        device = DIMM0;
        while(device <= DIMM7) {
                int status = 0;
                int i;
        	print_debug("\n");
                print_debug("dimm ");
		print_debug_hex8(device);

                for(i = 0; (i < 256) ; i++) {
	                unsigned char byte;
                        if ((i % 16) == 0) {
				print_debug("\n");
				print_debug_hex8(i);
				print_debug(": ");
                        }
			status = smbus_read_byte(device, i);
                        if (status < 0) {
			         print_debug("bad device: ");
				 print_debug_hex8(-status);
				 print_debug("\n");
			         break;
			}
			print_debug_hex8(status);
			print_debug_char(' ');
		}
		device++;
		print_debug("\n");
	}
}

void show_dram_slots(void)
{
        unsigned device;
        device = DIMM0;
        while(device <= DIMM7) {
		 int status = 0;
		int i;
		print_debug("\n");
		print_debug("dimm ");
		print_debug_hex8(device);

		status = smbus_read_byte(device, 0);
		if (status < 0) {
			print_debug("bad device: ");
		} else {
			print_debug("present: ");
		}
		print_debug_hex8(status);
		print_debug("\n");
		device++;
		print_debug("\n");
	}
}

void dump_ipmi_registers(void)
{
        unsigned device;
        device = 0x42;
        while(device <= 0x42) {
                int status = 0;
                int i;
        	print_debug("\n");
                print_debug("ipmi ");
		print_debug_hex8(device);

                for(i = 0; (i < 8) ; i++) {
	                unsigned char byte;
			status = smbus_read_byte(device, 2);
                        if (status < 0) {
			         print_debug("bad device: ");
				 print_debug_hex8(-status);
				 print_debug("\n");
			         break;
			}
			print_debug_hex8(status);
			print_debug_char(' ');
		}
		device++;
		print_debug("\n");
	}
}
