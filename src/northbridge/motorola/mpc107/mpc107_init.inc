/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

.globl  bsp_init_northbridge

#define ASM
#include "mpc107.h"

bsp_init_northbridge:
        mflr    r11
        lis     r10, BMC_BASE

        /* 
	 * PCI Cmd 
	 */
        li      r4, 0x0006
        ori     r3, r10, 0x04
        bl      __pci_config_write_16
        
        /*
	 * PCI Stat 
	 */
        ori     r3, r10, 0x06
        bl      __pci_config_read_16
        ori     r4, r4, 0xffff
        ori     r3, r10, 0x06
        bl      __pci_config_write_16
	
        /* 
	 * PICR1 
	 */
        ori     r3, r10, 0xa8
        bl      __pci_config_read_32

	/*
	 * Preserve RCS0/Addr Map bits
	 */
        lis     r0, 0x0011
	and	r4, r4, r0

        lis     r0, 0xff00
//      oris    r0, r0, 0x0040	/* burst read wait states = 1 */
        oris    r0, r0, 0x0004	/* processor type = 603/750 */

//      ori	r0, r0, 0x2000	/* enable LocalBusSlave */
        ori	r0, r0, 0x1000	/* enable Flash write */
        ori	r0, r0, 0x0800	/* enable MCP* assertion */
//      ori	r0, r0, 0x0400	/* enable TEA* assertion */
        ori	r0, r0, 0x0200	/* enable data bus parking */
//      ori	r0, r0, 0x0040	/* enable PCI store gathering */
        ori	r0, r0, 0x0010	/* enable loop-snoop */
        ori	r0, r0, 0x0008	/* enable address bus parking */
//      ori	r0, r0, 0x0004	/* enable speculative PCI reads */

        or      r4, r4, r0
        ori     r3, r10, 0xa8
        bl      __pci_config_write_32

        /* 
	 * PICR2 
	 */
        lis     r4, 0x0000
//      oris    r4, r4, 0x2000	/* no Serialize Config cycles */
//      oris    r4, r4, 0x0800	/* No PCI Snoop cycles */
        oris    r4, r4, 0x0400	/* FF0 is Local ROM */
//      oris    r4, r4, 0x0200	/* Flash write lockout */

//      oris    r4, r4, 0x0000	/* snoop wt states = 0 */
        oris    r4, r4, 0x0004	/* snoop wt states = 1 */
//      oris    r4, r4, 0x0008	/* snoop wt states = 2 */
//      oris    r4, r4, 0x000c	/* snoop wt states = 3 */

//      ori     r4, r4, 0x0000	/* addr phase wt states = 0 */
        ori     r4, r4, 0x0004	/* addr phase wt states = 1 */
//      ori     r4, r4, 0x0008	/* addr phase wt states = 2 */
//      ori     r4, r4, 0x000c	/* addr phase wt states = 3 */

        ori     r3, r10, 0xac
        bl      __pci_config_write_32

        /*
	 * EUMBBAR 
	 */
        lis     r4, 0xfc00
        ori     r3, r10, 0x78
        bl      __pci_config_write_32
	
        /*
	 * MCCR1 - Set MEMGO bit later!
	 */
        ori     r3, r10, 0xf0
        bl      __pci_config_read_32

	lis 	r7, 0x0008	/* Preserve MEMGO bit in case we're in RAM */
	and 	r4, r7, r3
//	lis 	r4, 0x0000	/* Rom speed @100MHz (10ns) */
        oris    r4, r4, 0x7580	/* Safe local ROM = 10+3 clocks */
//      oris    r4, r4, 0x7380	/* Fast local ROM = 7+3 clocks */

//      oris    r4, r4, 0x0010	/* Burst ROM/Flash enable */
//      oris    r4, r4, 0x0004	/* Self-refresh enable */
//      oris    r4, r4, 0x0002	/* EDO/FP enable (else SDRAM) */
//      oris    r4, r4, 0x0001	/* Parity check */

