/*****************************************************************************

Copyright © 1994, Digital Equipment Corporation, Maynard, Massachusetts. 

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
**	EBxx Software Tools - makerom
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Makerom makes roms.  It takes a series of input files
**      adds headers to them and outputs them either compressed
**      or uncompressed (the default).   This module contains the
**      compressioin code.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: David A Rusling
**
** CREATION-DATE: 04-Feb-1994
** 
** MODIFIED BY: 
**
*/
#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 * $Log$
 * Revision 1.1  2001/03/22 21:26:32  rminnich
 * testing I HATE CVS
 *
 * Revision 1.1  2000/03/21 03:56:31  stepan
 * Check in current version which is a nearly a 2.2-16
 *
 * Revision 1.9  1995/03/04  05:54:15  fdh
 * Keep track of output size.
 * Use the pre-defined macros for computing checksum.
 *
 * Revision 1.8  1995/02/08  01:22:40  fdh
 * Pad to longword align the end of the ROM image.
 *
 * Revision 1.7  1995/02/07  01:01:12  fdh
 * Removed some unnecessary definitions.
 *
 * Revision 1.6  1995/02/05  02:02:07  fdh
 * Modified to accept the latest romheader definition.
 *
 * Revision 1.5  1995/02/02  20:28:03  fdh
 * Wrapped _fputc() with output_c() to keep count of the
 * bytes written.
 * Modified to work in two passes with conditional
 * output enable.
 *
 * Revision 1.4  1994/07/04  10:58:22  rusling
 * Fixed WNT compile warnings.
 *
 * Revision 1.3  1994/04/19  12:37:53  rusling
 * Fixed up compression message.
 *
 * Revision 1.2  1994/03/03  16:15:38  rusling
 * Fixed bug.
 *
 * Revision 1.5  1994/02/04  11:51:26  rusling
 * Ported to Alpha WNT.
 *
 * Revision 1.3  1994/02/04  11:22:35  rusling
 * Fixed but (not resetting index) in push() when
 * we flush some of the stack.
 *
 * Revision 1.2  1994/02/04  10:17:26  rusling
 * Added RCS bits to the file.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "c_32_64.h"
#include "romhead.h"

/*
 * Function prototypes
 */
extern int compress(FILE * in , FILE * out);
static int pop(void);
static void flush(FILE * out);
static void push(int c , FILE * out);
static void Repeater(int c , int repeat , FILE * out);
static void output_c(int c , FILE * f);

#define MAXDIFFERENT 127
#define MAXREPEAT 128
#define MINREPEAT 2            /* As when this is output as
				  a repeated record, then the
				  output is no longer than the
				  input */

#define _minimum(a,b) ((a < b)? (a) : (b))
#define _maximum(a,b) ((a > b)? (a) : (b))

/*
 *  Global variables used by the helper routines.
 */
#define MAXBUFFER  (2 * (MINREPEAT + MAXDIFFERENT))
int buffer[MAXBUFFER];
int idx = 0;
extern int output_enable;
extern romheader_t head;

/*
 *  Global values used by statistics gatherers.
 */
int ccount;
int ocount;
int rrecords;
int nrecords;
int toosmall;
/*****************************************************
 *  Helper routines maintaining a stack.             *
 *****************************************************/

/*
 *  Pop the top charater off of the stack.
 */
static int pop()
{
    if (idx != 0)
	return buffer[idx--];
    else {
	fprintf(stderr, "ERROR: attempting to pop an empty stack\n");
	exit(0);
    }
}

/*
 *  Flush the stack out as a series of non
 *  repeating records of maximum size MAXDIFFERENT.
 */
static void flush(FILE *out)
{
    int i = 0, j;

    while (idx > i) {
	j = 0;
	output_c(_minimum(MAXDIFFERENT, idx - i), out);
	while (j < _minimum(MAXDIFFERENT, idx - i)) {
	    output_c(buffer[i + (j++)], out);
        }
	nrecords++;
	i = i + MAXDIFFERENT;
    }
    idx = 0;
}
/*
 *  Push a character onto the stack, if it gets full
 *  then flush it.  The last character must be available.
 *  This is wy we pass in the output file stream pointer.
 */
