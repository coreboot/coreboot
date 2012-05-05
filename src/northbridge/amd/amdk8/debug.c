/*
 * generic K8 debug code, used by mainboard specific romstage.c
 *
 */

static inline void print_debug_addr(const char *str, void *val)
{
#if CONFIG_DEBUG_CAR
	printk(BIOS_DEBUG, "------Address debug: %s%p------\n", str, val);
#endif
}

#if 1
static void print_debug_pci_dev(unsigned dev)
{
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%02x", (dev>>20) & 0xff, (dev>>15) & 0x1f, (dev>>12) & 0x7);
}

static inline void print_pci_devices(void)
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
		printk(BIOS_DEBUG, " %04x:%04x\n", (id & 0xffff), (id>>16));
		if(((dev>>12) & 0x07) == 0) {
			uint8_t hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);

	for(i = 0; i < 256; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			printk(BIOS_DEBUG, "\n%02x:",i);
		}
		val = pci_read_config8(dev, i);
		printk(BIOS_DEBUG, " %02x", val);
	}
	print_debug("\n");
}

#if CONFIG_K8_REV_F_SUPPORT
static uint32_t pci_read_config32_index_wait(device_t dev, uint32_t index_reg, uint32_t index);
static inline void dump_pci_device_index_wait(unsigned dev, uint32_t index_reg)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug(" -- index_reg="); print_debug_hex32(index_reg);

	for(i = 0; i < 0x40; i++) {
		uint32_t val;
		int j;
		printk(BIOS_DEBUG, "\n%02x:",i);
		val = pci_read_config32_index_wait(dev, index_reg, i);
		for(j=0;j<4;j++) {
			printk(BIOS_DEBUG, " %02x", val & 0xff);
			val >>= 8;
		}

	}
	print_debug("\n");
}
#endif

static inline void dump_pci_devices(void)
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

		if(((dev>>12) & 0x07) == 0) {
			uint8_t hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

static inline void dump_pci_devices_on_bus(unsigned busn)
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

		if(((dev>>12) & 0x07) == 0) {
			uint8_t hdr_type;
			hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
			if((hdr_type & 0x80) != 0x80) {
				dev += PCI_DEV(0,0,7);
			}
		}
	}
}

#if CONFIG_DEBUG_SMBUS

static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					printk(BIOS_DEBUG, "\n%02x: ", j);
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			print_debug("\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.1: %02x", i, device);
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					printk(BIOS_DEBUG, "\n%02x: ", j);
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			print_debug("\n");
		}
	}
}
static void dump_smbus_registers(void)
{
	unsigned device;
	print_debug("\n");
	for(device = 1; device < 0x80; device++) {
		int j;
		if( smbus_read_byte(device, 0) < 0 ) continue;
		printk(BIOS_DEBUG, "smbus: %02x", device);
		for(j = 0; j < 256; j++) {
			int status;
			unsigned char byte;
			status = smbus_read_byte(device, j);
			if (status < 0) {
				break;
			}
			if ((j & 0xf) == 0) {
				printk(BIOS_DEBUG, "\n%02x: ",j);
			}
			byte = status & 0xff;
			printk(BIOS_DEBUG, "%02x ", byte);
		}
		print_debug("\n");
	}
}
#endif

static inline void dump_io_resources(unsigned port)
{

	int i;
	udelay(2000);
	printk(BIOS_DEBUG, "%04x:\n", port);
	for(i=0;i<256;i++) {
		uint8_t val;
		if ((i & 0x0f) == 0) {
			printk(BIOS_DEBUG, "%02x:", i);
		}
		val = inb(port);
		printk(BIOS_DEBUG, " %02x",val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
		port++;
	}
}

static inline void dump_mem(unsigned start, unsigned end)
{
	unsigned i;
	print_debug("dump_mem:");
	for(i=start;i<end;i++) {
		if((i & 0xf)==0) {
			printk(BIOS_DEBUG, "\n%08x:", i);
		}
		printk(BIOS_DEBUG, " %02x", (unsigned char)*((unsigned char *)i));
	}
	print_debug("\n");
 }
#endif
