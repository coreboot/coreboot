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

typedef __builtin_div_t div_t;
typedef __builtin_ldiv_t ldiv_t;
typedef __builtin_udiv_t udiv_t;
typedef __builtin_uldiv_t uldiv_t;

static div_t div(int numer, int denom)
{
	return __builtin_div(numer, denom);
}

static ldiv_t ldiv(long numer, long denom)
{
	return __builtin_ldiv(numer, denom);
}

static udiv_t udiv(unsigned numer, unsigned denom)
{
	return __builtin_udiv(numer, denom);
}

static uldiv_t uldiv(unsigned long numer, unsigned long denom)
{
	return __builtin_uldiv(numer, denom);
}



int log2(int value)
{
	/* __builtin_bsr is a exactly equivalent to the x86 machine
	 * instruction with the exception that it returns -1  
	 * when the value presented to it is zero.
	 * Otherwise __builtin_bsr returns the zero based index of
	 * the highest bit set.
	 */
	return __builtin_bsr(value);
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


struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;

static tsc_t rdtsc(void)
{
	tsc_t res;
	asm ("rdtsc"
		: "=a" (res.lo), "=d"(res.hi) /* outputs */
		: /* inputs */
		: /* Clobbers */
		);
	return res;
}


#define PCI_ADDR(BUS, DEV, FN, WHERE) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN) & 0x07) << 8) | \
	((WHERE) & 0xFF))

#define PCI_DEV(BUS, DEV, FN) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x7) << 8))

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

typedef unsigned device_t;

static unsigned char pci_read_config8(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}

static unsigned short pci_read_config16(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}

static unsigned int pci_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}

static void pci_write_config8(device_t dev, unsigned where, unsigned char value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}

static void pci_write_config16(device_t dev, unsigned where, unsigned short value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outw(value, 0xCFC + (addr & 2));
}

static void pci_write_config32(device_t dev, unsigned where, unsigned int value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}

#define PCI_DEV_INVALID (0xffffffffU)
static device_t pci_locate_device(unsigned pci_id, device_t dev)
{
	for(; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0,0,1)) {
		unsigned int id;
		id = pci_read_config32(dev, 0);
		if (id == pci_id) {
			return dev;
		}
	}
	return PCI_DEV_INVALID;
}

