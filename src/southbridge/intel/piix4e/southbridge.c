#include <pci.h>
#include <pc80/keyboard.h>
#include <printk.h>

void
southbridge_fixup()
{
}

void nvram_on()
{
	/*; now set up PIIX4e registers 4e and 4f for nvram access.
	   ; 4e will have value 0xc3, 4f will have value 2
	   ; we are going to PIIX4 function 0; the PIIX4 is device 0x12.
	   ; bit coding is 0x80000000 + 0x9000 (dev 0x12)  + 0x4c (x4 aligned)
	   ; plus 2 or 3 (e or f)
	 */
	/* well, this turns on the 1 MB, but we might as well enable APIC 
	 * access too
	 */

        struct pci_dev *pcidev;

        pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);
        if (pcidev) pci_write_config_word(pcidev, 0x4e, 0x03c3);

}

// Have to talk to Eric Beiderman about this ...
void keyboard_on()
{
}
