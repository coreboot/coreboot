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
 * Revision 4.0  1993/10/18  14:39:10  rusling
 * *** empty log message ***
 *
 *
 */
/* --------------------------------------------------- */
/* | Copyright (c) 1986 MIPS Computer Systems, Inc.  | */
/* | All Rights Reserved.                            | */
/* --------------------------------------------------- */

/* (C) Copyright 1984 by Third Eye Software, Inc.
 *
 * Third Eye Software, Inc. grants reproduction and use rights to
 * all parties, PROVIDED that this comment is maintained in the copy.
 *
 * Third Eye makes no claims about the applicability of this
 * symbol table to a particular use.
 */

/* glevels for field in FDR */
#define GLEVEL_0	2
#define GLEVEL_1	1
#define GLEVEL_2	0	/* for upward compat reasons. */
#define GLEVEL_3	3

/* magic number for symheader */
#define magicSym	0x1992

/* Language codes */
#define langC		0	
#define langPascal	1
#define langFortran	2
#define	langAssembler	3	/* one Assembley inst might map to many mach */
#define langMachine	4
#define langNil		5
#define langAda		6
#define langPl1		7
#define langCobol	8
#define langMax		32

/* The following are value definitions for the fields in the SYMR */

/*
 * Storage Classes
 */

#define scNil		0
#define scText		1	/* text symbol */
#define scData		2	/* initialized data symbol */
#define scBss		3	/* un-initialized data symbol */
#define scRegister	4	/* value of symbol is register number */
#define scAbs		5	/* value of symbol is absolute */
#define scUndefined	6	/* who knows? */
#define scCdbLocal	7	/* variable's value is IN se->va.?? */
#define scBits		8	/* this is a bit field */
#define scCdbSystem	9	/* variable's value is IN CDB's address space */
#define scDbx		9	/* overlap dbx internal use */
#define scRegImage	10	/* register value saved on stack */
#define scInfo		11	/* symbol contains debugger information */
#define scUserStruct	12	/* address in struct user for current process */
#define scSData		13	/* load time only small data */
#define scSBss		14	/* load time only small common */
#define scRData		15	/* load time only read only data */
#define scVar		16	/* Var parameter (fortran,pascal) */
#define scCommon	17	/* common variable */
#define scSCommon	18	/* small common */
#define scVarRegister	19	/* Var parameter in a register */
#define scVariant	20	/* Variant record */
#define scSUndefined	21	/* small undefined(external) data */
#define scInit		22	/* .init section symbol */
#define scBasedVar	23	/* Fortran or PL/1 ptr based var */ 
#define scXData         24      /* exception handling data */
#define scPData         25      /* Procedure section */
#define scFini          26      /* .fini section */
#define scMax		32


/*
 *   Symbol Types
 */

#define stNil		0	/* Nuthin' special */
#define stGlobal	1	/* external symbol */
#define stStatic	2	/* static */
#define stParam		3	/* procedure argument */
#define stLocal		4	/* local variable */
#define stLabel		5	/* label */
#define stProc		6	/*     "      "	 Procedure */
#define stBlock		7	/* beginnning of block */
#define stEnd		8	/* end (of anything) */
#define stMember	9	/* member (of anything	- struct/union/enum */
#define stTypedef	10	/* type definition */
#define stFile		11	/* file name */
#define stRegReloc	12	/* register relocation */
#define stForward	13	/* forwarding address */
#define stStaticProc	14	/* load time only static procs */
#define stConstant	15	/* const */
#define stStaParam	16	/* Fortran static parameters */
    /* Psuedo-symbols - internal to debugger */
#define stStr		60	/* string */
#define stNumber	61	/* pure number (ie. 4 NOR 2+2) */
#define stExpr		62	/* 2+2 vs. 4 */
#define stType		63	/* post-coersion SER */
#define stMax		64

/* definitions for fields in TIR */

/* type qualifiers for ti.tq0 -> ti.(itqMax-1) */
#define tqNil	0	/* bt is what you see */
#define tqPtr	1	/* pointer */
#define tqProc	2	/* procedure */
#define tqArray 3	/* duh */
#define tqFar	4	/* longer addressing - 8086/8 land */
#define tqVol	5	/* volatile */
#define tqConst 6	/* const */
#define tqMax	8

