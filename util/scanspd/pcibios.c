#include <stdio.h>

#include <asm/io.h>

typedef unsigned short u16;
typedef unsigned long u32;

#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))

 int pcibios_write_config_byte(unsigned char bus, int devfn, int where, unsigned char value)
{
	outl(CONFIG_CMD(bus,devfn,where&0xfc), 0xCF8);    
	outb(value, (0xCFC + (where&2)));
	return 0;    
}

 int pcibios_write_config_word(unsigned char bus, int devfn, int where, u16 value)
{
	outl(CONFIG_CMD(bus,devfn,where&0xfc), 0xCF8);    
	outw(value, (0xCFC + (where&2)));
	return 0;    
}

 int pcibios_write_config_dword(unsigned char bus, int devfn, int where, u32 value)
{
	outl(CONFIG_CMD(bus,devfn,where&0xfc), 0xCF8);
	outl(value, 0xcfc);
	return 0;    
}

 int pcibios_read_config_byte(unsigned char bus, int devfn, int where, unsigned char *value)
{
	outl(CONFIG_CMD(bus,devfn,(where&0xfc)), 0xCF8);    
	*value = inw(0xCFC + (where&2));
	return 0;    
}

 int pcibios_read_config_word(unsigned char bus, int devfn, int where, u16 *value)
{
	outl(CONFIG_CMD(bus,devfn,where&0xfc), 0xCF8);    
	*value = inw(0xCFC + (where&2));
	return 0;    
}

 int pcibios_read_config_dword(unsigned char bus, int devfn, int where, u32 *value)
{
	u32 retval;
	int i, j;
	outl(CONFIG_CMD(bus,devfn,where&0xfc), 0xCF8);
	retval = inl(0xCFC);
	*value = retval;
	return 0;    
}

