/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * 31-May-92  Alessandro Forin (af) at Carnegie-Mellon University
 *	Adapted for Alpha.
 *
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
 * 
 */
/*
 *	File: coff.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	Structure definitions for COFF headers
 */

struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	int		f_timdat;	/* time & date stamp */
	unsigned long	f_symptr;	/* file pointer to symtab */
	int		f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
};

#define  F_EXEC		0000002

#define ALPHAMAGIC	0603

struct scnhdr {
	char		s_name[8];	/* section name */
	unsigned long	s_paddr;	/* physical address */
	unsigned long	s_vaddr;	/* virtual address */
	unsigned long	s_size;		/* section size */
	unsigned long	s_scnptr;	/* file ptr to raw data for section */
	unsigned long	s_relptr;	/* file ptr to relocation */
	unsigned long	s_lnnoptr;	/* file ptr to line numbers */
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of line number entries */
	int		s_flags;	/* flags */
};



struct aouthdr {
	short		magic;		/* see magic.h			*/
	short		vstamp;		/* version stamp		*/
	int		pad;		/* help for cross compilers	*/
	unsigned long	tsize;		/* text size in bytes, padded to FW
					   bdry				*/
	unsigned long	dsize;		/* initialized data "  "	*/
	unsigned long	bsize;		/* uninitialized data "   "	*/
	unsigned long	entry;		/* entry point, value of "start"*/
	unsigned long	text_start;	/* base of text used for this file*/
	unsigned long	data_start;	/* base of data used for this file*/
	unsigned long	bss_start;	/* base of bss used for this file */
	int		gprmask;	/* general purpose register mask*/
	int		fprmask;	/* FPA register mask		*/
	unsigned long	gp_value;	/* the gp value used for this object*/
};


#define OMAGIC	0407		/* old impure format */
#define NMAGIC	0410		/* read-only text */
#define ZMAGIC	0413		/* demand load format */

#define	N_BADMAG(a) \
  ((a).magic != OMAGIC && (a).magic != NMAGIC && (a).magic != ZMAGIC)

#define SCNROUND ((long)16)

struct exechdr {
	struct filehdr	f;
	struct aouthdr	a;
};

#define N_TXTOFF(f, a) \
 (((a).magic == ZMAGIC) ? 0 : \
   ((sizeof(struct filehdr) + sizeof(struct aouthdr) + \
    (f).f_nscns * sizeof(struct scnhdr) + SCNROUND-1) & ~(SCNROUND -1))) 

