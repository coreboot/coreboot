#ifndef _SOUTHBRIDGE_AMD_CS5535
#define _SOUTHBRIDGE_AMD_CS5535

extern struct chip_operations southbridge_amd_cs5535_ops;

struct southbridge_amd_cs5535_config {
	/* PCI function enables so the pci scan bus finds the devices */
	int enable_ide;
	int enable_nvram;
};

#endif	/* _SOUTHBRIDGE_AMD_CS5535 */
