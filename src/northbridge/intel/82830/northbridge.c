/* 

   Intel 830 sizing.
*/ 

#include <pci.h>
#include <printk.h>

unsigned long sizeram()
{
  unsigned long totalmem;
  unsigned char regval;

  struct pci_dev *pcidev;
  int i;

  totalmem = 0; 

  pcidev = pci_find_slot(0, PCI_DEVFN(0,0));
  for(i = 0x60; i < 0x64; i++)
    {
      pci_read_config_byte(pcidev, i, &regval);
      totalmem += regval * 32;
    }

  return totalmem * 1024; 
}








