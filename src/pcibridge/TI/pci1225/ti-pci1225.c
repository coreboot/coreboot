
/*
 * This code is totally non-portable to any system that doesn't
 * happen to match the one it was written for.  This really needs
 * to be done as a set of utilities that mainboard_fixup() can call
 * as needed and with the right arguements for the configuration
 * of the board.
 */

#include <subr.h>
#include <pci.h>
#include <pci_ids.h>

int ti_pci1225_setup() {

  struct pci_dev *pcidev;
  struct pci_dev *pcidev1;
  struct pci_dev *pcidev2;

  int slot;
  int func;
  int bus;
  
  /*
   * setup serial interrupt in the PIIX4.
   *
   * This code has no business being here.  There should be
   * utility functions in southbridge.c that mainboard_fixup()
   * calls to do this before trying to enable pci1225 chip.
   */

  pcidev = pci_find_device(PCI_VENDOR_INTEL,
			   PCI_DEVICE_ID_INTEL_82371AB_0,
			   (void *)NULL);

  if (!pcidev) return(-1);

  pci_write_config_byte(pcidev, 0x64, 0xd0);
  pci_write_config_byte(pcidev, 0xb2, 0x01);


  /*
   * Setup the TI PCI1225 registers to match the PnR
   *
   * At least this code belongs in this file.  However
   * it needs to be provided as utility functions that 
   * mainboard_fixup() can use to fixup the main board.
   */

  pcidev = pci_find_device(PCI_VENDOR_TI,
			   PCI_DEVICE_TI_1225,
			   (void *)NULL);

  if (!pcidev) return(-2);

  pcidev2 = pci_find_device(PCI_VENDOR_TI,
			   PCI_DEVICE_TI_1225,
			   pcidev);

  if (!pcidev2) return(-3);

  if ((pcidev2->vendor != pcidev->vendor) ||
      (pcidev2->device != pcidev->device) ||
      (pcidev2->class  != pcidev->class )) return (-4);

  /* Are we guarenteed to get chip functions in order? */

  if (pcidev->devfn < pcidev2->devfn) {
    pcidev1 = pcidev;
  }
  else {
    pcidev1 = pcidev2;
    pcidev2 = pcidev;
  }

  pci_write_config_byte (pcidev1, 0x3c,       0xff);
  pci_write_config_byte (pcidev2, 0x3c,       0xff);

  pci_write_config_dword(pcidev1, 0x80, 0x2844b060);
  pci_write_config_dword(pcidev1, 0x8c, 0x00001002);
  pci_write_config_byte (pcidev1, 0x90,       0xc0);
  pci_write_config_byte (pcidev1, 0x91,       0x02);
  pci_write_config_byte (pcidev1, 0x92,       0x64);

  return(0);
}