/* basic types as seen in ti.bt */
#define btNil		0	/* undefined */
#define btAdr32		1	/* 32-bit address */
#define btAdr		btAdr32	/* address - integer same size as pointer */
#define btChar		2	/* character */
#define btUChar		3	/* unsigned character */
#define btShort		4	/* short */
#define btUShort	5	/* unsigned short */
#define btInt32		6	/* 32-bit int */
#define btInt		btInt32	/* int */
#define btUInt32	7	/* 32-bit unsigned int */
#define btUInt		btUInt32	/* unsigned int */
#define btLong32	8	/* 32-bit long */
#define btLong		btLong32	/* long */
#define btULong32	9	/* 32-bit unsigned long */
#define btULong		btULong32	/* unsigned long */
#define btFloat		10	/* float (real) */
#define btDouble	11	/* Double (real) */
#define btStruct	12	/* Structure (Record) */
#define btUnion		13	/* Union (variant) */
#define btEnum		14	/* Enumerated */
#define btTypedef	15	/* defined via a typedef, isymRef points */
#define btRange		16	/* subrange of int */
#define btSet		17	/* pascal sets */
#define btComplex	18	/* fortran complex */
#define btDComplex	19	/* fortran double complex */
#define btIndirect	20	/* forward or unnamed typedef */
#define btFixedDec	21	/* Fixed Decimal */
#define btFloatDec	22	/* Float Decimal */
#define btString	23	/* Varying Length Character String */
#define btBit		24	/* Aligned Bit String */
#define btPicture	25	/* Picture */
#define btVoid		26	/* void */
#define btPtrMem	27	/* DEC C++:  Pointer to member */
#define btInt64		28	/* 64-bit int */
#define btUInt64	29	/* 64-bit unsigned int */
#define btLong64	30	/* 64-bit long */
#define btULong64	31	/* 64-bit unsigned long */
#define btLongLong64	32	/* 64-bit long long */
#define btLongLong	btLongLong64	/* long long */
#define btULongLong64	33	/* 64-bit unsigned long long */
#define btULongLong	btULongLong64	/* unsigned long long */
#define btAdr64		34	/* 64-bit address */
#define btMax		64

#if (MFG == MIPS)
/* optimization type codes */
#define otNil		0
#define otReg		1	/* move var to reg */
#define otBlock		2	/* begin basic block */
#define	otProc		3	/* procedure */
#define otInline	4	/* inline procedure */
#define otEnd		5	/* whatever you started */
#define otMax		6	/* KEEP UP TO DATE */
#endif (MFG == MIPS)


#ifdef LANGUAGE_C

/* 
 * This (part of the) file contains the definition of the Third Eye Symbol Table.
 *
 * Symbols are assumed to be in 'encounter order' - i.e. the order that
 * the things they represent were encountered by the compiler/assembler/loader.
 * EXCEPT for globals!	These are assumed to be bunched together,
 * probably right after the last 'normal' symbol.  Globals ARE sorted
 * in ascending order.
 *
 * -----------------------------------------------------------------
 * A brief word about Third Eye naming/use conventions:
 *
 * All arrays and index's are 0 based.
 * All "ifooMax" values are the highest legal value PLUS ONE. This makes
 * them good for allocating arrays, etc. All checks are "ifoo < ifooMax".
 *
 * "isym"	Index into the SYMbol table.
 * "ipd"	Index into the Procedure Descriptor array.
 * "ifd"	Index into the File Descriptor array.
 * "iss"	Index into String Space.
 * "cb"		Count of Bytes.
 * "rgPd"	array whose domain is "0..ipdMax-1" and RanGe is PDR.
 * "rgFd"	array whose domain is "0..ifdMax-1" and RanGe is FDR.
 */


/* 
 * Symbolic Header (HDR) structure.
 * As long as all the pointers are set correctly,
 * we don't care WHAT order the various sections come out in!
 *
 * A file produced solely for the use of CDB will probably NOT have
 * any instructions or data areas in it, as these are available
 * in the original.
 */

typedef struct {
	short	magic;		/* to verify validity of the table */
	short	vstamp;		/* version stamp */
	int	ilineMax;	/* number of line number entries */
	int	idnMax;		/* max index into dense number table */
	int	ipdMax;		/* number of procedures */
	int	isymMax;	/* number of local symbols */
	int	ioptMax;	/* max index into optimization symbol entries */
	int	iauxMax;	/* number of auxillary symbol entries */
	int	issMax;		/* max index into local strings */
	int	issExtMax;	/* max index into external strings */
	int	ifdMax;		/* number of file descriptor entries */
	int	crfd;		/* number of relative file descriptor entries */
	int	iextMax;	/* max index into external symbols */
	unsigned long	cbLine;	/* number of bytes for line number entries */
	unsigned long	cbLineOffset;/* offset to start of line number entries*/
	unsigned long	cbDnOffset;/* offset to start dense number table */
	unsigned long	cbPdOffset;/* offset to procedure descriptor table */
	unsigned long	cbSymOffset;/* offset to start of local symbols*/
	unsigned long	cbOptOffset;/* offset to optimization symbol entries */
	unsigned long	cbAuxOffset;/* offset to start of auxillary symbol entries*/
	unsigned long	cbSsOffset;/* offset to start of local strings */
	unsigned long	cbSsExtOffset;/* offset to start of external strings */
	unsigned long	cbFdOffset;/* offset to file descriptor table */
	unsigned long	cbRfdOffset;/* offset to relative file descriptor table */
	unsigned long	cbExtOffset;/* offset to start of external symbol entries*/
	/* If you add machine dependent fields, add them here */
	} HDRR, *pHDRR; 
