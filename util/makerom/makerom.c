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
**      command parsing code.
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
static char *RCSid = 
  "$Id$";
#endif

/*
 * $Log$
 * Revision 1.1  2001/03/22 21:26:32  rminnich
 * testing I HATE CVS
 *
 * Revision 1.1  2000/03/21 03:56:31  stepan
 * Check in current version which is a nearly a 2.2-16
 *
 * Revision 1.18  1995/02/27  15:34:24  fdh
 * Modified to not use 64 bit ints for portability.
 *
 * Revision 1.17  1995/02/25  05:14:40  fdh
 * Clean up handling of options with 64bit quantities.
 * Updated usage().
 *
 * Revision 1.16  1995/02/16  20:45:46  fdh
 * Inform user when image is padded.
 *
 * Revision 1.15  1995/02/10  15:44:47  fdh
 * Modified image size to reflect the size after any necessary
 * padding is added to align to a 32 bit boundary.
 *
 * Revision 1.14  1995/02/10  02:10:40  fdh
 * Modified to use the COMPUTE_CHECKSUM Macro.
 *
 * Revision 1.13  1995/02/09  23:38:40  fdh
 * Corrected char sign extension problem when computing
 * checksums.
 * Print out standard header summary.
 *
 * Revision 1.12  1995/02/08  01:23:15  fdh
 * Pad to longword align the ROM image.
 * ROTATE_RIGHT macro was moved to romhead.h.
 *
 * Revision 1.11  1995/02/07  23:16:06  fdh
 * Modified the -x and -f options to work with 32 bit OS's.
 *
 * Revision 1.10  1995/02/07  04:51:37  fdh
 * Modified to work with a change to fwid_match_i().
 *
 * Revision 1.9  1995/02/07  01:00:22  fdh
 * Corrected header size computation.
 * Corrected comments.
 *
 * Revision 1.8  1995/02/06  02:42:36  fdh
 * Moved some code to library file routines
 * fwid_match(), fwid_match_i(), and fwid_dump().
 *
 * Revision 1.7  1995/02/05  02:01:10  fdh
 * Modified to accept the latest romheader definition.
 *
 * Revision 1.6  1995/02/02  20:19:28  fdh
 * Modified to use ROM header version 1.
 * Computes header and image checksums.
 * Added -s -x and -f switches to specify user
 * optional revision data in the header.
 * Added -i switch used to specify the firmware
 * ID type as either a registered string, registered
 * number, or unregistered number.
 *
 * Revision 1.5  1995/01/23  21:23:46  fdh
 * Modified to use common include file "romhead.h"
 *
 * Revision 1.4  1994/07/01  14:23:56  rusling
 * Fixed up NT warnings.
 *
 * Revision 1.3  1994/03/03  16:26:09  rusling
 * Fixed help text.
 *
 * Revision 1.6  1994/02/04  11:51:35  rusling
 * Ported to Alpha WNT.
 *
 * Revision 1.5  1994/02/04  11:39:03  rusling
 * Oops.
 *
 * Revision 1.3  1994/02/04  10:17:47  rusling
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
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000


#define _SEEN(o) seen[o-'a']
#define _OPTION(o) (_SEEN(o) == TRUE)

romheader_t head;

typedef struct fileinfo {
    struct fileinfo *next;		/* next file in the list */
    char *file;
    struct {
      ui low;
      ui high;
    } load;				/* load point for this file */
    int32 compress;			/* Should we compress this file? */
    int32 size;				/* size of the file */
    int32 padding;			/* padding required to longword align at the end of the file */
    int32 outputfile;			/* is this an output file? */
    fw_id_t *fwid_ptr;                  /* Points to a predefined table */
    int fwid_alt;                       /* Used when user specified */
    union int_char {
      char ch[8];
      struct {
	ui low;
	ui high;
      } data;
    } fwid_optional;
} fileinfo_t;
fileinfo_t *files = NULL;
fileinfo_t *last = NULL;
int32 fcount = 0;


int output_enable = 0;
/*
 *  Global data (all names preceded by 'rhdr_g'.
 */
#define SEEN_SIZE 100
char seen[SEEN_SIZE];			/* upper and lower case */

/*
 *  Forward routine descriptions.
 */
fileinfo_t *allocate_fileinfo();
int main(int argc, char **argv);
void process_file(fileinfo_t *ifile, FILE *out);
void usage();
void read_file(char *filename);
void write_file(char *filename);
int32 fsize(FILE *file);
/*
 *  External routines.
 */
extern int compress(FILE *in, FILE *out);

