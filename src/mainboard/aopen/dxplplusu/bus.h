#ifndef XE7501DEVKIT_BUS_H_INCLUDED
#define XE7501DEVKIT_BUS_H_INCLUDED

// These were determined by seeing how coreboot enumerates the various
// PCI (and PCI-like) buses on the board.

#define PCI_BUS_ROOT		0
#define PCI_BUS_AGP		1		// AGP
#define PCI_BUS_E7501_HI_B	2		// P64H2#1
#define PCI_BUS_P64H2_B		3		// P64H2#1 bus B
#define PCI_BUS_P64H2_A		4		// P64H2#1 bus A
#define PCI_BUS_ICH4		5		// ICH4

#define SUPERIO_BUS		8		// (arbitrary but unique bus #)

#endif	// XE7501DEVKIT_BUS_H_INCLUDED
