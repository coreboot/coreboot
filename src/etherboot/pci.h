#ifndef	PCI_H
#define PCI_H

/*
** Support for NE2000 PCI clones added David Monro June 1997
** Generalised for other PCI NICs by Ken Yap July 1997
**
** Most of this is taken from:
**
** /usr/src/linux/drivers/pci/pci.c
** /usr/src/linux/include/linux/pci.h
** /usr/src/linux/arch/i386/bios32.c
** /usr/src/linux/include/linux/bios32.h
** /usr/src/linux/drivers/net/ne.c
*/

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#define PCI_COMMAND_IO			0x1	/* Enable response in I/O space */
#define PCI_COMMAND_MASTER		0x4	/* Enable bus mastering */
#define PCI_LATENCY_TIMER		0x0d	/* 8 bits */

#define PCIBIOS_PCI_FUNCTION_ID         0xb1XX
#define PCIBIOS_PCI_BIOS_PRESENT        0xb101
#define PCIBIOS_FIND_PCI_DEVICE         0xb102
#define PCIBIOS_FIND_PCI_CLASS_CODE     0xb103
#define PCIBIOS_GENERATE_SPECIAL_CYCLE  0xb106
#define PCIBIOS_READ_CONFIG_BYTE        0xb108
#define PCIBIOS_READ_CONFIG_WORD        0xb109
#define PCIBIOS_READ_CONFIG_DWORD       0xb10a
#define PCIBIOS_WRITE_CONFIG_BYTE       0xb10b
#define PCIBIOS_WRITE_CONFIG_WORD       0xb10c
#define PCIBIOS_WRITE_CONFIG_DWORD      0xb10d

#define PCI_VENDOR_ID           0x00    /* 16 bits */
#define PCI_DEVICE_ID           0x02    /* 16 bits */
#define PCI_COMMAND             0x04    /* 16 bits */

#define PCI_REVISION            0x08    /* 8 bits  */
#define PCI_CLASS_CODE          0x0b    /* 8 bits */
#define PCI_SUBCLASS_CODE       0x0a    /* 8 bits */
#define PCI_HEADER_TYPE         0x0e    /* 8 bits */

#define PCI_BASE_ADDRESS_0      0x10    /* 32 bits */
#define PCI_BASE_ADDRESS_1      0x14    /* 32 bits */
#define PCI_BASE_ADDRESS_2      0x18    /* 32 bits */
#define PCI_BASE_ADDRESS_3      0x1c    /* 32 bits */
#define PCI_BASE_ADDRESS_4      0x20    /* 32 bits */
#define PCI_BASE_ADDRESS_5      0x24    /* 32 bits */

#ifndef	PCI_BASE_ADDRESS_IO_MASK
#define	PCI_BASE_ADDRESS_IO_MASK       (~0x03)
#endif
#define	PCI_BASE_ADDRESS_SPACE_IO	0x01
#define	PCI_ROM_ADDRESS		0x30	/* 32 bits */
#define	PCI_ROM_ADDRESS_ENABLE	0x01	/* Write 1 to enable ROM,
					   bits 31..11 are address,
					   10..2 are reserved */

#define PCI_FUNC(devfn)           ((devfn) & 0x07)

#define BIOS32_SIGNATURE        (('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))

/* PCI signature: "PCI " */
#define PCI_SIGNATURE           (('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))

/* PCI service signature: "$PCI" */
#define PCI_SERVICE             (('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))

union bios32 {
	struct {
		unsigned long signature;	/* _32_ */
		unsigned long entry;		/* 32 bit physical address */
		unsigned char revision;		/* Revision level, 0 */
		unsigned char length;		/* Length in paragraphs should be 01 */
		unsigned char checksum;		/* All bytes must add up to zero */
		unsigned char reserved[5];	/* Must be zero */
	} fields;
	char chars[16];
};

#define KERN_CODE_SEG	0x8	/* This _MUST_ match start.S */

/* Stuff for asm */
#define save_flags(x) \
__asm__ __volatile__("pushfl ; popl %0":"=g" (x): /* no input */ :"memory")

#define cli() __asm__ __volatile__ ("cli": : :"memory")

#define restore_flags(x) \
__asm__ __volatile__("pushl %0 ; popfl": /* no output */ :"g" (x):"memory")