fileinfo_t *allocate_fileinfo()
{
    fileinfo_t *file;

    file = (fileinfo_t *) malloc(sizeof(fileinfo_t));
    if (file == NULL) {
	fprintf(stderr, "ERROR: failed to allocate memory\n");
	exit(0);
    } else {
	file->load.high = 0;
	file->load.low = 0;
	file->size = 0;
	file->compress = 0;		/* no compression */
	file->file = NULL;
	file->outputfile = FALSE;
    }
    fcount++;
/*
 * return the address of the fileinfo structure to the
 * caller.
 */
    return file;
}

int main(int argc, char **argv)
{
    char *arg, option;
    int i;
    fileinfo_t *now, *output;
    FILE *out;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;
/*
 * Allocate at least one file description block.
 */
    now = last = files = allocate_fileinfo();
    now->fwid_ptr = NULL;

    if (argc < 2) {
      usage();
      exit(1);
    }
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i = 1; i < argc; i++) {
	arg = argv[i];
	if (*arg == '-') {

/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		switch (option) {
		    case 'h': 
		    case 'H': 
			usage();
			exit(1);
		    case 'v': 		/* verbose */
		    case 'V': 
			_SEEN(tolower(option)) = TRUE;
			break;
		    case 'C': 
		    case 'c': 
/*
 *  The -C,-c option means "compress the image".  The compression algorithm
 *  is a simple removal of repeating bytes.  The decompression algorithm is
 *  implemented in the SROM code itself.
 */
			_SEEN(tolower(option)) = TRUE;
			now->compress = TRUE;
			break;
		    case 'O': 
		    case 'o': 
/*
 *  This file is marked as an output file.
 */
			if (_SEEN(tolower(option))) {
			    fprintf(stderr, 
			      "ERROR: two output file names given\n");
			    exit(0);
			} else {
			    _SEEN(tolower(option)) = TRUE;
			    now->outputfile = TRUE;
			}
		    case 'L': 
		    case 'l': 
/*
 *  This is a special one, the -l option is followed (immediately)
 *  by the address where the file should be loaded into memory.
 */
			_SEEN(tolower(option)) = TRUE;
			{
			  int i, j;
			  char strbuf[16+1];

			  sscanf(arg, "%16s", strbuf);
			  i = strlen(strbuf);

			  j = ((i-8)>0)?(i-8):0;
			  now->load.low = strtoul(&strbuf[j], NULL, 16);
			  strbuf[j] = '\0';
			  now->load.high = strtoul(strbuf, NULL, 16);
			}
			arg = arg + strlen(arg);
			break;

		    case 'I': 
		    case 'i': 
/*
 *  This is a special one, the -i option is followed (immediately)
 *  by a firmware type identifier.
 */
			_SEEN(tolower(option)) = TRUE;

			if ((now->fwid_ptr = fwid_match(arg)) == NULL) {
			  now->fwid_alt = atoi(arg);
			  now->fwid_ptr = fwid_match_i(now->fwid_alt);
			}

			arg = arg + strlen(arg);
			break;

		    case 'X': 
		    case 'x': 
/*
 *  This is a special one, the -x option is followed (immediately)
 *  by a Hex value, truncated to 8 digits.
 */
			_SEEN(tolower(option)) = TRUE;
			{
			  int i, j;
			  char strbuf[16+1];

			  sscanf(arg, "%16s", strbuf);
			  i = strlen(strbuf);

			  j = ((i-8)>0)?(i-8):0;
			  now->fwid_optional.data.low = strtoul(&strbuf[j], NULL, 16);
			  strbuf[j] = '\0';
			  now->fwid_optional.data.high = strtoul(strbuf, NULL, 16);
			}
			arg = arg + strlen(arg);
			break;

		    case 'S': 
		    case 's': 
/*
 *  This is a special one, the -s option is followed (immediately)
 *  by a 8 character string.
 */
			_SEEN(tolower(option)) = TRUE;
			strncpy(now->fwid_optional.ch, arg, 8);
			arg = arg + strlen(arg);
			break;

		    case 'F': 
		    case 'f': 
/*
 *  This is a special one, the -f option is followed (immediately)
 *  by a filename from which the first 8 bytes will be read and
 *  placed into the optional firmware ID field of the ROM header.
 */
			_SEEN(tolower(option)) = TRUE;
			{
			  FILE *inf;
			  int i, j;
			  int ch;
			  int quote;
			  char strbuf[16+1];

			  inf = fopen(arg, "rb");
			  if (inf == NULL) {
			    fprintf(stderr, "ERROR: failed to open input file %s\n", 
				    arg);
			    exit(0);
			  }
			  if (fscanf(inf, "%16s", strbuf) && isxdigit(strbuf[0])) {
			    i = strlen(strbuf);

			    j = ((i-8)>0)?(i-8):0;
			    now->fwid_optional.data.low = strtoul(&strbuf[j], NULL, 16);
			    strbuf[j] = '\0';
			    now->fwid_optional.data.high = strtoul(strbuf, NULL, 16);
			  }
			  else {
			    rewind(inf);
			    /* Ignore leading white space */
			    while((ch = getc(inf)) != EOF) {
			      if (isspace(ch)) continue;
			      break;
			    }

			    quote = FALSE;
			    j=0;
			    /* Open quote if necessary */
			    /* Otherwise store the character. */
			    if (ch == '"') quote = TRUE;
			    else
			      now->fwid_optional.ch[j++] = ch;

			    /* Now store up to 7 more characters */
			    while((ch = getc(inf)) != EOF) {
			      now->fwid_optional.ch[j++] = ch;
			      if (j > 8) break;
			      if (quote && (ch == '"')) break;
			    }

			    /* Pad zero to 8 chacters if necessary */
			    for (j; j<8; ++j)
			      now->fwid_optional.ch[j] = '\0';
			  }
			  fclose(inf);
			}
			arg = arg + strlen(arg);
			break;

/*
 *  And now print usage and exit if we see an unrecognized switch.
 */
		      default: 
			usage();
			exit(0);
			break;
		      }
	      }
	} else {
	  fileinfo_t *new;
	  /*
	   * For each new filename supplied, create another file description
	   * block and put it into the file list.  The last file is the output
	   * file.
	   */

	    now->file = arg;
	    new = allocate_fileinfo();
	    new->next = NULL;
	    now->next = new;
	    last = now;
	    now = new;
	    now->fwid_ptr = NULL;
	}
    }
