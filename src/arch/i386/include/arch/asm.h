#ifndef ASM_H
#define	ASM_H

#define ASSEMBLER

/*
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 *
 * $Id$
 *
 */

#define	I386_ALIGN_TEXT	0
#define	I386_ALIGN_DATA	0

/*
 *	XXX
 */
#ifdef __ELF__
#define	EXT(x)		x
#else
#define	EXT(x)		_ ## x
#endif

#define	STATIC(x)	.align I386_ALIGN_TEXT; EXT(x): 
#define	GLOBAL(x)	.globl EXT(x); STATIC(x)
#define	ENTRY(x)	.text; GLOBAL(x)

#endif /* ASM_H */