#define PCI_VENDOR_ID_ADMTEK            0x1317
#define PCI_DEVICE_ID_ADMTEK_0985       0x0985
#define PCI_VENDOR_ID_REALTEK           0x10ec
#define PCI_DEVICE_ID_REALTEK_8029      0x8029
#define PCI_DEVICE_ID_REALTEK_8139      0x8139
#define PCI_VENDOR_ID_WINBOND2          0x1050
#define PCI_DEVICE_ID_WINBOND2_89C940   0x0940
#define PCI_DEVICE_ID_WINBOND2_89C840   0x0840
#define PCI_VENDOR_ID_COMPEX            0x11f6
#define PCI_DEVICE_ID_COMPEX_RL2000     0x1401
#define PCI_DEVICE_ID_COMPEX_RL100ATX   0x2011
#define PCI_VENDOR_ID_KTI               0x8e2e
#define PCI_DEVICE_ID_KTI_ET32P2        0x3000
#define PCI_VENDOR_ID_NETVIN            0x4a14
#define PCI_DEVICE_ID_NETVIN_NV5000SC   0x5000
#define PCI_VENDOR_ID_3COM		0x10b7
#define PCI_DEVICE_ID_3COM_3C590	0x5900
#define PCI_DEVICE_ID_3COM_3C595	0x5950
#define PCI_DEVICE_ID_3COM_3C595_1	0x5951
#define PCI_DEVICE_ID_3COM_3C595_2	0x5952
#define PCI_DEVICE_ID_3COM_3C900TPO	0x9000
#define PCI_DEVICE_ID_3COM_3C900COMBO	0x9001
#define PCI_DEVICE_ID_3COM_3C905TX	0x9050
#define PCI_DEVICE_ID_3COM_3C905T4	0x9051
#define PCI_DEVICE_ID_3COM_3C905B_TX	0x9055
#define PCI_DEVICE_ID_3COM_3C905C_TXM	0x9200
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_DEVICE_ID_INTEL_82557	0x1229
#define PCI_DEVICE_ID_INTEL_82559ER	0x1209
#define PCI_DEVICE_ID_INTEL_ID1029	0x1029
#define PCI_DEVICE_ID_INTEL_ID1030	0x1030
#define PCI_DEVICE_ID_INTEL_82562	0x2449
#define PCI_VENDOR_ID_AMD		0x1022
#define PCI_DEVICE_ID_AMD_LANCE		0x2000
#define PCI_VENDOR_ID_AMD_HOMEPNA	0x1022
#define PCI_DEVICE_ID_AMD_HOMEPNA	0x2001
#define PCI_VENDOR_ID_SMC_1211          0x1113
#define PCI_DEVICE_ID_SMC_1211          0x1211
#define PCI_VENDOR_ID_DEC		0x1011
#define PCI_DEVICE_ID_DEC_TULIP		0x0002
#define PCI_DEVICE_ID_DEC_TULIP_FAST	0x0009
#define PCI_DEVICE_ID_DEC_TULIP_PLUS	0x0014
#define PCI_DEVICE_ID_DEC_21142		0x0019
#define PCI_VENDOR_ID_SMC		0x10B8
#ifndef	PCI_DEVICE_ID_SMC_EPIC100
# define PCI_DEVICE_ID_SMC_EPIC100	0x0005
#endif
#define PCI_VENDOR_ID_MACRONIX		0x10d9
#define PCI_DEVICE_ID_MX987x5		0x0531
#define PCI_VENDOR_ID_LINKSYS		0x11AD
#define PCI_DEVICE_ID_LC82C115		0xC115
#define PCI_VENDOR_ID_VIATEC		0x1106
#define PCI_DEVICE_ID_VIA_RHINE_I	0x3043
#define PCI_DEVICE_ID_VIA_VT6102	0x3065
#define PCI_DEVICE_ID_VIA_86C100A	0x6100
#define PCI_VENDOR_ID_DAVICOM		0x1282
#define	PCI_DEVICE_ID_DM9009		0x9009
#define PCI_DEVICE_ID_DM9102		0x9102
#define PCI_VENDOR_ID_SIS         	0x1039
#define PCI_DEVICE_ID_SIS900     	0x0900   
#define PCI_DEVICE_ID_SIS7016    	0x7016  
#define	PCI_VENDOR_ID_DLINK		0x1186
#define	PCI_DEVICE_ID_DFE530TXP		0x1300

struct pci_device {
	unsigned short	vendor, dev_id;
	const char	*name;
	unsigned int	membase;
	unsigned short	ioaddr;
	unsigned char	devfn;
	unsigned char	bus;
};

extern void	eth_pci_init(struct pci_device *);

extern int pcibios_read_config_byte(unsigned int bus, unsigned int device_fn, unsigned int where, unsigned char *value);
extern int pcibios_write_config_byte (unsigned int bus, unsigned int device_fn, unsigned int where, unsigned char value);
extern int pcibios_read_config_word(unsigned int bus, unsigned int device_fn, unsigned int where, unsigned short *value);
extern int pcibios_write_config_word (unsigned int bus, unsigned int device_fn, unsigned int where, unsigned short value);
extern int pcibios_write_config_dword(unsigned int bus, unsigned int device_fn, unsigned int where, unsigned int value);
#endif	/* PCI_H */
