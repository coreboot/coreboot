#include <asm/io.h>


unsigned long int pci_read_config_long(unsigned long address)
{
	unsigned short res;

	unsigned port = 0xcfc + (address & 0x03);
	address &= ~0x03;

	outl(address | 0x80000000 , 0xcf8);
	res = inw(port);

	return(res);
}
	
unsigned int pci_read_config_word(unsigned long address)
{
	unsigned short res;

	unsigned port = 0xcfc + (address & 0x03);
	address &= ~0x03;

	outl(address | 0x80000000 , 0xcf8);
	res = inw(port);

	return(res);
}
	
unsigned char pci_read_config_byte(unsigned long address)
{
	unsigned char res;

	unsigned port = 0xcfc + (address & 0x03);
	address &= ~0x03;

	outl(address | 0x80000000 , 0xcf8);
	res = inb(port);

	return(res);
}
	
int pci_write_config_word(unsigned long address, unsigned short value)
{
	unsigned port = 0xcfc + (address & 0x03);

	address &= ~0x03;

	outl(address | 0x80000000 , 0xcf8);
	outw(value, port);
}

int pci_write_config_byte(unsigned long address, unsigned char value)
{
	unsigned port = 0xcfc + (address & 0x03);

	address &= ~0x03;

	outl(address | 0x80000000 , 0xcf8);
	outb(value, port);
}

