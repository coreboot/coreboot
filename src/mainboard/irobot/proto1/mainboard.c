#include <printk.h>
#include <pci.h>

#include <cpu/p5/io.h>

/* #include "SMC_SuperIO.h" */
#ifndef _SMC_SUPER_IO_H_
#define _SMC_SUPER_IO_H_

#define SMC_PP_MODE_SPP     0x00
#define SMC_PP_MODE_EPP_SPP 0x01
#define SMC_PP_MODE_ECP     0x02
#define SMC_PP_MODE_EPP_ECP 0x03

int smc_uart_setup(int smc_addr,
                   int addr1, int irq1,
                   int addr2, int irq2);

int smc_pp_setup(int smc_addr, int pp_addr, int mode);

int smc_validbit(int smc_addr, int valid);

#endif /* _SMC_SUPER_IO_H_ */

void mainboard_fixup() {
  int rv;
  struct pci_dev *pcidev;
  unsigned long val;

  nvram_on();
  
  intel_display_cpuid();
  intel_mtrr_check();
  
  intel_zero_irq_settings();
  intel_check_irq_routing_table();
  intel_interrupts_on();
  
  rv = 0;

  /*
   * Select EPP to enable IRQ sharing between chips
   */
#if 0  /* does not seem to work */
  rv |= smc_pp_setup(0x370, 0x378, SMC_PP_MODE_EPP_SPP);
  rv |= smc_pp_setup(0x3f0, 0x380, SMC_PP_MODE_EPP_SPP);
#endif
  /*
   * Enable and config all the serial ports
   */

  rv |= smc_uart_setup(0x370, 0x3f8, 4, 0x2f8, 4);
  rv |= smc_uart_setup(0x3f0, 0x3e8, 3, 0x2e8, 3);

  /*
   * Set the valid bit to enable the devices
   */

  rv |= smc_validbit(0x370, 1);
  rv |= smc_validbit(0x3f0, 1);

  /* return(rv); */

  /* ti_pci1225_setup(); */

  // Enable GPI10, disable LID
  
  pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);
  if (pcidev) {
    pci_read_config_dword(pcidev, 0xb0, &val);
    pci_write_config_dword(pcidev, 0xb0, val | 0x02000000);
  }
  
  // Map power stuff to I/O 0x4000
  
  pcidev = pci_find_device(0x8086, 0x7113, (void *)NULL);
  if (pcidev) {
    pci_write_config_dword(pcidev, 0x40, 0x00004001);
    pci_write_config_byte(pcidev, 0x80, 0x01);
  }
  
  return;
}

/* 1Meg - 64K = 960K = 0xf0000 */

#define INITRD_SIZE 0xf0000

void
loader_setup(unsigned long base,
	     unsigned long totalram,
	     unsigned long *initrd_start,
	     unsigned long *initrd_size,
	     unsigned char **cmd_line,
	     unsigned char **zkernel_start,
	     unsigned long *zkernel_mask)
{
  unsigned char button;
  unsigned char gpo0;

  button = !(inb(0x4031) & 0x04);

  printk_info( "LID button is - %s\n", (button ? "closed" : "open"));

  if (button) {
    *cmd_line = "root=/dev/ram0 console=ttyS0,115200";
    *initrd_start = (totalram*1024 - INITRD_SIZE - 0x100000 -1) & 0xffff0000;
    *initrd_size  = INITRD_SIZE;
    *zkernel_start = (unsigned char *)0xfff00000;
    *zkernel_mask  = 0x0000ffff;

    /* here we load the initrd image */

    /* flip to other flash page */

    gpo0 = inb(0x4034);
    printk_info( "GPO[0] : 0x%02x\n", gpo0);
    gpo0 &= ~0x01;
    outb(gpo0, 0x4034);

    {
      int ii;

      for (ii = 0; ii<16; ii++) {
	printk_info( "[%02x] ", *(unsigned char *)(0xfff00000+ii));
      }
      
      printk_info("\n");
    }

    memcpy((void *)(*initrd_start), (void *)(0xfff00000), *initrd_size);

    /* flip back to default flash page */

    gpo0 |= 0x01;
    outb(gpo0, 0x4034);
  }
}

