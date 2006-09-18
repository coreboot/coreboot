#ifndef _SOUTHBRIDGE_AMD_CS5536
#define _SOUTHBRIDGE_AMD_CS5536

#define MAX_UNWANTED_VPCI 10	/* increase if needed */

extern struct chip_operations southbridge_amd_cs5536_ops;

struct southbridge_amd_cs5536_config {
	/* interrupt enable for LPC bus */
	int lpc_serirq_enable;	/* how to enable, e.g. 0x80 */
 	int lpc_irq;		/* what to enable, e.g. 0x18 */
	int enable_ide_nand_flash; /* if you are using nand flash instead of IDE drive */

	/* following are IRQ numbers for various southbridge resources.
	 * these are configured and PCI headers are set */
	
	int isa_irq;		// f.0, 1022:2090
	int flash_irq;		// f.1, 1022:2091

	// ide irq is tied to IRQ14, this can only be enabled or disabled
	int enable_ide_irq;		// f.2, 1022:2092

	int audio_irq; 		// f.3, 1022:2093
	
	int usb_irq;	// f.4,5,6,7, 1022:2094
	// only one irq source for all usb devices

	// internal UART IRQs
	int uart0_irq;
	int uart1_irq;

	/* GPIO to IRQ mapping, intended to use for PCI IRQ's.
	 * This only does physical mapping, no PCI headers are configured
	 * PCI configuration is mainboard-specific and should be done in mainboard.c
	 */
	// pci IRQs A-D. Set this to 0 to disable.
	int pci_int[4];
	// and their GPIO pins
	int pci_int_pin[4];


	// Enable KEL keyboard IRQ2
	int enable_kel_keyb_irq;
	// Enable KEL mouse IRQ12
	int enable_kel_mouse_irq;
	// Configure KEL Emulation IRQ (input Y13)
	int kel_emul_irq;

	/* the following allow you to disable unwanted virtualized PCI devices */
	unsigned long unwanted_vpci[MAX_UNWANTED_VPCI];
};

#endif	/* _SOUTHBRIDGE_AMD_CS5536 */