/*
 *  As a result of the algorithm that I've used for parsing the
 *  filenames and arguments into file information blocks.  "last"
 *  points at an entry that has a pointer to an invalid (empty)
 *  fileinfo block.  Decrement the file count also.
 */

    last->next = NULL;
    fcount--;
/*
 *  If the world wants to know, then tell it.
 */
    if _OPTION('v') {
	fprintf(stderr, "makerom [V2.0]\n");
    }
/*
 *  Check that at least two files have been specified.
 */
    if (fcount < 2) {
	fprintf(stderr, "ERROR: insufficient filenames supplied\n");
	exit(0);
    }

/*
 *  find the output file and open it.
 */
    output = files;
    while (output->next != NULL) {
	if (output->outputfile)
	    break;
	output = output->next;
    }

    if (!(output->outputfile)) {
	fprintf(stderr, "ERROR: no output file specified\n");
	exit(0);
    }
    out = fopen(output->file, "wb");
    if (out == NULL) {
	fprintf(stderr, "ERROR: failed to open output file %s\n", 
	  output->file);
	exit(0);
    }
/*
 *  Now, for every file, open it, add a header and output the result
 *  to the output file.
 */
    if _OPTION('v')
	fprintf(stderr, "...Output file is %s\n\n", last->file);
    now = files;
    while (now->next != NULL) {
      if (!(now->outputfile))
	process_file(now, out);
      now = now->next;
    }
    /*
     *  close the output file and return.
     */
    fclose(out);
    return 0;
}					/* end of main() */

