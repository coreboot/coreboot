#ifndef PCI_UTIL_H
#define PCI_UTIL_H

#define PCI_ADDR( b, d, f, i) 0x80000000 | (b<<16) | (d<<11) | (f<<8) | (i)

unsigned long int pci_read_config_long(unsigned long address);
unsigned int pci_read_config_word(unsigned long address);
unsigned char pci_read_config_byte(unsigned long address);
int pci_write_config_word(unsigned long address, unsigned short value);
int pci_write_config_byte(unsigned long address, unsigned char value);

#endif