#define cbHDRR sizeof(HDRR)
#define hdrNil ((pHDRR)0)

/*
 * The FDR and PDR structures speed mapping of address <-> name.
 * They are sorted in ascending memory order and are kept in
 * memory by CDB at runtime.
 */

/* 
 * File Descriptor
 *
 * There is one of these for EVERY FILE, whether compiled with
 * full debugging symbols or not.  The name of a file should be
 * the path name given to the compiler.	 This allows the user
 * to simply specify the names of the directories where the COMPILES
 * were done, and we will be able to find their files.
 * A field whose comment starts with "R - " indicates that it will be
 * setup at runtime.
 */
typedef struct fdr {
	unsigned long	adr;	/* memory address of beginning of file */
	unsigned long cbLineOffset;/* byte offset from header for this file ln's */
	unsigned long	cbLine;	/* size of lines for this file */
	unsigned long	cbSs;	/* number of bytes in the ss */
	int	rss;		/* file name (of source, if known) */
	int	issBase;	/* file's string space */
	int	isymBase;	/* beginning of symbols */
	int	csym;		/* count file's of symbols */
	int	ilineBase;	/* file's line symbols */
	int	cline;		/* count of file's line symbols */
	int	ioptBase;	/* file's optimization entries */
	int	copt;		/* count of file's optimization entries */
	int	ipdFirst;	/* start of procedures for this file */
	int	cpd;		/* count of procedures for this file */
	int	iauxBase;	/* file's auxiliary entries */
	int	caux;		/* count of file's auxiliary entries */
	int	rfdBase;	/* index into the file indirect table */
	int	crfd;		/* count file indirect entries */
	unsigned lang: 5;	/* language for this file */
	unsigned fMerge : 1;	/* whether this file can be merged */
	unsigned fReadin : 1;	/* true if it was read in (not just created) */
	unsigned fBigendian : 1;/* if set, was compiled on big endian machine */
				/*	aux's will be in compile host's sex */
	unsigned glevel : 2;	/* level this file was compiled with */
	unsigned reserved : 22;  /* reserved for future use */
	} FDR, *pFDR;
#define cbFDR sizeof(FDR)
#define fdNil ((pFDR)0)
#define ifdNil -1
#define ifdTemp 0
#define ilnNil -1


/* 
 * Procedure Descriptor
 *
 * There is one of these for EVERY TEXT LABEL.
 * If a procedure is in a file with full symbols, then isym
 * will point to the PROC symbols, else it will point to the
 * global symbol for the label.
 */

typedef struct pdr {
	unsigned long	adr;	/* memory address of start of procedure */
	unsigned long	cbLineOffset;/* byte offset for this procedure from the fd base */
	int	isym;		/* start of local symbol entries */
	int	iline;		/* start of line number entries*/
	int	regmask;	/* save register mask */
	int	regoffset;	/* save register offset */
	int	iopt;		/* start of optimization symbol entries*/
	int	fregmask;	/* save floating point register mask */
	int	fregoffset;	/* save floating point register offset */
	int	frameoffset;	/* frame size */
	int	lnLow;		/* lowest line in the procedure */
	int	lnHigh;		/* highest line in the procedure */
	unsigned gp_prologue : 8; /* byte size of GP prologue */
	unsigned gp_used : 1;	/* true if the procedure uses GP */
	unsigned reserved : 23;
	short	framereg;	/* frame pointer register */
	short	pcreg;		/* offset or reg of return pc */
	} PDR, *pPDR;
#define cbPDR sizeof(PDR)
#define pdNil ((pPDR) 0)
#define ipdNil	-1

/*
 * The structure of the runtime procedure descriptor created by the loader
 * for use by the static exception system.
 */