void process_file(fileinfo_t *ifile, FILE *out)
{
    FILE *in;
    char *ptr;
    int32 i;
    int c;
/*
 *  Open the input file.  
 *  Get its size.
 *  Output its header.
 *  Output the input file's contents.
 */

    if _OPTION('v')
	fprintf(stderr, "...processing input file %s\n", ifile->file);

    in = fopen(ifile->file, "rb");
    if (in == NULL) {
	fprintf(stderr, "ERROR: failed to open input file %s\n", 
	  ifile->file);
	exit(0);
    }

    /* Load file to get it's size */
    ifile->size = fsize(in);

/*
 *  Start to build the header.
 */
    head.romh.V0.signature = ROM_H_SIGNATURE;	/* pattern for data path */
    head.romh.V0.csignature = (ui) ~ROM_H_SIGNATURE;	/* comp pattern for data path */
    head.romh.V0.decomp = ifile->compress;	/* decompression algorithm */
    head.romh.V0.hsize =                        /* header size */
      (ui) (((unsigned long) &head.romh.hchecksum
	     - (unsigned long) &head) + sizeof(head.romh.hchecksum));

    head.romh.V0.destination.high = ifile->load.high;
    head.romh.V0.destination.low = ifile->load.low;

    head.romh.V1.hversion = (char) ROM_H_REVISION;
    head.romh.V0.checksum = 0;   /* Initialize ROM image checksum */
    head.romh.hchecksum = 0;  /* Initialize header checksum */
    head.romh.V1.rimage_size = 0;  /* Initialize ROM image size */

    /* Set FW ID optional field */
    head.romh.V1.fwoptid.id_S.high = ifile->fwid_optional.data.high;
    head.romh.V1.fwoptid.id_S.low = ifile->fwid_optional.data.low;

    head.romh.V1.fw_id =
      (char) ((ifile->fwid_ptr != NULL) ? ifile->fwid_ptr->firmware_id : ifile->fwid_alt);

    /*
     *  Make a pass on the input file to compute the ROM image checksum.
     */
    output_enable = FALSE;    /* <-FALSE, Compute checksum only */
    if (ifile->compress) {         /*  Do we compress this file? */
      head.romh.V1.rimage_size = compress(in, out);
      head.romh.V0.size = ifile->size; /* Uncompressed size */
    }
    else {
      for (i = 0; i < ifile->size; ++i) {
	if ((c = getc(in)) == EOF)
	  abort();
	COMPUTE_CHECKSUM(c,head.romh.V0.checksum);
	++head.romh.V1.rimage_size;        /* Write out the ROM image size */
      }
      ifile->padding = 0;
      while(((ifile->size+ifile->padding)%4) != 0) {
	++ifile->padding;
	COMPUTE_CHECKSUM('\0',head.romh.V0.checksum);
	++head.romh.V1.rimage_size;        /* Write out the ROM image size */
      }

      if (ifile->padding)
	printf("  Image padded by %d bytes\n", ifile->padding);

      head.romh.V0.size = head.romh.V1.rimage_size; /* Same size, No compression */
    }

    rewind(in);  /* Back to the start of the input file. */

/*
 * Compute the header checksum.
 */
    head.romh.hchecksum = compute_romh_chksum(&head);

/*
 * Now write out the header.
 */
    ptr = (char *) &head;
    for (i = 0; i < (int) head.romh.V0.hsize; i++)
	fputc(*ptr++, out);

/*
 * print out header summary.
 */
    if _OPTION('v') dumpHeader(&head);

/*
 *  Second pass.  The checksums have been computed and the header
 *  has already been written out so produce the output file this time.
 */
    output_enable = TRUE;  /* <-TRUE, Output file this time */
    if (ifile->compress)          /*  Do we compress this file? */
	compress(in, out);
    else {
/*
 *  Now read in the input file again and output it.
 */
	for (i = 0; i < ifile->size; ++i) {
	    if ((c = getc(in)) == EOF)
		abort();
	    fputc(c, out);
	}
	/* Pad to longword align the end of the file */
	for (i=0; i<ifile->padding; ++i)
	  fputc('\0', out);
    }

/*
 *  Finally, close the input file.
 */
    fclose(in);
}
void usage()
{
    printf("\nmakerom [options] [<input-file-options>]<input-file>...-o <output-file>\n\n");
    printf("Builds a rom image by adding headers to the input files.\n");
    printf("The input files and headers and concatenated and written\n");
    printf("to the output file.  Optionally, those input files may also be\n");
    printf("compressed\n\n");
    printf("Where each input file is preceded by options:\n");
    printf("\t-l,L<address> = load address in memory of the image\n");
    printf("\t-c,C = compress this file (the SROM code decompresses\n");
    printf("\t\tthe image).  The default is no compression\n");
    printf("\t-x,X<hex value> = Sets optional firmware ID field.\n");
    printf("\t-s,S<8 char string> = Sets optional firmware ID field.\n");
    printf("\t-f,F<file> = Sets optional firmware ID field.\n");
    printf("\t\tWhere the file contains either a hex value or\n");
    printf("\t\ta \"quoted\" ASCII string.\n\n");
    printf("\t-i,I<fw_id> = Firmware ID string or number.\n");
    printf("\t\tThe following pre-defined values can also be used...\n");

    fwid_dump("\t\t ");

    printf("\nOptional flags are:\n");
    printf("\t-v,V = verbose mode\n");
    printf("\t-h,H = print this help text\n");
    printf("\nExample:\n");
    printf("\tmakerom -v -l200000 eb66_rom.img -o eb66_rom.rom\n");

}


int32 fsize(FILE *fp)
{
    int c;
    int32 size = 0;

    fseek(fp, 0, 0);
    while ((c = getc(fp)) != EOF)
	size++;
    fseek(fp, 0, 0);
    return size;
}
