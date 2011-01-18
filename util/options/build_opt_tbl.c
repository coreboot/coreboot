/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman (ebiederm@xmission.com)
 * Copyright (C) 2007-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#define UTIL_BUILD_OPTION_TABLE
#include "../../src/include/pc80/mc146818rtc.h"
#include "../../src/include/boot/coreboot_tables.h"

#define CMOS_IMAGE_BUFFER_SIZE 256
#define INPUT_LINE_MAX 256
#define MAX_VALUE_BYTE_LENGTH 64

#define TMPFILE_LEN 25600
#define TMPFILE_TEMPLATE "/build_opt_tbl_XXXXXX"

static unsigned char cmos_table[4096];

/* This array is used to isolate bits that are to be changed in a byte */
static unsigned char clip[9]={0,1,3,7,0x0f,0x1f,0x3f,0x7f,0xff};

#ifdef WIN32
#include <fcntl.h>
char *mkstemp(char* name)
{
	static char val='0';
	char *c=name;
	while (*c!='X') c++;
	*c=val++;
	return open(name,O_CREAT | O_RDWR);
}
#define UNLINK_IF_NECESSARY(x) unlink(x)
#else
#define UNLINK_IF_NECESSARY(x)
#endif

/**
 * This routine loops through the entried and tests if any of the fields
 * overlap.
 * If there is an overlap, the routine exits, otherwise it returns.
 *
 * @param entry_start memory pointer to the start of the entries.
 * @param entry_end   memory pointer to the byte past the entries.
 */
static void test_for_entry_overlaps(void *entry_start, void *entry_end)
{
	int ptr;
	char *cptr;
	int buffer_bit_size;
	int offset;
	int byte;
	int byte_length;
        struct cmos_entries *ce;
	unsigned char test[CMOS_IMAGE_BUFFER_SIZE];
	unsigned char set;

	/* calculate the size of the cmos buffer in bits */
	buffer_bit_size=(CMOS_IMAGE_BUFFER_SIZE*8);
	/* clear the temporary test buffer */
	for(ptr=0; ptr < CMOS_IMAGE_BUFFER_SIZE; ptr++)
		test[ptr]=0;

	/* loop through each entry in the table testing for errors */
	for(cptr = entry_start; cptr < (char *)entry_end; cptr += ce->size) {
		ce=(struct cmos_entries *)cptr;
		/* test if entry goes past the end of the buffer */
		if((int)(ce->bit+ce->length) > buffer_bit_size) {
			printf("Error - Entry %s start bit + length must be less than %d\n",
				ce->name,buffer_bit_size);
			exit(1);
		}
		byte=ce->bit/8;
		offset=ce->bit%8;
		byte_length=ce->length/8;
		if(byte_length) {	/* entry is 8 bits long or more */
			if(offset) { /* if 8 bits or more long, it must be byte aligned */
				printf("Error - Entry %s length over 8 must be byte aligned\n",
					ce->name);
				exit(1);
			}
			/* test if entries 8 or more in length are even bytes */
			if(ce->length%8){
                                printf("Error - Entry %s length over 8 must be a multiple of 8\n",
                                        ce->name);
                                exit(1);
                        }
			/* test if any of the bits have been previously used */
			for(;byte_length;byte_length--,byte++) {
				if(test[byte]) {
					printf("Error - Entry %s uses same bits previously used\n",
						ce->name);
					exit(1);
	                        }
				test[byte]=clip[8]; /* set the bits defined in test */
			}
		} else {
			/* test if bits overlap byte boundaries */
			if((int)ce->length > (8-offset)) {
                                printf("Error - Entry %s length overlaps a byte boundry\n",
					ce->name);
                                exit(1);
                        }
			/* test for bits previously used */
			set=(clip[ce->length]<<offset);
			if(test[byte]&set) {
                        	printf("Error - Entry %s uses same bits previously used\n",
                                                ce->name);
                                exit(1);
                        }
                        test[byte]|=set;  /* set the bits defined in test */
		}
	}
	return;
}

/* This routine displays the usage options */
static void display_usage(char *name)
{
	printf("Usage: %s [--config filename]\n", name);
	printf("                       [--option filename]\n");
	printf("                       [--header filename]\n\n");
	printf("--config = Build the definitions table from the given file.\n");
	printf("--binary = Output a binary file with the definitions.\n");
	printf("--option = Output a C source file with the definitions.\n");
	printf("--header = Output a C header file with the definitions.\n");
	exit(1);
}

