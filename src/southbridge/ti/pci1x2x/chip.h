#ifndef SOUTHBRIDGE_TI_PCI1X2X_H
#define SOUTHBRIDGE_TI_PCI1X2X_H

extern struct chip_operations southbridge_ti_pci1x2x_ops;

struct southbridge_ti_pci1x2x_config {
	int scr;
	int mrr;
	int clsr;
	int cltr;
	int bcr;
};
#endif
