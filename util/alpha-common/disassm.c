#ifndef lint
static char *RCSid = "$Id$";
#endif
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/
/*
**
** FACILITY:	
**
**	ED64 Software Tools - 21064 Disassembler.
** 
** FUNCTIONAL DESCRIPTION:
**
**      Common code used by many utilities to disassemble
**      21064 object code.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: Anthony Berent
**
** CREATION-DATE: NOV-1992
** 
** MODIFIED BY: 
** 
**
** $Log$
** Revision 1.1  2001/03/22 21:26:32  rminnich
** testing I HATE CVS
**
** Revision 1.1  2000/03/21 03:56:31  stepan
** Check in current version which is a nearly a 2.2-16
**
 * Revision 4.2  1994/08/06  00:00:00  fdh
 * Updated Copyright header
 *
 * Revision 4.1  1994/07/18  09:42:34  rusling
 * Fixed WNT compile warnings.
 *
**
**
**	AB	   Nov-1992	First version.
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "alpha_op.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*==============================*/
/* Masks for instruction fields */
/*==============================*/

/* Masks used to recognise the instruction */

#define OPCODE_MASK 0xFC000000
#define IMMEDIATE_MASK 0x00001000
#define JUMP_FUNCTION_MASK 0x0000C000
#define MEMORY_FUNCTION_MASK 0x0000FFFF
#define INTEGER_OPERATE_FUNCTION_MASK 0x00000FE0
#define FPOINT_OPERATE_FUNCTION_MASK 0x0000FFE0
#define HW_MODE_BITS_MASK	     0x0000F000
#define HW_REG_SET_MASK		     0x000000E0

/* Masks used to find arguments */
#define RA_MASK 0x03E00000
#define RB_MASK 0x001F0000
#define RC_MASK 0x0000001F
#define MEM_DISP_MASK 0x0000FFFF
#define HW_MEM_DISP_MASK 0x00000FFF
#define JUMP_HINT_MASK 0x00003FFF
#define BRANCH_DISP_MASK 0x001FFFFF
#define LITERAL_MASK 0x001FE000
#define PALCODE_FUNCTION_MASK 0x03FFFFFF
#define INTEGER_OPERATE_SBZ_MASK 0x0000E000
#define MXPR_DISPLACEMENT_MASK 0x0000000F

/* Shifts for arguments */

#define RA_SHIFT 21
#define RB_SHIFT 16
#define RC_SHIFT 0
#define MEM_DISP_SHIFT 0
#define HW_MEM_DISP_SHIFT 0
#define JUMP_HINT_SHIFT 0
#define BRANCH_DISP_SHIFT 0
#define LITERAL_SHIFT 13
#define PALCODE_FUNCTION_SHIFT 0


/* Function to search for a instruction in the opcode table */
struct alpha_opcode *find_opcode(unsigned int instruction)
{
    struct alpha_opcode *opcodedata = 0;
    int i;