static void skip_spaces(char *line, char **ptr)
{
	if (!isspace(**ptr)) {
		printf("Error missing whitespace in line\n%s\n", line);
		exit(1);
	}
	while(isspace(**ptr)) {
		(*ptr)++;
	}
	return;
}

static unsigned long get_number(char *line, char **ptr, int base)
{
	unsigned long value;
	char *ptr2;
	value = strtoul(*ptr, &ptr2, base);
	if (ptr2 == *ptr) {
		printf("Error missing digits at: \n%s\n in line:\n%s\n",
			*ptr, line);
		exit(1);
	}
	*ptr = ptr2;
	return value;
}

static int is_ident_digit(int c)
{
	int result;
	switch(c) {
	case '0':	case '1':	case '2':	case '3':
	case '4':	case '5':	case '6':	case '7':
	case '8':	case '9':
		result = 1;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

static int is_ident_nondigit(int c)
{
	int result;
	switch(c) {
	case 'A':	case 'B':	case 'C':	case 'D':
	case 'E':	case 'F':	case 'G':	case 'H':
	case 'I':	case 'J':	case 'K':	case 'L':
	case 'M':	case 'N':	case 'O':	case 'P':
	case 'Q':	case 'R':	case 'S':	case 'T':
	case 'U':	case 'V':	case 'W':	case 'X':
	case 'Y':	case 'Z':
	case 'a':	case 'b':	case 'c':	case 'd':
	case 'e':	case 'f':	case 'g':	case 'h':
	case 'i':	case 'j':	case 'k':	case 'l':
	case 'm':	case 'n':	case 'o':	case 'p':
	case 'q':	case 'r':	case 's':	case 't':
	case 'u':	case 'v':	case 'w':	case 'x':
	case 'y':	case 'z':
	case '_':
		result = 1;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

static int is_ident(char *str)
{
	int result;
	int ch;
	ch = *str;
	result = 0;
	if (is_ident_nondigit(ch)) {
		do {
			str++;
			ch = *str;
		} while(ch && (is_ident_nondigit(ch) || (is_ident_digit(ch))));
		result = (ch == '\0');
	}
	return result;
}

/**
 * This routine builds the cmos definition table from the cmos layout file
 *
 * The input comes from the configuration file which contains two parts
 * entries and enumerations.  Each section is started with the key words
 * entries and enumerations.  Records then follow in their respective
 * formats.
 *
 * The output of this program is the cmos definitions table.  It is stored
 * in the cmos_table array. If this module is called, and the global
 * table_file has been implimented by the user, the table is also written
 * to the specified file.
 *
 * This program exits with a return code of 1 on error.  It returns 0 on
 * successful completion
 */
int main(int argc, char **argv)
{
	int i;
	char *config=0;
	char *binary=0;
	char *option=0;
	char *header=0;
	FILE *fp;
	int tempfile;
	char tempfilename[TMPFILE_LEN];
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	struct cmos_enums *c_enums, *c_enums_start;
	struct cmos_checksum *cs, *new_cs;
	char line[INPUT_LINE_MAX];
	unsigned char uc;
	int entry_mode=0;
	int enum_mode=0;
	int checksum_mode=0;
	int cnt;
	char *cptr;
	void *entry_start, *entry_end;
	int entries_length;
	int enum_length;
	int len;
	char buf[16];

        for(i=1;i<argc;i++) {
                if(argv[i][0]!='-') {
                        display_usage(argv[0]);
                }
                switch(argv[i][1]) {
                        case '-':       /* data is requested from a file */
                                switch(argv[i][2]) {
                                        case 'c':  /* use a configuration file */
                                                if(strcmp(&argv[i][2],"config")) {
                                                        display_usage(argv[0]);
                                                }
                                                config=argv[++i];
                                                break;
                                        case 'b':  /* Emit a binary file */
                                                if(strcmp(&argv[i][2],"binary")) {
                                                        display_usage(argv[0]);
                                                }
                                                binary=argv[++i];
                                                break;
                                        case 'o':  /* use a cmos definitions table file */
                                                if(strcmp(&argv[i][2],"option")) {
                                                        display_usage(argv[0]);
                                                }
                                                option=argv[++i];
                                                break;
					case 'h': /* Output a header file */
						if (strcmp(&argv[i][2], "header") != 0) {
							display_usage(argv[0]);
						}
						header=argv[++i];
						break;
                                        default:
                                                display_usage(argv[0]);
                                                break;
                                }
                                break;

                        default:
                                display_usage(argv[0]);
                                break;
                }
        }


	/* Has the user specified a configuration file */
	if(config) {	/* if yes, open it */
		if((fp=fopen(config,"r"))==NULL){
			fprintf(stderr, "Error - Can not open config file %s\n",config);
			exit(1);  /* exit if it can not be opened */
		}
	}
	else {  /* no configuration file specified, so try the default */
		if((fp=fopen("cmos.layout","r"))==NULL){
			fprintf(stderr, "Error - Can not open cmos.layout\n");
			exit(1);  /* end of no configuration file is found */
		}
	}
	/* type cast a pointer, so we can us the structure */
	ct=(struct cmos_option_table*)cmos_table;
	/* start the table with the type signature */
	ct->tag = LB_TAG_CMOS_OPTION_TABLE;
	/* put in the header length */
	ct->header_length=sizeof(*ct);

	/* Get the entry records */
	ce=(struct cmos_entries*)(cmos_table+(ct->header_length));
	cptr = (char*)ce;
	for(;;){  /* this section loops through the entry records */
		if(fgets(line,INPUT_LINE_MAX,fp)==NULL)
			break; /* end if no more input */
		// FIXME mode should be a single enum.
		if(!entry_mode) {  /* skip input until the entries key word */
			if (strstr(line,"entries") != 0) {
				entry_mode=1;
				enum_mode=0;
				checksum_mode=0;
				continue;
			}
		} else {  /* Test if we are done with entries and starting enumerations */
			if (strstr(line,"enumerations") != 0){
				entry_mode=0;
				enum_mode=1;
				checksum_mode=0;
				break;
			}
			if (strstr(line, "checksums") != 0) {
				entry_mode=0;
				enum_mode=0;
				checksum_mode=1;
				break;
			}
		}

		/* skip commented and blank lines */
		if(line[0]=='#') continue;
		if(line[strspn(line," ")]=='\n') continue;
		/* scan in the input data */
		sscanf(line,"%d %d %c %d %s",
			&ce->bit,&ce->length,&uc,&ce->config_id,&ce->name[0]);
		ce->config=(int)uc;
		/* check bit and length ranges */
		if(ce->bit>(CMOS_IMAGE_BUFFER_SIZE*8)) {
                        fprintf(stderr, "Error - bit is to big in line \n%s\n",line);
                        exit(1);
                }
		if((ce->length>(MAX_VALUE_BYTE_LENGTH*8))&&(uc!='r')) {
			fprintf(stderr, "Error - Length is to long in line \n%s\n",line);
			exit(1);
		}
		if (!is_ident((char *)ce->name)) {
			fprintf(stderr,
				"Error - Name %s is an invalid identifier in line\n %s\n",
				ce->name, line);
			exit(1);
		}
		/* put in the record type */
		ce->tag=LB_TAG_OPTION;
		/* calculate and save the record length */
		len=strlen((char *)ce->name)+1;
		/* make the record int aligned */
		if(len%4)
			len+=(4-(len%4));
		ce->size=sizeof(struct cmos_entries)-32+len;
		cptr = (char*)ce;
		cptr += ce->size;  /* increment to the next table position */
		ce = (struct cmos_entries*) cptr;
	}

	/* put the length of the entries into the header section */
	entries_length = (cptr - (char *)&cmos_table) - ct->header_length;

	/* compute the start of the enumerations section */
	entry_start = ((char*)&cmos_table) + ct->header_length;
	entry_end   = ((char *)entry_start) + entries_length;
	c_enums_start = c_enums = (struct cmos_enums*)entry_end;
  	/* test for overlaps in the entry records */
	test_for_entry_overlaps(entry_start, entry_end);

	for(;enum_mode;){ /* loop to build the enumerations section */
		long ptr;
		if(fgets(line,INPUT_LINE_MAX,fp)==NULL)
			break; /* go till end of input */

		if (strstr(line, "checksums") != 0) {
			enum_mode=0;
			checksum_mode=1;
			break;
		}

		/* skip commented and blank lines */
		if(line[0]=='#') continue;
		if(line[strspn(line," ")]=='\n') continue;

		/* scan in the data */
		for(ptr=0;(line[ptr]==' ')||(line[ptr]=='\t');ptr++);
		c_enums->config_id=strtol(&line[ptr],(char**)NULL,10);
		for(;(line[ptr]!=' ')&&(line[ptr]!='\t');ptr++);
		for(;(line[ptr]==' ')||(line[ptr]=='\t');ptr++);
		c_enums->value=strtol(&line[ptr],(char**)NULL,10);
		for(;(line[ptr]!=' ')&&(line[ptr]!='\t');ptr++);
		for(;(line[ptr]==' ')||(line[ptr]=='\t');ptr++);
		for(cnt=0;(line[ptr]!='\n')&&(cnt<31);ptr++,cnt++)
			c_enums->text[cnt]=line[ptr];
		c_enums->text[cnt]=0;

		/* make the record int aligned */
		cnt++;
		if(cnt%4)
			cnt+=4-(cnt%4);
		/* store the record length */
		c_enums->size=((char *)&c_enums->text[cnt]) - (char *)c_enums;
		/* store the record type */
		c_enums->tag=LB_TAG_OPTION_ENUM;
		/* increment to the next record */
		c_enums=(struct cmos_enums*)&c_enums->text[cnt];
	}
	/* save the enumerations length */
	enum_length= (char *)c_enums - (char *)c_enums_start;
	ct->size=ct->header_length+enum_length+entries_length;

	/* Get the checksum records */
	new_cs = (struct cmos_checksum *)(cmos_table+(ct->size));
	for(;checksum_mode;) { /* This section finds the checksums */
		char *ptr;
		if(fgets(line, INPUT_LINE_MAX,fp)==NULL)
			break; /* end if no more input */

		/* skip commented and blank lines */
		if (line[0]=='#') continue;
		if (line[strspn(line, " ")]=='\n') continue;
		if (memcmp(line, "checksum", 8) != 0) continue;

		/* We actually found a new cmos checksum entry */
		cs = new_cs;

		/* get the information */
		ptr = line + 8;
		skip_spaces(line, &ptr);
		cs->range_start = get_number(line, &ptr, 10);

		skip_spaces(line, &ptr);
		cs->range_end = get_number(line, &ptr, 10);

		skip_spaces(line, &ptr);
		cs->location = get_number(line, &ptr, 10);

		/* Make certain there are spaces until the end of the line */
		skip_spaces(line, &ptr);

		if ((cs->range_start%8) != 0) {
			fprintf(stderr, "Error - range start is not byte aligned in line\n%s\n", line);
			exit(1);
		}
		if (cs->range_start >= (CMOS_IMAGE_BUFFER_SIZE*8)) {
			fprintf(stderr, "Error - range start is to big in line\n%s\n", line);
			exit(1);
		}
		if ((cs->range_end%8) != 7) {
			fprintf(stderr, "Error - range end is not byte aligned in line\n%s\n", line);
			exit(1);
		}
		if ((cs->range_end) >= (CMOS_IMAGE_BUFFER_SIZE*8)) {
			fprintf(stderr, "Error - range end is to long in line\n%s\n", line);
			exit(1);
		}
		if ((cs->location%8) != 0) {
			fprintf(stderr, "Error - location is not byte aligned in line\n%s\n", line);
			exit(1);
		}
		if ((cs->location >= (CMOS_IMAGE_BUFFER_SIZE*8)) ||
			((cs->location + 16) > (CMOS_IMAGE_BUFFER_SIZE*8)))
		{
			fprintf(stderr, "Error - location is to big in line\n%s\n", line);
			exit(1);
		}

		cs->tag = LB_TAG_OPTION_CHECKSUM;
		cs->size = sizeof(*cs);
		cs->type = CHECKSUM_PCBIOS;

		cptr = (char *)cs;
		cptr += cs->size;
		new_cs = (struct cmos_checksum *)cptr;
	}
	ct->size += (cptr - (char *)(cmos_table + ct->size));
	fclose(fp);

	/* See if we want to output a C source file */
	if(option) {
		int err=0;
		snprintf(tempfilename, TMPFILE_LEN, "%s%s", dirname(strdup(option)), TMPFILE_TEMPLATE);
		tempfile = mkstemp(tempfilename);
		if(tempfile == -1) {
                        perror("Error - Could not create temporary file");
                        exit(1);
		}

		if((fp=fdopen(tempfile,"w"))==NULL){
			perror("Error - Could not open temporary file");
			unlink(tempfilename);
			exit(1);
		}

		/* write the header */
        	if(!fwrite("unsigned char option_table[] = {",1,32,fp)) {
        	        perror("Error - Could not write image file");
        	        fclose(fp);
			unlink(tempfilename);
        	        exit(1);
        	}
		/* write the array values */
		for(i=0; i<(int)(ct->size-1); i++) {
			if(!(i%10) && !err) err=!fwrite("\n\t",1,2,fp);
			sprintf(buf,"0x%02x,",cmos_table[i]);
			if(!err) err=!fwrite(buf,1,5,fp);
		}
		/* write the end */
		sprintf(buf,"0x%02x\n",cmos_table[i]);
		if(!err) err=!fwrite(buf,1,4,fp);
        	if(!fwrite("};\n",1,3,fp)) {
        	        perror("Error - Could not write image file");
        	        fclose(fp);
			unlink(tempfilename);
        	        exit(1);
        	}

        	fclose(fp);
		UNLINK_IF_NECESSARY(option);
		if (rename(tempfilename, option)) {
			fprintf(stderr, "Error - Could not write %s: ", option);
			perror(NULL);
			unlink(tempfilename);
			exit(1);
		}
	}

	/* See if we also want to output a binary file */
	if(binary) {
		int err=0;
		snprintf(tempfilename, TMPFILE_LEN, "%s%s", dirname(strdup(binary)), TMPFILE_TEMPLATE);
		tempfile = mkstemp(tempfilename);
		if(tempfile == -1) {
                        perror("Error - Could not create temporary file");
                        exit(1);
		}

		if((fp=fdopen(tempfile,"wb"))==NULL){
			perror("Error - Could not open temporary file");
			unlink(tempfilename);
			exit(1);
		}

		/* write the array values */
		if(!fwrite(cmos_table, (int)(ct->size-1), 1, fp)) {
        	        perror("Error - Could not write image file");
        	        fclose(fp);
			unlink(tempfilename);
        	        exit(1);
		}

        	fclose(fp);
		UNLINK_IF_NECESSARY(binary);
		if (rename(tempfilename, binary)) {
			fprintf(stderr, "Error - Could not write %s: ", binary);
			perror(NULL);
			unlink(tempfilename);
			exit(1);
		}
	}

	/* See if we also want to output a C header file */
	if (header) {
		struct cmos_option_table *hdr;
		struct lb_record *ptr, *end;

		snprintf(tempfilename, TMPFILE_LEN, "%s%s", dirname(strdup(header)), TMPFILE_TEMPLATE);
		tempfile = mkstemp(tempfilename);
		if(tempfile == -1) {
			perror("Error - Could not create temporary file");
			exit(1);
		}

		fp = fdopen(tempfile, "w");
		if (!fp) {
			perror("Error - Could not open temporary file");
			unlink(tempfilename);
			exit(1);
		}

		/* Get the cmos table header */
		hdr = (struct cmos_option_table *)cmos_table;
		/* Walk through the entry records */
		ptr = (struct lb_record *)(cmos_table + hdr->header_length);
		end = (struct lb_record *)(cmos_table + hdr->size);
		fprintf(fp, "/* This file is autogenerated.\n"
			    " * See mainboard's cmos.layout file.\n */\n\n"
			    "#ifndef __OPTION_TABLE_H\n#define __OPTION_TABLE_H\n\n");

		for(;ptr < end; ptr = (struct lb_record *)(((char *)ptr) + ptr->size)) {
			if (ptr->tag != LB_TAG_OPTION) {
				continue;
			}
			ce = (struct cmos_entries *)ptr;

			if (!is_ident((char *)ce->name)) {
				fprintf(stderr, "Invalid identifier: %s\n",
					ce->name);
				fclose(fp);
				unlink(tempfilename);
				exit(1);
			}
			fprintf(fp, "#define CMOS_VSTART_%s %d\n",
				ce->name, ce->bit);
			fprintf(fp, "#define CMOS_VLEN_%s %d\n",
				ce->name, ce->length);
		}

		if (cs != NULL) {
			fprintf(fp, "\n#define LB_CKS_RANGE_START %d\n", cs->range_start / 8);
			fprintf(fp, "#define LB_CKS_RANGE_END %d\n", cs->range_end / 8);
			fprintf(fp, "#define LB_CKS_LOC %d\n", cs->location / 8);
		} else {
			fprintf(stderr, "Error - No checksums defined.\n");
			fclose(fp);
			unlink(tempfilename);
			exit(1);
		}
		fprintf(fp, "\n#endif // __OPTION_TABLE_H\n");
		fclose(fp);

		UNLINK_IF_NECESSARY(header);
		if (rename(tempfilename, header)) {
			fprintf(stderr, "Error - Could not write %s: ", header);
			perror(NULL);
			unlink(tempfilename);
			exit(1);
		}
	}

	return 0;
}