static void push(int c, FILE *out)
{
    if (idx == MAXBUFFER) 
      flush(out);

    buffer[idx++] = c;
}

/*
 *  Print out a series of repeater records.
 */
static void Repeater(int c, int repeat, FILE *out)
{
    int i = 0;

    while (repeat > i) {
	output_c(-(_minimum(MAXREPEAT, repeat - i)), out);
	output_c(c, out);
	rrecords++;
	i = i + MAXREPEAT;
    }
}


int compress(FILE *in, FILE *out)
{
    int last, this;
    int padding;

#define REPEATING 1
#define NONE 0
    int state = NONE;
    int repeat = 0;
    float compression;

ccount = 0;
ocount = 0;
rrecords = 0;
nrecords = 0;
toosmall = 0;

/*
 *  Read the input stream byte by byte writing
 *  compression records.  These are either repeating records 
 *  or non-repeating records.  Finally, output an end of
 *  file record (0).
 */
    last = getc(in);
    push(last, out);
    ccount++;

    this = getc(in);
    while (!feof(in)) {
	ccount++;
	if (state == REPEATING) {
/*
 *  At least the last two characters were the same.  If this
 *  one is then we're still in a run of the same character
 *  (which is "last").  However, if it's different, then
 *  we've reached the end of the repeat run.
 */
	    if (last == this) {
		repeat++;
	    } else {
/*
 *  We've reached the end of a repeating string of character
 *  "last".  If this repeat string is not long enough, then do
 *  not worry about, just stick it on the stack, otherwise
 *  put out whatever was before the repeated character and 
 *  then put out a repeat record.
 */
		state = NONE;
		if (repeat < MINREPEAT+1) {
		    toosmall++;
		    while (repeat--)
			 push(last, out);
		} else {
		    flush(out);		/* flush the non-repeating record
					 * before we got to the repetitive
					 * character */
		    Repeater(last, repeat, out);
					/* write out the repeater record */
		    repeat = 0;
		}
		push(this, out);
	    }
	} else {
/*
 *  We're just bumbling along seeing different characters 
 *  all the time.  If we see the same character as last time,
 *  then we change our state to repeating and take that
 *  last character off of the stack.  Otherwise we just
 *  carry on pushing characters onto the stack.  When the
 *  stack fills, then it will be emptied as non-repeating
 *  records by flush().
 */
	    if (last == this) {
		state = REPEATING;
		pop();			/* don't put repeating chars on the
					 * stack, we did, so take it off! */
		repeat = 2;		/* this one and the last one! */
	    } else {
		push(this, out);
	    }
	}
	last = this;
	this = getc(in);
    }
/*
 *  Flush whatever is lying in the stack as non-repeating
 *  records.  Then put out any repeat record.
 */
    flush(out);
    Repeater(last, repeat, out);
/* 
 *  Print out the end of file record.
 */
    output_c(0, out);

    /* Pad to longword align the end of the file */
    padding = 0;
    while((ocount%4) != 0) {
      output_c(0, out);
      ++padding;
    }

    /* Print on 2nd pass if necessary */
    if ((output_enable) && (padding))
      printf("  Image padded by %d bytes\n", padding);

/*
 *  Now tell the user what happened.
 */
    if (output_enable) {
      compression = (float)(ccount - ocount) / ((float)ccount) * 100;
      fprintf(stderr, "...Compression Statistics:\n");
      fprintf(stderr, "\tinput char count = %d,", ccount);
      fprintf(stderr, "output char count = %d,\n", ocount);
      fprintf(stderr, "\tCompression = %2.2f%%\n", compression);
      fprintf(stderr, "\trepeating records = %d,", rrecords);
      fprintf(stderr, "non-repeating records = %d\n", nrecords);
      fprintf(stderr, "\trepeated strings less than minimum (%d) = %d\n\n",
	      MINREPEAT, toosmall);
    }
    /*
     *  Return the total bytes written to the caller.
     */
    return ocount;
  }


/*
 * Control output of characters and compute
 * checksum and size.
 */
static void output_c(int c, FILE* f)
{
  ocount++;
  if (output_enable)
    fputc(c,f);
  else
    COMPUTE_CHECKSUM(c, head.romh.V0.checksum)
}
