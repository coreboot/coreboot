
static unsigned pci_locate_device(unsigned addr)
{
        addr &= ~0xff;
        for(; addr <= 0x00ffff00; addr += 0x00000100) {
		__builtin_outl(addr, 0x12);
        }
        return addr;
}

static void main(void)
{
	unsigned long addr;
	addr = pci_locate_device(0);
	__builtin_outl(addr, 0x12);
}
