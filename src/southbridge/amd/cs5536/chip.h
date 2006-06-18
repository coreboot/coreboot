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
};

#endif	/* _SOUTHBRIDGE_AMD_CS5536 */
