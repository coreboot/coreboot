/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1  2001/03/22 21:26:32  rminnich
 * testing I HATE CVS
 *
 * Revision 1.1  2000/03/21 03:56:31  stepan
 * Check in current version which is a nearly a 2.2-16
 *
 * Revision 4.0  1993/10/18  15:37:42  rusling
 * Include file.
 *
 * Revision 4.0  1993/10/18  14:38:36  rusling
 * *** empty log message ***
 *
 */
/*
 * exec stucture in an a.out file derived from FSF's
 * a.out.gnu.h file.
 */

#ifndef	_ALPHA_EXEC_H_
#define	_ALPHA_EXEC_H_

/*
 * Header prepended to each a.out file.
 */
struct exec
{
	integer_t a_magic;	/* Use macros N_MAGIC, etc for access */
	vm_size_t a_text;	/* bytes of text in file */
	vm_size_t a_data;	/* bytes of data in file */
	vm_size_t a_bss;	/* bytes of auto-zeroed data */
	vm_size_t a_syms;	/* bytes of symbol table data in file */
	vm_offset_t a_entry;	/* start PC */
	vm_offset_t a_tstart;	/* text start, in memory */
	vm_offset_t a_dstart;	/* data start, in memory */
	vm_size_t a_trsize;	/* bytes of text-relocation info in file */
	vm_size_t a_drsize;	/* bytes of data-relocation info in file */
};

/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413

#endif	_ALPHA_EXEC_H_
