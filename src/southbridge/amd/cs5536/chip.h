#ifndef _SOUTHBRIDGE_AMD_CS5536
#define _SOUTHBRIDGE_AMD_CS5536

extern struct chip_operations southbridge_amd_cs5536_ops;

struct southbridge_amd_cs5536_config {
	/* interrupt enable for LPC bus */
	int lpc_serirq_enable;	/* how to enable, e.g. 0x80 */
 	int lpc_irq;		/* what to enable, e.g. 0x18 */
 	int enable_gpio0_inta; 	/* almost always will be true */
	int enable_ide_nand_flash; /* if you are using nand flash instead of IDE drive */
	int enable_uarta; 		/* internal uarta interrupt enable */
	/* following are IRQ numbers for various southbridge resources. */
	/* I have guessed at some things, as I still don't have an lspci from anyone */
	int ide_irq;		/* f.2 */
	int audio_irq; 		/* f.3 */
	int usbf4_irq;		/* f.4 */
	int usbf5_irq;		/* f.5 */
	int usbf6_irq;		/* f.6 */
	int usbf7_irq; 		/* f.7 */
};

#endif	/* _SOUTHBRIDGE_AMD_CS5536 */
