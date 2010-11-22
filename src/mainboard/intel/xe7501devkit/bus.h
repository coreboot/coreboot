#ifndef XE7501DEVKIT_BUS_H_INCLUDED
#define XE7501DEVKIT_BUS_H_INCLUDED

// These were determined by seeing how coreboot enumerates the various
// PCI (and PCI-like) buses on the board.

#define PCI_BUS_CHIPSET		0
#define PCI_BUS_E7501_HI_B	1		// P64H2#2
#define PCI_BUS_P64H2_2_B	2		// P64H2#2 bus B
#define PCI_BUS_P64H2_2_A	3		// P64H2#2 bus A
#define PCI_BUS_E7501_HI_D	4		// P64H2#1
#define PCI_BUS_P64H2_1_B	5		// P64H2#1 bus B
#define PCI_BUS_P64H2_1_A	6		// P64H2#1 bus A
#define PCI_BUS_ICH3		7		// ICH3-S

#endif	// XE7501DEVKIT_BUS_H_INCLUDED