//      ori     r4, r4, 0xFFFF	/* 16Mbit/2 bank SDRAM */
//      ori     r4, r4, 0x5555	/* 64Mbit/2 bank SDRAM */
        ori     r4, r4, 0x0000	/* 64Mbit/4 bank SDRAM */

        ori     r3, r10, 0xf0
        bl      __pci_config_write_32

        /* 
	 * MCCR2 
	 */
	lis 	r4, 0x0000
//      oris    r4, r4, 0x4000	/* TS_WAIT_TIMER = 3 clocks */
        oris    r4, r4, 0x0400	/* ASRISE = 2 clocks */
        oris    r4, r4, 0x0040	/* ASFALL = 2 clocks */
//      oris    r4, r4, 0x0010	/* SDRAM parity (else ECC) */
//      oris    r4, r4, 0x0008	/* SDRAM inline writes */
//      oris    r4, r4, 0x0004	/* SDRAM inline reads */
//      oris    r4, r4, 0x0002	/* ECC enable */
//      oris    r4, r4, 0x0001	/* EDO (else FP) */

//      ori     r4, r4, 0x06b8	/* Refresh 33MHz bus */
//      ori     r4, r4, 0x035c	/* Refresh 66MHz bus */
        ori     r4, r4, 0x023c	/* Refresh 100MHz bus */
//      ori     r4, r4, 0x01ac	/* Refresh 133MHz bus */

//      ori     r4, r4, 0x0002	/* Reserve a page */
//      ori     r4, r4, 0x0001	/* RWM parity */

        ori     r3, r10, 0xf4
        bl      __pci_config_write_32

	/*
	 * MCCR3
	 */
	lis 	r4, 0x7000	/* BSTOPRE_M = 7 */
        oris    r4, r4, 0x0800	/* REFREC = 8 clocks */
        oris    r4, r4, 0x0040	/* RDLAT = 4 clocks */
//      oris    r4, r4, 0x0030	/* RDLAT = 3 clocks */

        ori     r3, r10, 0xf8
        bl      __pci_config_write_32

        /* 
	 * MCCR4 
	 */
	lis 	r4, 0x3000	/* PRETOACT = 3 clocks */
        oris    r4, r4, 0x0500	/* ACTOPRE = 5 clocks */
//      oris    r4, r4, 0x0080	/* Enable 8-beat burst (32-bit bus) */
//      oris    r4, r4, 0x0040	/* Enable Inline ECC/Parity */
        oris    r4, r4, 0x0020	/* Enable Extended ROM (RCS2/RCS3) */
        oris    r4, r4, 0x0010	/* Registered buffers */
        oris    r4, r4, 0x0000	/* BSTOPRE_U = 0 */
        oris    r4, r4, 0x0002	/* Change RCS1 to 8-bit mode */

//      ori     r4, r4, 0x8000	/* Registered DIMMs */
        ori     r4, r4, 0x3000	/* CAS Latencey (CL=3) */
//      ori     r4, r4, 0x2000	/* CAS Latencey (CL=2) */
        ori     r4, r4, 0x0200	/* Sequential wrap/4-beat burst */
        ori     r4, r4, 0x0030	/* Reserve a page */
        ori     r4, r4, 0x0009	/* RWM parity */

        ori     r3, r10, 0xfc
        bl      __pci_config_write_32

	/*
	 * MSAR1/MSAR2/MESAR1/MESAR2
	 */
	lis	r4, 0x6040
	ori	r4, r4, 0x2000
        ori     r3, r10, 0x80
        bl      __pci_config_write_32

	lis	r4, 0xe0c0
	ori	r4, r4, 0xa080
        ori     r3, r10, 0x84
        bl      __pci_config_write_32

	lis	r4, 0x0000
	ori	r4, r4, 0x0000
        ori     r3, r10, 0x88
        bl      __pci_config_write_32

	lis	r4, 0x0000
	ori	r4, r4, 0x0000
        ori     r3, r10, 0x8c
        bl      __pci_config_write_32

	/*
	 * MEAR1/MEAR2/MEEAR1/MEEAR2
	 */
	lis	r4, 0x7f5f
	ori	r4, r4, 0x3f1f
        ori     r3, r10, 0x90
        bl      __pci_config_write_32

	lis	r4, 0xffdf
	ori	r4, r4, 0xbf9f
        ori     r3, r10, 0x94
        bl      __pci_config_write_32

	lis	r4, 0x0000
	ori	r4, r4, 0x0000
        ori     r3, r10, 0x98
        bl      __pci_config_write_32

	lis	r4, 0x0000
	ori	r4, r4, 0x0000
        ori     r3, r10, 0x9c
        bl      __pci_config_write_32

	/*
	 * ODCR
	 */
	li	r4, 0
	ori	r4, r4, 0x80	/* PCI I/O 50 ohms */
	ori	r4, r4, 0x40	/* CPU I/O 50 ohms */
