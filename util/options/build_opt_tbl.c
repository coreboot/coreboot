#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <string.h>
#include "../../src/include/boot/linuxbios_tables.h"

#define CMOS_IMAGE_BUFFER_SIZE 128
#define INPUT_LINE_MAX 256
#define MAX_VALUE_BYTE_LENGTH 64


static unsigned char cmos_table[4096];

/* This array is used to isolate bits that are to be changed in a byte */
static unsigned char clip[9]={0,1,3,7,0x0f,0x1f,0x3f,0x7f,0xff};


/* This routine loops through the entried and tests if any of the fields overlap
	input entry_start = the memory pointer to the start of the entries.
	      entry_end = the byte past the entries.
	output  none
		if there is an overlap, the routine exits, other wise it returns.
*/
void test_for_entry_overlaps(int entry_start,int entry_end)
{
	int ptr;
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
	for(ptr=0;ptr<CMOS_IMAGE_BUFFER_SIZE;ptr++)
		test[ptr]=0;
	/* loop through each entry in the table testing for errors */
	for(ptr=entry_start;ptr<entry_end;ptr+=ce->size) {
		ce=(struct cmos_entries *)ptr;
		/* test if entry goes past the end of the buffer */
		if((ce->bit+ce->length)>buffer_bit_size) {
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
			if(ce->length>(8-offset)) {
                                printf("Error - Entry %s length overlaps a byte boundry\n",                                        ce->name);
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
void display_usage(void)
{
        printf("Usage build_opt_table [-b] [--option filename]\n");
        printf("                [--config filename]\n");
        printf("b = build option_table.c\n");
        printf("--option = name of option table output file\n");
        printf("--config = build the definitions table from the given file\n");
        exit(1);
}



/* This routine builds the cmos definition table from the cmos configuration file
	input The input comes from the configuration file which contains two parts
		entries and enumerations. Each section is started with the key words
		entries and enumerations.  Records then follow in their respective 
		formats.
	output The output of this program is the cmos definitions table.  It is stored
		in the cmos_table array. If this module is called, and the global 
		table_file has been implimented by the user, the table is also written
		to the specified file.
		This program exits on and error.  It returns a 1 on successful 
		completion
*/
int main(int argc, char **argv)
{
	int i;
	char *config=0;
	char *option=0;
	FILE *fp;
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	struct cmos_enums *c_enums, *c_enums_start;
	unsigned char line[INPUT_LINE_MAX];
	unsigned char uc;
	int entry_mode=0;
	int enum_mode=0;
	int ptr,cnt;
	char *cptr;
	int offset,entry_start;
	int entries_length;
	int enum_length;
	int len;
	unsigned char buf[16];

        for(i=1;i<argc;i++) {
                if(argv[i][0]!='-') {
                        display_usage();
                }
                switch(argv[i][1]) {
                        case 'b':       /* build the table */
                                break;
                        case '-':       /* data is requested from a file */
                                switch(argv[i][2]) {
                                        case 'c':  /* use a configuration file */
                                                if(strcmp(&argv[i][2],"config")) {
                                                        display_usage();
                                                }
                                                config=argv[++i];
                                                break;
                                        case 'o':  /* use a cmos definitions table file */
                                                if(strcmp(&argv[i][2],"option")) {
                                                        display_usage();
                                                }
                                                option=argv[++i];
                                                break;
                                        default:
                                                display_usage();
                                                break;
                                }
                                break;
                        default:
                                display_usage();
                                break;
                }
        }


	/* Has the user specified a configuration file */
	if(config) {	/* if yes, open it */
		if((fp=fopen(config,"r"))==NULL){
			printf("Error - Can not open config file %s\n",config);
			exit(1);  /* exit if it can not be opened */
		}
	}
	else {  /* no configuration file specified, so try the default */
		if((fp=fopen("cmos.conf","r"))==NULL){
			printf("Error - Can not open cmos.conf\n");
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
		if(!entry_mode) {  /* skip input until the entries key word */
			if((ptr=(int)strstr(line,"entries"))){
				entry_mode=1;
				continue;
			}
		}
		else{  /* Test if we are done with entries and starting enumerations */
			if((ptr=(int)strstr(line,"enumerations"))){
				entry_mode=0;
				enum_mode=1;
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
                        printf("Error - bit is to big in line \n%s\n",line);
                        exit(1);
                }
		if((ce->length>(MAX_VALUE_BYTE_LENGTH*8))&&(uc!='r')) {
			printf("Error - Length is to long in line \n%s\n",line);
			exit(1);
		}
		/* put in the record type */
		ce->tag=LB_TAG_OPTION;
		/* calculate and save the record length */
		len=strlen(ce->name)+1;
		/* make the record int aligned */
		if(len%4)
			len+=(4-(len%4));
		ce->size=sizeof(struct cmos_entries)-32+len;
		cptr = (char*)ce;
		cptr+=ce->size;  /* increment to the next table position */
		ce = (struct cmos_entries*) cptr;
	}

	/* put the length of the entries into the header section */
	entries_length=(int)cptr;
	entries_length-=(int)(cmos_table+ct->header_length);

	/* compute the start of the enumerations section */
	entry_start=(int)cmos_table;
	entry_start+=ct->header_length;
	offset=entry_start+entries_length;
	c_enums_start=c_enums=(struct cmos_enums*)offset;
  	/* test for overlaps in the entry records */
	test_for_entry_overlaps(entry_start,offset);

	for(;enum_mode;){ /* loop to build the enumerations section */
		if(fgets(line,INPUT_LINE_MAX,fp)==NULL) break; /* go till end of input */
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
		c_enums->size=((int)&c_enums->text[cnt])-(int)c_enums;
		/* store the record type */
		c_enums->tag=LB_TAG_OPTION_ENUM;
		/* increment to the next record */
		c_enums=(struct cmos_enums*)&c_enums->text[cnt];
	}
	/* save the enumerations length */
	enum_length=(int)c_enums-(int)c_enums_start;
	ct->size=ct->header_length+enum_length+entries_length;
	fclose(fp);

	/* test if an alternate file is to be created */
	if(option) {
		if((fp=fopen(option,"w"))==NULL){
                        printf("Error - Can not open %s\n",option);
                        exit(1);
		}
	}
	else {  /* no, so use the default option_table.c */
                if((fp=fopen("option_table.c","w"))==NULL){
                        printf("Error - Can not open option_table.c\n");
                        exit(1);
		}
	}
	/* write the header */
        if(!fwrite("unsigned char option_table[] = {",1,32,fp)) {
                printf("Error - Could not write image file\n");
                fclose(fp);
                exit(1);
        }
	/* write the array values */
	for(i=0;i<(ct->size-1);i++) {
		if(!(i%10)) fwrite("\n\t",1,2,fp);
		sprintf(buf,"0x%02x,",cmos_table[i]);
		fwrite(buf,1,5,fp);
	}
	/* write the end */
	sprintf(buf,"0x%02x",cmos_table[i]);
	fwrite(buf,1,4,fp);
        if(!fwrite("};\n",1,3,fp)) {
                printf("Error - Could not write image file\n");
                fclose(fp);
                exit(1);
        }

        fclose(fp);
	return(0);
}


