/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <ppc_asm.tmpl>


.globl  bsp_init_northbridge

bsp_init_northbridge:
        mflr    r11
        lis     r10, 0x8000
        
        /* PCI Cmd */
        li      r4, 6
        ori     r3, r10, 4
        bl      __pci_config_write_16
        
        /* PCI Stat */
        ori     r3, r10, 6
        bl      __pci_config_read_16
        ori     r4, r4, 0xffff
        ori     r3, r10, 6
        bl      __pci_config_write_16
        
        /* CLK Drive */
        ori     r4, r10, 0xfc01 /* Top bit will be ignored */
        ori     r3, r10, 0x74
        bl      __pci_config_write_16

        /* EUMBBAR */
        lis     r4, 0xfc00
        ori     r3, r10, 0x78
        bl      __pci_config_write_32

        /* Page CTR */
        li      r4, 0x32
        ori     r3, r10, 0xa3
        lis     r7, 1
        mtctr   r7
        bl      __pci_config_write_8
	
        /* PICR1 */
        lis     r4, 0xff04
        ori     r4, r4, 0x1a18
        ori     r3, r10, 0xa8
        bl      __pci_config_write_32

        /* PICR2 */
        lis     r4, 0x0404
        ori     r4, r4, 0x0004
        ori     r3, r10, 0xac
        bl      __pci_config_write_32
	
	/* Preserve memgo bit */
        /* MCCR1 */
        ori     r3, r10, 0xf0
    	bl  	__pci_config_read_32
	lis 	r7, 0x0008
	and 	r4, r7, r3
        oris    r4, r4, 0x75e0
        ori     r3, r10, 0xf0
        bl      __pci_config_write_32

        /* MCCR4 */
        ori     r3, r10, 0xfc
    	bl  	__pci_config_read_32
	lis 	r7, 0x0050
	and 	r4, r7, r3
        oris    r4, r4, 0x3522
        ori     r3, r10, 0xfc
        bl      __pci_config_write_32

        /* MCCR2 */
        ori     r3, r10, 0xf4
    	bl  	__pci_config_read_32
	lis 	r7, 0x000f
	ori 	r7, r7, 0x0001
	and 	r4, r3, r7
        oris    r4, r4, 0x0440
        ori     r3, r10, 0xf4
        bl      __pci_config_write_32
        mtlr    r11
        blr
        
