
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

static void dump_spd_registers(int controllers, const struct mem_controller *ctrl)
{
	int n;
	for(n = 0; n < controllers; n++) {
		int i;
		print_debug("\n");
		activate_spd_rom(&ctrl[n]);
		for(i = 0; i < 4; i++) {
			unsigned device;
			device = ctrl[n].channel0[i];
			if (device) {
				int j;
				print_debug("dimm: ");
				print_debug_hex8(n);
				print_debug_char('.');
				print_debug_hex8(i);
				print_debug(".0: ");
				print_debug_hex8(device);
				for(j = 0; j < 256; j++) {
					int status;
					unsigned char byte;
					if ((j & 0xf) == 0) {
						print_debug("\n");
						print_debug_hex8(j);
						print_debug(": ");
					}
					status = spd_read_byte(device, j);
					if (status < 0) {
						print_debug("bad device\n");
						break;
					}
#if 0
					byte = status & 0xff;
					print_debug_hex8(byte);
#else
					print_debug_hex8(status & 0xff);
#endif
					print_debug_char(' ');
				}
				print_debug("\n");
			}
			device = ctrl[n].channel1[i];
			if (device) {
				int j;
				print_debug("dimm: ");
				print_debug_hex8(n);
				print_debug_char('.');
				print_debug_hex8(i);
				print_debug(".1: ");
				print_debug_hex8(device);
				for(j = 0; j < 256; j++) {
					int status;
					unsigned char byte;
					if ((j & 0xf) == 0) {
						print_debug("\n");
						print_debug_hex8(j);
						print_debug(": ");
					}
					status = spd_read_byte(device, j);
					if (status < 0) {
						print_debug("bad device\n");
						break;
					}
#if 0
					byte = status & 0xff;
					print_debug_hex8(byte);
#else
					print_debug_hex8(status & 0xff);
#endif
					print_debug_char(' ');
				}
				print_debug("\n");
			}
		}
	}
}
