/* 

   Intel 830 sizing.
*/ 

#include <mem.h>
#include <part/sizeram.h>
#include <pci.h>
#include <printk.h>

struct mem_range *sizeram(void)
{
  static struct mem_range mem[3];
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


  mem[0].basek = 0;
  mem[0].sizek = 640;
  mem[1].basek = 1024;
  mem[1].sizek = totalmem*1024 - mem[1].basek;
  mem[2].basek = 0;
  mem[2].sizek = 0;
  return &mem;
}








