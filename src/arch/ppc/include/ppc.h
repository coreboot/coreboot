/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#ifndef _PPC_H
#define _PPC_H

#define BIG_ENDIAN
#define RODATA __attribute__ ((__section__ (".rodata"))) 

/* Do CPU specific setup, with optional icache */
void ppc_setup_cpu(int icache);

void ppc_enable_dcache(void);
void ppc_disable_dcache(void);
void ppc_enable_mmu(void);

/* Describe which sort of ppc CPU I am */
void ppc_identify(void);

#endif