typedef struct runtime_pdr {
	unsigned long	adr;	/* memory address of start of procedure */
	int	regmask;	/* save register mask */
	int	regoffset;	/* save register offset */
	int	fregmask;	/* save floating point register mask */
	int	fregoffset;	/* save floating point register offset */
	int	frameoffset;	/* frame size */
	short	framereg;	/* frame pointer register */
	short	pcreg;		/* offset or reg of return pc */
	int	irpss;		/* index into the runtime string table */
	int	reserved;
	struct exception_info *exception_info;/* pointer to exception array */
} RPDR, *pRPDR;
#define cbRPDR sizeof(RPDR)
#define rpdNil ((pRPDR) 0)

/*
 * Line Numbers
 *
 * Line Numbers are segregated from the normal symbols because they
 * are [1] smaller , [2] are of no interest to your
 * average loader, and [3] are never needed in the middle of normal
 * scanning and therefore slow things down.
 *
 * By definition, the first LINER for any given procedure will have
 * the first line of a procedure and represent the first address.
 */

typedef int LINER, *pLINER;
#define lineNil ((pLINER)0)
#define cbLINER sizeof(LINER)
#define ilineNil	-1



/*
 * The Symbol Structure		(GFW, to those who Know!)
 */

typedef struct {
	unsigned long	value;	/* value of symbol */
	int	iss;		/* index into String Space of name */
	unsigned st : 6;	/* symbol type */
	unsigned sc  : 5;	/* storage class - text, data, etc */
	unsigned reserved : 1;	/* reserved */
	unsigned index : 20;	/* index into sym/aux table */
	} SYMR, *pSYMR;
#define symNil ((pSYMR)0)
#define cbSYMR sizeof(SYMR)
#define isymNil -1
#define indexNil 0xfffff
#define issNil -1
#define issNull 0


/* The following converts a memory resident string to an iss.
 * This hack is recognized in SbFIss, in sym.c of the debugger.
 */
#define IssFSb(sb) (0x80000000 | ((unsigned int)(sb)))

/* E X T E R N A L   S Y M B O L  R E C O R D
 *
 *	Same as the SYMR except it contains file context to determine where
 *	the index is.
 */
typedef struct {
	SYMR	asym;		/* symbol for the external */
	unsigned jmptbl:1;	/* symbol is a jump table entry for shlibs */
	unsigned cobol_main:1;	/* symbol is a cobol main procedure */
	unsigned weakext:1;	/* symbol is weak external */
	unsigned reserved:29;	/* reserved for future use */
	int	ifd;		/* where the iss and index fields point into */
	} EXTR, *pEXTR;
#define extNil ((pEXTR)0)
#define cbEXTR sizeof(EXTR)


/* A U X I L L A R Y   T Y P E	 I N F O R M A T I O N */

/*
 * Type Information Record
 */
typedef struct {
	unsigned fBitfield : 1; /* set if bit width is specified */
	unsigned continued : 1; /* indicates additional TQ info in next AUX */
	unsigned bt  : 6;	/* basic type */
	unsigned tq4 : 4;
	unsigned tq5 : 4;
	/* ---- 16 bit boundary ---- */
	unsigned tq0 : 4;
	unsigned tq1 : 4;	/* 6 type qualifiers - tqPtr, etc. */
	unsigned tq2 : 4;
	unsigned tq3 : 4;
	} TIR, *pTIR;
#define cbTIR sizeof(TIR)
#define tiNil ((pTIR)0)
#define itqMax 6

/*
 * Relative symbol record
 *
 * If the rfd field is 4095, the index field indexes into the global symbol
 *	table.
 */

typedef struct {
	unsigned	rfd : 12;    /* index into the file indirect table */
	unsigned	index : 20; /* index int sym/aux/iss tables */
	} RNDXR, *pRNDXR;
#define cbRNDXR sizeof(RNDXR)
#define rndxNil ((pRNDXR)0)

/* dense numbers or sometimes called block numbers are stored in this type,
 *	a rfd of 0xffffffff is an index into the global table.
 */
typedef struct {
	unsigned int	rfd;    /* index into the file table */
	unsigned int	index; 	/* index int sym/aux/iss tables */
	} DNR, *pDNR;
#define cbDNR sizeof(DNR)
#define dnNil ((pDNR)0)



/*
 * Auxillary information occurs only if needed.
 * It ALWAYS occurs in this order when present.

	    isymMac		used by stProc only
	    TIR			type info
	    TIR			additional TQ info (if first TIR was not enough)
	    rndx		if (bt == btStruct,btUnion,btEnum,btSet,btRange,
				    btTypedef):
				    rsym.index == iaux for btSet or btRange
				    else rsym.index == isym
	    dimLow		btRange, btSet
	    dimMac		btRange, btSet
	    rndx0		As many as there are tq arrays
	    dimLow0
	    dimHigh0
	    ...
	    rndxMax-1
	    dimLowMax-1
	    dimHighMax-1
	    width in bits	if (bit field), width in bits.
 */
