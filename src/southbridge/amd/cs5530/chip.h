#ifndef _SOUTHBRIDGE_AMD_CS5530
#define _SOUTHBRIDGE_AMD_CS5530

extern struct chip_operations southbridge_amd_cs5530_ops;

struct southbridge_amd_cs5530_config {
	/* PCI function enables so the pci scan bus finds the devices */
	int enable_ide;
	int enable_nvram;
};

#endif	/* _SOUTHBRIDGE_AMD_CS5530 */
