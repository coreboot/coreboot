struct reloc {
    long	r_vaddr;	/* (virtual) address of reference */
    unsigned	r_symndx;	/* index into symbol table */
    unsigned	r_type 	: 8;	/* relocation type */
    unsigned	r_extern: 1;	/* if 1 symndx is an index into the external
				   symbol table, else symndx is a section # */

    unsigned	r_offset:6;	/* for R_OP_STORE, quad based LE bit offset */
    unsigned	r_reserved:11;	/* Must be zero */
    unsigned	r_size:6;	/* R_OP_STORE, bit size */
};
#define	RELOC	struct reloc
#define	RELSZ	sizeof(RELOC)


/*
 * ALPHA machines r_type values
 *
 *	reloc. already performed to symbol in the same section
 *	32-bit reference
 *	64-bit reference
 *	32-bit displacement from gp
 *	reference to global pointer relative literal pool item
 *	identifies useage of a literal address previously loaded
 *	lda/ldah instruction pair to initialize gp.
 *	21-bit branch reference
 *	14-bit jsr hint reference
 */

#define R_ABS		0
#define R_REFLONG	1
#define R_REFQUAD	2
#define R_GPREL32	3
#define R_LITERAL	4
#define R_LITUSE	5
#define R_GPDISP	6
#define R_BRADDR	7
#define R_HINT		8
/*
 *	self relative relocations mean that the memory location at
 *	r_vaddr contains an offset to the destination. If the relocation
 *	is r_extern==1, then the value at the memory location is ignored
 *	(maybe we should allow offsets?). If r_extern==0, then the value
 *	at the memory location is the actual offset. 
 *
 *	The linker uses the relocated target and a relocated r_vaddr to
 *	determine the offset. Offsets are considered signed.
 */
#define R_SREL16	9		/* self relative 16 bit offset */
#define R_SREL32	10		/* self relative 32 bit offset */
#define R_SREL64	11		/* self relative 64 bit offset */
/* 
 *	stack relocations provide a primitive expression evaluator for
 *	relocatable and constant values at link time. It also provides
 *	a way to store a value into a bit field (the R_OP_STORE has a
 *	bit size and offset field (from a quadword aligned qaudword)).
 *
 *	The operations specify what they relocate and what happens to
 *	the linktime stack. It is an error to cause a stack underflow
 *	or have values left on the stack when the relocation for a section
 *	is complete.
 *
 *	terms:
 *		tos		top of stack
 *		stack		qaudword array representing the stack
 *		vaddr		address field in reloc record or 
 *					extern symbol address
 *		relocate(X)	relocate address X
 *		X(o:s)		address X, bitoffset o, bit size s
 *		r_offset	offset field in reloc record
 *		r_size		bitsize field in reloc record
 *
 *	Note: use R_SN_ABS as the section for constants (like in shifts).
 *		
 */
#define R_OP_PUSH	12	/* stack[++tos] = relocate(vaddr) */
#define R_OP_STORE	13	/* vaddr(r_offset:r_size) = stack[tos--] */
#define R_OP_PSUB	14	/* stack[tos] = stack[tos] - relocate(vaddr) */
#define R_OP_PRSHIFT	15	/* stack[tos] = stack[tos] >> relocate(vaddr) */

#define MAX_R_TYPE	16

/*
 * Section numbers for symndex for local relocation entries (r_extern == 0).
 * For these entries the starting address for the section referenced by the
 * section number is used in place of an external symbol table entry's value.
 */
#define	R_SN_NULL	0
#define	R_SN_TEXT	1
#define	R_SN_RDATA	2
#define	R_SN_DATA	3
#define	R_SN_SDATA	4
#define	R_SN_SBSS	5
#define	R_SN_BSS	6
#define	R_SN_INIT	7
#define	R_SN_LIT8	8
#define	R_SN_LIT4	9
#define	R_SN_XDATA	10
#define	R_SN_PDATA	11
#define R_SN_FINI       12
#define	R_SN_LITA	13
#define R_SN_ABS	14		/* constant relocation r_vaddr's */
#define MAX_R_SN	14

/*
 * symindex values when type is R_LITUSE
 *
 *	literal address in register of a memory format instruction
 *	literal address in byte offset register of byte-manipulation instruction
 *	literal address is in target register of a jsr instruction.
 */

#define R_LU_BASE	1
#define R_LU_BYTOFF	2
#define R_LU_JSR	3