    /* Search the opcode table */
    for (i = 0; i < NUMOPCODES; i++)
      {

	/* Check if the function code matches */
	if ((alpha_opcodes[i].match & OPCODE_MASK) ==
	  (instruction & OPCODE_MASK))
	  {

	    /* opcode matches so this is a candidate; check what arguments it
	     * takes to work out whether */
	    /* a function code has to match */
	    if (strcmp(alpha_opcodes[i].args, "1,l(2)") == 0 || 
	      strcmp(alpha_opcodes[i].args, "e,l(2)") == 0 || 
	      strcmp(alpha_opcodes[i].args, "1,L") == 0)
	      {

		/* Memory or branch format without function code */
		opcodedata = &alpha_opcodes[i];
		break;
	    }
	      else if (strcmp(alpha_opcodes[i].args, "1,(2),i") == 0)
	      {

		/* Jump (Memory format branch) format; check the jump function 
		 * mask */
		if ((alpha_opcodes[i].match & JUMP_FUNCTION_MASK) ==
		  (instruction & JUMP_FUNCTION_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "1,t(2)") == 0)
	      {

		/* EV4 hardware memory access format format; check the mode
		 * bits */
		if ((alpha_opcodes[i].match & HW_MODE_BITS_MASK) ==
		  (instruction & HW_MODE_BITS_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "R,3") == 0)
	      {

		/* EV4 internal register access format format; check the
		 * register set mask */
		if ((alpha_opcodes[i].match & HW_REG_SET_MASK) ==
		  (instruction & HW_REG_SET_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "R,8") == 0)
	      {

		/* EV4 internal register access format format; check the
		 * register set mask */
		if ((alpha_opcodes[i].match & HW_REG_SET_MASK)
		  == (instruction & HW_REG_SET_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "1,2,3") == 0 || 
	      strcmp(alpha_opcodes[i].args, "1,b,3") == 0)
	      {

		/* Operate format; check immediate bit and function code */
		if ((alpha_opcodes[i].match & IMMEDIATE_MASK) == 
		  (instruction & IMMEDIATE_MASK) && 
		  (alpha_opcodes[i].match & INTEGER_OPERATE_FUNCTION_MASK) == 
		  (instruction & INTEGER_OPERATE_FUNCTION_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "e,f,g") == 0 || 
	      strcmp(alpha_opcodes[i].args, "f,g") == 0)
	      {

		/* Operate format; check immediate bit and function mask */
		if ((alpha_opcodes[i].match & FPOINT_OPERATE_FUNCTION_MASK) ==
		  (instruction & FPOINT_OPERATE_FUNCTION_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "I") == 0)
	      {

		/* We have found a PAL code call instruction - there may a more
		 * precise opcode defined */
		/* so only use this code if we have not already found a match
		 */

		if (opcodedata == 0)
		     opcodedata = &alpha_opcodes[i];

	    /* Don't exit the loop; there may be a more precise match yet */
	    }
	      else if (strcmp(alpha_opcodes[i].args, "0(2)") == 0)
	      {

		/* Miscellaneous instructions - Memory format with function
		 * code*/
		if ((alpha_opcodes[i].match & MEMORY_FUNCTION_MASK) ==
		  (instruction & MEMORY_FUNCTION_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }
	      else if (strcmp(alpha_opcodes[i].args, "1") == 0)
	      {

		/* Miscellaneous instructions - Memory format with function
		 * code but different arguments */
		if ((alpha_opcodes[i].match & MEMORY_FUNCTION_MASK) ==
		  (instruction & MEMORY_FUNCTION_MASK))
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }

	      else if (strcmp(alpha_opcodes[i].args, "") == 0)
	      {

		/* Miscellaneous instructions with no arguments; should be
		 * exact match */
		if (alpha_opcodes[i].match == instruction)
		  {

		    /* This is what we want */
		    opcodedata = &alpha_opcodes[i];
		    break;
		}
	    }

	/* All other cases are only used for predefined macros and
	 * psedo-instructions; ignore them */
	}
    }

    /* Return the opcode data; it will be null if a match hasn't been found */
    return opcodedata;
}
/* Function to print an instruction and arguments */
void print_instruction(int instruction, FILE *stream)
{
    struct alpha_opcode *opcodedata;
    char *argptr;
    int duplicate_reg_bad = FALSE;
    int bad_reg_id;
    char bad_reg_name;


    /* Try to find it in the opcode table */
    opcodedata = find_opcode(instruction);
    if (opcodedata == 0)
      {
	fprintf(stream, "\t# BAD INSTRUCTION 0x%08x \n", instruction);
	return;
    }

    /* Print the instruction name */
    fprintf(stream, "\t%s\t", opcodedata->name);

    /* Now scan its arguments */
    argptr = opcodedata->args;

    while (*argptr != 0)
      {
	switch (*argptr)
	  {

	    /* Real argument types */
	    case '1': 
	    case 'e': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		break;
	    case '2': 
	    case 'f': 
		fprintf(stream, "$%i", (instruction & RB_MASK) >> RB_SHIFT);
		break;
	    case '3': 
	    case 'g': 
		fprintf(stream, "$%i", (instruction & RC_MASK) >> RC_SHIFT);
		break;
	    case 'r': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		if (((instruction & RC_MASK) >> RC_SHIFT) !=
		  ((instruction & RA_MASK) >> RA_SHIFT))
		  {
		    duplicate_reg_bad = TRUE;
		    bad_reg_name = 'C';
		    bad_reg_id = (instruction & RC_MASK) >> RC_SHIFT;
		}
		break;
	    case 'R': 
		fprintf(stream, "$%i", (instruction & RA_MASK) >> RA_SHIFT);
		if (((instruction & RB_MASK) >> RB_SHIFT) !=
		  ((instruction & RA_MASK) >> RA_SHIFT))
		  {
		    duplicate_reg_bad = TRUE;
		    bad_reg_name = 'B';
		    bad_reg_id = (instruction & RB_MASK) >> RB_SHIFT;
		}
		break;
	    case 'I': 
		fprintf(stream, "0x%07x", (instruction &
		  PALCODE_FUNCTION_MASK) >> PALCODE_FUNCTION_SHIFT);
		break;
	    case 'l': 
		fprintf(stream, "0x%04x",
		  (instruction & MEM_DISP_MASK) >> MEM_DISP_SHIFT);
		break;
	    case 'h': 
		fprintf(stream, "0x%04x",
		  ((instruction & MEM_DISP_MASK) >> MEM_DISP_SHIFT) << 16);
		break;
	    case 't': 

		/* Special for EV4 hardware load/store instructions */
		fprintf(stream, "0x%04x",
		  ((instruction & HW_MEM_DISP_MASK) >> HW_MEM_DISP_SHIFT));
		break;
	    case 'L': 
		fprintf(stream, ".+0x%08x",
		  ((instruction & BRANCH_DISP_MASK) >> BRANCH_DISP_SHIFT) * 4);
		break;
	    case 'i': 
		fprintf(stream, ".+0x%06x",
		  ((instruction & JUMP_HINT_MASK) >> JUMP_HINT_SHIFT) * 4);
		break;
	    case 'b': 
		fprintf(stream, "0x%02x",
		  (instruction & LITERAL_MASK) >> LITERAL_SHIFT);
		break;
	    case '8': 
		fprintf(stream, "0x%02x",
		  (instruction & MXPR_DISPLACEMENT_MASK));
		break;

	    /* All other possible characters are punctuation; print what we are
	     * given */
	    default: 
		fprintf(stream, "%c", *argptr);
	}
	argptr++;
    }

    /* Check for some special cases where fields must be zero etc. */
    if (strcmp(opcodedata->args, "1,2,3") == 0)
      {
	if ((instruction & INTEGER_OPERATE_SBZ_MASK) != 0)
	  {
	    fprintf(stream, " WARNING: SBZ field not zero");
	}
    }
      else if (strcmp(opcodedata->args, "f,g") == 0)
      {
	if ((instruction & RA_MASK) != RA_MASK)
	  {
	    fprintf(stream, " WARNING: RA field not $31");
	}
    }

    if (duplicate_reg_bad)
      {
	fprintf(stream,
	  " WARNING: R%c register field does not match RA - value %i",
	  bad_reg_name, bad_reg_id);
    }
}