//	ori	r4, r4, 0x30	/* Mem I/O 8 ohms */
//	ori	r4, r4, 0x20	/* Mem I/O 13 ohms */
//	ori	r4, r4, 0x10	/* Mem I/O 20 ohms */
	ori	r4, r4, 0x00	/* Mem I/O 40 ohms */
//	ori	r4, r4, 0x0c	/* PCIClk 8 ohms */
//	ori	r4, r4, 0x08	/* PCIClk 13 ohms */
//	ori	r4, r4, 0x04	/* PCIClk 20 ohms */
	ori	r4, r4, 0x00	/* PCIClk 40 ohms */
//	ori	r4, r4, 0x03	/* MemClk 8 ohms */
//	ori	r4, r4, 0x02	/* MemClk 13.3 ohms */
//	ori	r4, r4, 0x01	/* MemClk 20 ohms */
	ori	r4, r4, 0x00	/* MemClk 40 ohms */
        ori     r3, r10, 0x73
        bl      __pci_config_write_8

        /* CDCR */
	li	r4, 0x7fff
	addi	r4, r4, 1	/* PCI_SYNC_OUT disabled */
        ori     r4, r4, 0x7c00	/* PCI_CLK disabled */
//      ori     r4, r4, 0x0300	/* CPU_CLK 8 ohms */
//      ori     r4, r4, 0x0200	/* CPU_CLK 13 ohms */
//      ori     r4, r4, 0x0100	/* CPU_CLK 20 ohms */
//      ori     r4, r4, 0x0000	/* CPU_CLK 40 ohms */
//      ori     r4, r4, 0x0080	/* SDRAM_SYNC_OUT disabled */
//      ori     r4, r4, 0x0078	/* SDRAM_CLK disabled */
//      ori     r4, r4, 0x0004	/* CPU_CLK0 disabled */
//      ori     r4, r4, 0x0002	/* CPU_CLK1 disabled */
        ori     r4, r4, 0x0001	/* CPU_CLK2 disabled */

        ori     r3, r10, 0x74
	bl      __pci_config_write_16

	/*
	 * MDCR
	 */
	li	r4, 0x00
//      ori     r4, r4, 0x80	/* MCP 1=open-drain, 0=output */
        ori     r4, r4, 0x40	/* SRESET 1=open-drain, 0=output */
        ori     r4, r4, 0x20	/* QACK 1=high-Z, 0=output */

        ori     r3, r10, 0x76
	bl      __pci_config_write_8

	/*
	 * MBEN
	 */
	li	r4, 0x03	/* Enable banks 0 and 1 */
        ori     r3, r10, 0xa0
	bl      __pci_config_write_8

        /* 
	 * PGMAX
	 */
        li      r4, 0x32	/* 33MHz value w/ROMFAL=8 */
        ori     r3, r10, 0xa3
        bl      __pci_config_write_8

	/*
	 * Wait 200us
	 */
        lis     r7, 0x0001
        mtctr   r7
wait1:	
	bdnz	wait1
	
	/*
	 * Now set memgo bit in MCCR1
	 */
        ori     r3, r10, 0xf0
    	bl  	__pci_config_read_32

	lis 	r7, 0x0008	/* MEMGO=1 */
	or 	r4, r7, r3

        ori     r3, r10, 0xf0
        bl      __pci_config_write_32

	/*
	 * Wait again
	 */

	lis	r7, 0x002
	ori	r7, r7, 0xffff
	mtctr	r7
wait2:
	bdnz	wait2

        mtlr    r11
        blr
        
