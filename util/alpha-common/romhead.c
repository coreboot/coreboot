
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

#ifndef LINT
static char *rcsid = "$Id$";
#endif

/*
 * $Log$
 * Revision 1.1  2001/03/22 21:26:32  rminnich
 * testing I HATE CVS
 *
 * Revision 1.1  2000/03/21 03:56:31  stepan
 * Check in current version which is a nearly a 2.2-16
 *
 * Revision 1.13  1995/02/27  19:22:33  fdh
 * Print header->romh.V0.destination as two longwords instead of
 * a single quadword.
 *
 * Revision 1.12  1995/02/27  15:35:26  fdh
 * Modified to not use 64 bit ints for portability.
 *
 * Revision 1.11  1995/02/25  05:18:13  fdh
 * Print 64bit fields as type long.
 *
 * Revision 1.10  1995/02/19  17:47:31  fdh
 * Modified an error message.
 *
 * Revision 1.9  1995/02/16  20:46:31  fdh
 * Print out decimal representation of image checksum too.
 *
 * Revision 1.8  1995/02/10  19:08:55  fdh
 * Minor fixup.
 *
 * Revision 1.7  1995/02/10  02:18:11  fdh
 * Moved compute_romh_chksum() to this file.
 *
 * Revision 1.5  1995/02/07  04:57:30  fdh
 * Modified fwid_match_i().
 * Added ostype_array[] and routines for accessing it.
 *
 * Revision 1.4  1995/02/07  01:04:05  fdh
 * Removed some unnecessary definitions.
 *
 * Revision 1.3  1995/02/06  02:44:42  fdh
 * Added fwid_match(), fwid_match_i(), and fwid_dump() routines.
 *
 * Revision 1.2  1995/02/05  01:59:05  fdh
 * Added include files and definitions.
 *
 * Revision 1.1  1995/02/03  16:35:37  fdh
 * Initial revision
 *
 */

#include <stdlib.h>
#include "romhead.h"

char *fw_dbm_id[] = {FW_DBM_STRINGS};
char *fw_wnt_id[] = {FW_WNT_STRINGS};
char *fw_srm_id[] = {FW_SRM_STRINGS};

fw_id_t fwid_array[] = {
  { FW_DBM,  fw_dbm_id},
  { FW_WNT,  fw_wnt_id},
  { FW_SRM,  fw_srm_id},
  { 0, NULL}
};

char *ostype_dbm[] = {OS_DBM_STRINGS};
char *ostype_wnt[] = {OS_WNT_STRINGS};
char *ostype_vms[] = {OS_VMS_STRINGS};
char *ostype_osf[] = {OS_OSF_STRINGS};

ostype_t ostype_array[] = {
  { FW_OSTYPE_DBM, FW_DBM,  ostype_dbm},
  { FW_OSTYPE_WNT, FW_WNT,  ostype_wnt},
  { FW_OSTYPE_VMS, FW_SRM,  ostype_vms},
  { FW_OSTYPE_OSF, FW_SRM,  ostype_osf},
  { 0, 0, NULL}
};

#define MAXSTRING 80
char strbuf1[MAXSTRING+1];
char strbuf2[MAXSTRING+1];

fw_id_t *fwid_match(char *arg)
{
  fw_id_t *ptr;
  int i, k;

  ptr = fwid_array;
  strncpy(strbuf1, arg, MAXSTRING);
  strbuf1[MAXSTRING+1] = '\0';
  k = 0;
  while ((strbuf1[k] = tolower(strbuf1[k])) != '\0') ++k;

  while (ptr->id_string) {
    i = 0;
    while (ptr->id_string[i]) {
      strncpy(strbuf2, ptr->id_string[i], MAXSTRING);
      strbuf2[MAXSTRING+1] = '\0';
      k = 0;
      while ((strbuf2[k] = tolower(strbuf2[k])) != '\0') ++k;

      if (strcmp(strbuf1, strbuf2) == 0)
	return (ptr);

      ++i;
    }
    ++ptr;
  }
  return ((fw_id_t *)NULL);
}

fw_id_t *fwid_match_i(int fwid)
{
  fw_id_t *ptr;

  ptr = fwid_array;
  while (ptr->id_string) {
    if (fwid == ptr->firmware_id)
      return (ptr);

    ++ptr;
  }
  return ((fw_id_t *)NULL);
}

