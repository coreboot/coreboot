static void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

static void outw(unsigned short value, unsigned short port)
{
	__builtin_outw(value, port);
}

static void outl(unsigned int value, unsigned short port)
{
	__builtin_outl(value, port);
}


static unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}


static unsigned char inw(unsigned short port)
{
	return __builtin_inw(port);
}

static unsigned char inl(unsigned short port)
{
	return __builtin_inl(port);
}

static void hlt(void)
{
	__builtin_hlt();
}

typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
	__builtin_wrmsr(index, msr.lo, msr.hi);
}

#define PCI_ADDR(BUS, DEV, FN, WHERE) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN) & 0x07) << 8) | \
	((WHERE) & 0xFF))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

static unsigned char pci_read_config8(unsigned addr)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

static unsigned short pci_read_config16(unsigned addr)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

static unsigned int pci_read_config32(unsigned addr)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

static void pci_write_config8(unsigned addr, unsigned char value)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

static void pci_write_config16(unsigned addr, unsigned short value)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outw(value, 0xCFC + (addr & 2));
}

static void pci_write_config32(unsigned addr, unsigned int value)
{
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

static unsigned pci_locate_device(unsigned pci_id, unsigned addr)
{
	addr &= ~0xff;
	for(; addr <= PCI_ADDR(255, 31, 7, 0); addr += PCI_ADDR(0,0,1,0)) {
		unsigned int id;
		id = pci_read_config32(addr);
		if (id == pci_id) {
			return addr;
		}
	}
	return ~0U;
}