#define cAuxMax (6 + (idimMax*3))

/* a union of all possible info in the AUX universe */
typedef union {
	TIR	ti;		/* type information record */
	RNDXR	rndx;		/* relative index into symbol table */
	int	dnLow;		/* low dimension */
	int	dnHigh;		/* high dimension */
	int	isym;		/* symbol table index (end of proc) */
	int	iss;		/* index into string space (not used) */
	int	width;		/* width for non-default sized struc fields */
	int	count;		/* count of ranges for variant arm */
	} AUXU, *pAUXU;
#define cbAUXU sizeof(AUXU)
#define auxNil ((pAUXU)0)
#define iauxNil -1


/*
 * Optimization symbols
 *
 * Optimization symbols contain some overlap information with the normal
 * symbol table. In particular, the proc information
 * is somewhat redundant but necessary to easily find the other information
 * present. 
 *
 * All of the offsets are relative to the beginning of the last otProc
 */

typedef struct {
	unsigned ot: 8;		/* optimization type */
	unsigned value: 24;	/* address where we are moving it to */
	RNDXR	rndx;		/* points to a symbol or opt entry */
	unsigned int	offset;	/* relative offset this occured */
	} OPTR, *pOPTR;
#define optNil	((pOPTR) 0)
#define cbOPTR sizeof(OPTR)
#define ioptNil -1

/*
 * File Indirect
 *
 * When a symbol is referenced across files the following procedure is used:
 *	1) use the file index to get the File indirect entry.
 *	2) use the file indirect entry to get the File descriptor.
 *	3) add the sym index to the base of that file's sym table
 *
 */

typedef int RFDT, *pRFDT;
#define cbRFDT sizeof(RFDT)
#define rfdNil	-1

/*
 * The file indirect table in the mips loader is known as an array of FITs.
 * This is done to keep the code in the loader readable in the area where
 * these tables are merged.  Note this is only a name change.
 */
typedef int FIT, *pFIT;
#define cbFIT	sizeof(FIT)
#define ifiNil	-1
#define fiNil	((pFIT) 0)


/* Dense numbers
 *
 * Rather than use file index, symbol index pairs to represent symbols
 *	and globals, we use dense number so that they can be easily embeded
 *	in intermediate code and the programs that process them can
 *	use direct access tabls instead of hash table (which would be
 *	necesary otherwise because of the sparse name space caused by
 *	file index, symbol index pairs. Dense number are represented
 *	by RNDXRs.
 */

/*
 * The following table defines the meaning of each SYM field as
 * a function of the "st". (scD/B == scData OR scBss)
 *
 * Note: the value "isymMac" is used by symbols that have the concept
 * of enclosing a block of related information.	 This value is the
 * isym of the first symbol AFTER the end associated with the primary
 * symbol. For example if a procedure was at isym==90 and had an
 * isymMac==155, the associated end would be at isym==154, and the
 * symbol at 155 would probably (although not necessarily) be the
 * symbol for the next procedure.  This allows rapid skipping over
 * internal information of various sorts. "stEnd"s ALWAYS have the
 * isym of the primary symbol that started the block.
 * 

ST		SC	VALUE		INDEX
--------	------	--------	------
stFile		scText	address		isymMac
stLabel		scText	address		---
stGlobal	scD/B	address		iaux
stStatic	scD/B	address		iaux
stParam		scAbs	offset		iaux
stLocal		scAbs	offset		iaux
stProc		scText	address		iaux	(isymMac is first AUX)
stStaticProc	scText	address		iaux	(isymMac is first AUX)

stMember	scNil	ordinal		---	(if member of enum)
stMember	scNil	byte offset	iaux	(if member of struct/union)
stMember	scBits	bit offset	iaux	(bit field spec)

stBlock		scText	address		isymMac (text block)
stBlock		scNil	cb		isymMac (struct/union member define)
stBlock		scNil	cMembers	isymMac (enum member define)

stEnd		scText	address		isymStart
stEnd		scNil	-------		isymStart (struct/union/enum)

stTypedef	scNil	-------		iaux
stRegReloc	sc???	value		old register number
stForward	sc???	new address	isym to original symbol

stConstant	scInfo	value		--- (scalar)
stConstant	scInfo	iss		--- (complex, e.g. string)

 *
 */

#endif LANGUAGE_C