void fwid_dump(char *pre)
{
  fw_id_t *ptr;
  int i;

  ptr = fwid_array;
  while (ptr->id_string) {
    i = 0;
    printf("%s\"%d\"", pre, ptr->firmware_id);
    while (ptr->id_string[i])
      printf(" \"%s\"", ptr->id_string[i++]);
    printf("\n");
    ++ptr;
  }
}

ostype_t *ostype_match(char *arg)
{
  ostype_t *ptr;
  int i, k;

  ptr = ostype_array;
  strncpy(strbuf1, arg, MAXSTRING);
  strbuf1[MAXSTRING+1] = '\0';
  k = 0;
  while ((strbuf1[k] = tolower(strbuf1[k])) != '\0') ++k;

  while (ptr->id_string) {
    for (i=0; i<2; ++i) {
      strncpy(strbuf2, ptr->id_string[i], MAXSTRING);
      strbuf2[MAXSTRING+1] = '\0';
      k = 0;
      while ((strbuf2[k] = tolower(strbuf2[k])) != '\0') ++k;

      if (strcmp(strbuf1, strbuf2) == 0)
	return (ptr);
    }
    ++ptr;
  }
  return ((ostype_t *)NULL);
}

ostype_t *ostype_match_i(int ostype)
{
  ostype_t *ptr;

  ptr = ostype_array;
  while (ptr->id_string) {
    if (ostype == ptr->ostype)
      return (ptr);
    ++ptr;
  }
  return ((ostype_t *)NULL);
}

void ostype_dump(char *pre)
{
  ostype_t *ptr;
  int i;

  ptr = ostype_array;
  while (ptr->id_string) {
    printf("%s\"%d\"", pre, ptr->ostype);
    for (i=0; i<2; ++i)
      printf(" \"%s\"", ptr->id_string[i]);
    printf("\n");
    ++ptr;
  }
}

/*
 * Compute the header checksum.
 */
ui compute_romh_chksum(romheader_t *header)
{
  char *ptr;
  ui chksum = 0;

  ptr = (char *) header;
  while (ptr < (char *) &header->romh.hchecksum) {
    COMPUTE_CHECKSUM(*ptr,chksum);
    ++ptr;
  }
  return (chksum);
}

/* Prints out the ROM header pointed to by argument. 
 * Returns TRUE if header is valid; FALSE otherwise.
 */
int dumpHeader(romheader_t *header)
{

  int i, hver;
  ui hchksum;
  fw_id_t *fwid_ptr;
   
  printf("  Header Size......... %d bytes\n", header->romh.V0.hsize);
  printf("  Image Checksum...... 0x%04x (%d)\n",
	 header->romh.V0.checksum, header->romh.V0.checksum);
  printf("  Image Size (Uncomp). %d (%d KB)\n", header->romh.V0.size, header->romh.V0.size/1024);
  printf("  Compression Type.... %d\n", header->romh.V0.decomp);
  printf("  Image Destination... 0x%08x%08x\n",
	 header->romh.V0.destination.high,
	 header->romh.V0.destination.low);

  if ((hver = ROMH_VERSION(header)) > 0)  /* Version > 0 */
     {	
	printf("  Header Version...... %d\n", (ui) hver);

	fwid_ptr = fwid_match_i(header->romh.V1.fw_id);
	printf("  Firmware ID......... %d - %s\n",
	       (ui) header->romh.V1.fw_id,
	       fwid_ptr == NULL ? "Unknown ID Type." : fwid_ptr->id_string[0]);

	printf("  ROM Image Size...... %d (%d KB)\n", header->romh.V1.rimage_size, 
	       header->romh.V1.rimage_size/1024);

	printf("  Firmware ID (Opt.).. %08x%08x  ",
	       header->romh.V1.fwoptid.id_S.high,
	       header->romh.V1.fwoptid.id_S.low);
	for (i = 0; i < 8; i++)
	   if (isprint(header->romh.V1.fwoptid.id[i]))
	     printf ("%c", header->romh.V1.fwoptid.id[i]);
	   else printf (".");
	printf("\n  Header Checksum..... 0x%04x", header->romh.hchecksum);
        hchksum = compute_romh_chksum(header);
       if (hchksum != header->romh.hchecksum) {
	 printf("\nERROR: Bad ROM header checksum. 0x%04x\n", hchksum);
	 return (FALSE);
       }
       else
	  printf ("\n");	
  }      
  return (TRUE);  
}
