/*
 * Copyright (c) 1996, by Steve Passe
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the developer may NOT be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * mptable.c
 */

#define VMAJOR			2
#define VMINOR			0
#define VDELTA			15

/*
 * this will cause the raw mp table to be dumped to /tmp/mpdump
 *
#define RAW_DUMP
 */

#define MP_SIG			0x5f504d5f	/* _MP_ */
#define EXTENDED_PROCESSING_READY
#define OEM_PROCESSING_READY_NOT

#include <sys/types.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define ARRAY_SIZE(_x) (sizeof(_x) / sizeof(_x[0]))
#define SEP_LINE \
"\n-------------------------------------------------------------------------------\n"

#define SEP_LINE2 \
"\n===============================================================================\n"

/* EBDA is @ 40:0e in real-mode terms */
#define EBDA_POINTER		0x040e	/* location of EBDA pointer */

/* CMOS 'top of mem' is @ 40:13 in real-mode terms */
#define TOPOFMEM_POINTER	0x0413	/* BIOS: base memory size */

#define DEFAULT_TOPOFMEM	0xa0000

#define BIOS_BASE		0xf0000
#define BIOS_BASE2		0xe0000
#define BIOS_SIZE		0x10000
#define ONE_KBYTE		1024

#define GROPE_AREA1		0x80000
#define GROPE_AREA2		0x90000
#define GROPE_SIZE		0x10000

#define PROCENTRY_FLAG_EN	0x01
#define PROCENTRY_FLAG_BP	0x02
#define IOAPICENTRY_FLAG_EN	0x01

#define MAXPNSTR		132

#define COREBOOT_MP_TABLE      0

enum busTypes {
	CBUS = 1,
	CBUSII = 2,
	EISA = 3,
	ISA = 6,
	PCI = 13,
	XPRESS = 18,
	MAX_BUSTYPE = 18,
	UNKNOWN_BUSTYPE = 0xff
};

typedef struct BUSTYPENAME {
	uint8_t type;
	char name[7];
} busTypeName;

static busTypeName busTypeTable[] = {
	{CBUS, "CBUS"},
	{CBUSII, "CBUSII"},
	{EISA, "EISA"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{ISA, "ISA"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{PCI, "PCI"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"},
	{UNKNOWN_BUSTYPE, "---"}
};

char *whereStrings[] = {
	"Extended BIOS Data Area",
	"BIOS top of memory",
	"Default top of memory",
	"BIOS",
	"Extended BIOS",
	"GROPE AREA #1",
	"GROPE AREA #2"
};

typedef struct TABLE_ENTRY {
	uint8_t type;
	uint8_t length;
	char name[32];
} tableEntry;

tableEntry basetableEntryTypes[] = {
	{0, 20, "Processor"},
	{1, 8, "Bus"},
	{2, 8, "I/O APIC"},
	{3, 8, "I/O INT"},
	{4, 8, "Local INT"}
};

tableEntry extendedtableEntryTypes[] = {
	{128, 20, "System Address Space"},
	{129, 8, "Bus Heirarchy"},
	{130, 8, "Compatibility Bus Address"}
};

/* MP Floating Pointer Structure */
typedef struct MPFPS {
	uint8_t signature[4];
	uint32_t pap;
	uint8_t length;
	uint8_t spec_rev;
	uint8_t checksum;
	uint8_t mpfb1;
	uint8_t mpfb2;
	uint8_t mpfb3;
	uint8_t mpfb4;
	uint8_t mpfb5;
} mpfps_t;

/* MP Configuration Table Header */
typedef struct MPCTH {
	uint8_t signature[4];
	uint16_t base_table_length;
	uint8_t spec_rev;
	uint8_t checksum;
	uint8_t oem_id[8];
	uint8_t product_id[12];
	uint32_t oem_table_pointer;
	uint16_t oem_table_size;
	uint16_t entry_count;
	uint32_t apic_address;
	uint16_t extended_table_length;
	uint8_t extended_table_checksum;
	uint8_t reserved;
} mpcth_t;

typedef struct PROCENTRY {
	uint8_t type;
	uint8_t apicID;
	uint8_t apicVersion;
	uint8_t cpuFlags;
	uint32_t cpuSignature;
	uint32_t featureFlags;
	uint32_t reserved1;
	uint32_t reserved2;
} ProcEntry;

typedef struct BUSENTRY {
	uint8_t type;
	uint8_t busID;
	uint8_t busType[6];
} BusEntry;

typedef struct IOAPICENTRY {
	uint8_t type;
	uint8_t apicID;
	uint8_t apicVersion;
	uint8_t apicFlags;
	uint32_t apicAddress;
} IOApicEntry;

typedef struct INTENTRY {
	uint8_t type;
	uint8_t intType;
	uint16_t intFlags;
	uint8_t srcBusID;
	uint8_t srcBusIRQ;
	uint8_t dstApicID;
	uint8_t dstApicINT;
} IntEntry;

/*
 * extended entry type structures
 */

typedef struct SASENTRY {
	uint8_t type;
	uint8_t length;
	uint8_t busID;
	uint8_t addressType;
	uint64_t addressBase;
	uint64_t addressLength;
} SasEntry;

typedef struct BHDENTRY {
	uint8_t type;
	uint8_t length;
	uint8_t busID;
	uint8_t busInfo;
	uint8_t busParent;
	uint8_t reserved[3];
} BhdEntry;

typedef struct CBASMENTRY {
	uint8_t type;
	uint8_t length;
	uint8_t busID;
	uint8_t addressMod;
	uint32_t predefinedRange;
} CbasmEntry;

typedef uint32_t vm_offset_t;

static void apic_probe(vm_offset_t * paddr, int *where);

static void MPConfigDefault(int featureByte);

static void MPFloatingPointer(vm_offset_t paddr, int where, mpfps_t * mpfps);
static void MPConfigTableHeader(uint32_t pap);

static int readType(void);
static void seekEntry(vm_offset_t addr);
static void readEntry(void *entry, int size);

static void processorEntry(void);
static void busEntry(void);
static void ioApicEntry(void);
static void intEntry(void);
static void lintEntry(void);

static void sasEntry(void);
static void bhdEntry(void);
static void cbasmEntry(void);

static void doOptionList(void);
static void doDmesg(void);
static void pnstr(uint8_t * s, int c);

/* global data */
int pfd;			/* physical /dev/mem fd */

int busses[32];
int apics[16];

int ncpu;
int nbus;
int napic;
int nintr;

int dmesg = 0;
int grope = 0;
int verbose = 0;
int noisy = 0;
/* preamble to the mptable. This is fixed for all coreboots */

char *preamble[] = {
	"#include <console/console.h>",
	"#include <arch/smp/mpspec.h>",
	"#include <arch/ioapic.h>",
	"#include <device/pci.h>",
	"#include <string.h>",
	"#include <stdint.h>",
	"",
	"#define INTA 0x00",
	"#define INTB 0x01",
	"#define INTC 0x02",
	"#define INTD 0x03",
	"",
	"static void *smp_write_config_table(void *v)",
	"{",
	"        struct mp_config_table *mc;",
	"",
	"        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);",
	"",
	"        mptable_init(mc, \"TODO        \", LOCAL_APIC_ADDR);",
	"",
	"        smp_write_processors(mc);",
	"",
	0
};

char *postamble[] = {
	"	/* Compute the checksums. */",
	"	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);",
	"	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);",
	"	printk(BIOS_DEBUG, \"Wrote the mp table end at: %p - %p\\n\",",
	"		mc, smp_next_mpe_entry(mc));",
	"	return smp_next_mpe_entry(mc);",
	"}",
	"",
	"unsigned long write_smp_table(unsigned long addr)",
	"{",
	"	void *v;",
	"	v = smp_write_floating_table(addr);",
	"	return (unsigned long)smp_write_config_table(v);",
	"}",
	0
};

static void usage(void)
{
	fprintf(stderr,
		"usage: mptable [-dmesg] [-verbose] [-grope] [-help]\n");
	exit(0);
}

void write_code(char **code)
{
	while (*code)
		printf("%s\n", *code++);
}

/*
 *
 */
int main(int argc, char *argv[])
{
	vm_offset_t paddr;
	int where;
	mpfps_t mpfps;
	int defaultConfig;

	/* announce ourselves */

	if (verbose)
		puts(SEP_LINE2);

	printf("/* generated by MPTable, version %d.%d.%d*/\n", VMAJOR, VMINOR,
	       VDELTA);
	printf("/* as modified by RGM for coreboot */\n");
	write_code(preamble);

	/* Ron hates getopt() */

	for (argc--, argv++; argc; argc--, argv++) {
		if (strcmp(argv[0], "-dmesg") == 0) {
			dmesg = 1;
		} else if (strcmp(argv[0], "-help") == 0) {
			usage();
		} else if (strcmp(argv[0], "-grope") == 0) {
			grope = 1;
		} else if (strcmp(argv[0], "-verbose") == 0)
			verbose = 1;
		else if (strcmp(argv[0], "-noisy") == 0)
			noisy = 1;
		else
			usage();
	}

	/* open physical memory for access to MP structures */
	if ((pfd = open("/dev/mem", O_RDONLY)) < 0)
		err(1, "mem open");

	/* probe for MP structures */
	apic_probe(&paddr, &where);
	if (where <= 0) {
		fprintf(stderr, "\n MP FPS NOT found,\n");
		fprintf(stderr, " suggest trying -grope option!!!\n\n");
		return 1;
	}

	if (verbose)
		printf("\n MP FPS found in %s @ physical addr: 0x%08x\n",
		       whereStrings[where - 1], paddr);

	if (verbose)
		puts(SEP_LINE);

	/* analyze the MP Floating Pointer Structure */
	MPFloatingPointer(paddr, where, &mpfps);

	if (verbose)
		puts(SEP_LINE);

	/* check whether an MP config table exists */
	if ((defaultConfig = mpfps.mpfb1))
		MPConfigDefault(defaultConfig);
	else
		MPConfigTableHeader(mpfps.pap);

	/* build "options" entries for the kernel config file */
	if (noisy)
		doOptionList();

	write_code(postamble);
	/* do a dmesg output */
	if (dmesg)
		doDmesg();

	if (verbose)
		puts(SEP_LINE2);

	return 0;
}

/*
 * set PHYSICAL address of MP floating pointer structure
 */
#define NEXT(X)		((X) += 4)
static void apic_probe(vm_offset_t * paddr, int *where)
{
	/*
	 * c rewrite of apic_probe() by Jack F. Vogel
	 */

	int x;
	u_short segment;
	vm_offset_t target;
	u_int buffer[BIOS_SIZE / sizeof(int)];

	if (verbose)
		printf("\n");

	/* search Extended Bios Data Area, if present */
	if (verbose)
		printf(" looking for EBDA pointer @ 0x%04x, ", EBDA_POINTER);
	seekEntry((vm_offset_t) EBDA_POINTER);
	readEntry(&segment, 2);
	if (segment) {		/* search EBDA */
		target = (vm_offset_t) segment << 4;
		if (verbose)
			printf("found, searching EBDA @ 0x%08x\n", target);
		seekEntry(target);
		readEntry(buffer, ONE_KBYTE);

		for (x = 0; x < ONE_KBYTE / sizeof(unsigned int); NEXT(x)) {
			if (buffer[x] == MP_SIG) {
				*where = 1;
				*paddr = (x * sizeof(unsigned int)) + target;
				return;
			}
		}
	} else {
		if (verbose)
			printf("NOT found\n");
	}

	target = 0;
	segment = 0;
	if (verbose)
		printf(" searching for coreboot MP table  @ 0x%08x (%dK)\n",
		       target, segment);
	seekEntry(target);
	readEntry(buffer, ONE_KBYTE);

	for (x = 0; x < ONE_KBYTE / sizeof(unsigned int); NEXT(x)) {
		if (buffer[x] == MP_SIG) {
			*where = 2;
			*paddr = (x * sizeof(unsigned int)) + target;
			return;
		}
	}

	/* read CMOS for real top of mem */
	seekEntry((vm_offset_t) TOPOFMEM_POINTER);
	readEntry(&segment, 2);
	--segment;		/* less ONE_KBYTE */
	target = segment * 1024;
	if (verbose)
		printf(" searching CMOS 'top of mem' @ 0x%08x (%dK)\n",
		       target, segment);
	seekEntry(target);
	readEntry(buffer, ONE_KBYTE);

	for (x = 0; x < ONE_KBYTE / sizeof(unsigned int); NEXT(x)) {
		if (buffer[x] == MP_SIG) {
			*where = 2;
			*paddr = (x * sizeof(unsigned int)) + target;
			return;
		}
	}

	/* we don't necessarily believe CMOS, check base of the last 1K of 640K */
	if (target != (DEFAULT_TOPOFMEM - 1024)) {
		target = (DEFAULT_TOPOFMEM - 1024);
		if (verbose)
			printf
			    (" searching default 'top of mem' @ 0x%08x (%dK)\n",
			     target, (target / 1024));
		seekEntry(target);
		readEntry(buffer, ONE_KBYTE);

		for (x = 0; x < ONE_KBYTE / sizeof(unsigned int); NEXT(x)) {
			if (buffer[x] == MP_SIG) {
				*where = 3;
				*paddr = (x * sizeof(unsigned int)) + target;
				return;
			}
		}
	}

	/* search the BIOS */
	if (verbose)
		printf(" searching BIOS @ 0x%08x\n", BIOS_BASE);
	seekEntry(BIOS_BASE);
	readEntry(buffer, BIOS_SIZE);

	for (x = 0; x < BIOS_SIZE / sizeof(unsigned int); NEXT(x)) {
		if (buffer[x] == MP_SIG) {
			*where = 4;
			*paddr = (x * sizeof(unsigned int)) + BIOS_BASE;
			return;
		}
	}

	/* search the extended BIOS */
	if (verbose)
		printf(" searching extended BIOS @ 0x%08x\n", BIOS_BASE2);
	seekEntry(BIOS_BASE2);
	readEntry(buffer, BIOS_SIZE);

	for (x = 0; x < BIOS_SIZE / sizeof(unsigned int); NEXT(x)) {
		if (buffer[x] == MP_SIG) {
			*where = 5;
			*paddr = (x * sizeof(unsigned int)) + BIOS_BASE2;
			return;
		}
	}

	if (grope) {
		/* search additional memory */
		target = GROPE_AREA1;
		if (verbose)
			printf(" groping memory @ 0x%08x\n", target);
		seekEntry(target);
		readEntry(buffer, GROPE_SIZE);

		for (x = 0; x < GROPE_SIZE / sizeof(unsigned int); NEXT(x)) {
			if (buffer[x] == MP_SIG) {
				*where = 6;
				*paddr =
				    (x * sizeof(unsigned int)) + GROPE_AREA1;
				return;
			}
		}

		target = GROPE_AREA2;
		if (verbose)
			printf(" groping memory @ 0x%08x\n", target);
		seekEntry(target);
		readEntry(buffer, GROPE_SIZE);

		for (x = 0; x < GROPE_SIZE / sizeof(unsigned int); NEXT(x)) {
			if (buffer[x] == MP_SIG) {
				*where = 7;
				*paddr =
				    (x * sizeof(unsigned int)) + GROPE_AREA2;
				return;
			}
		}
	}

	*where = 0;
	*paddr = (vm_offset_t) 0;
}

/*
 *
 */
static void MPFloatingPointer(vm_offset_t paddr, int where, mpfps_t * mpfps)
{

	/* read in mpfps structure */
	seekEntry(paddr);
	readEntry(mpfps, sizeof(mpfps_t));

	/* show its contents */
	if (verbose) {
		printf("MP Floating Pointer Structure:\n\n");

		printf("  location:\t\t\t");
		switch (where) {
		case 1:
			printf("EBDA\n");
			break;
		case 2:
			printf("BIOS base memory\n");
			break;
		case 3:
			printf("DEFAULT base memory (639K)\n");
			break;
		case 4:
			printf("BIOS\n");
			break;
		case 5:
			printf("Extended BIOS\n");
			break;

		case 0:
			printf("NOT found!\n");
			exit(1);
		default:
			printf("BOGUS!\n");
			exit(1);
		}
		printf("  physical address:\t\t0x%08x\n", paddr);

		printf("  signature:\t\t\t'");
		pnstr(mpfps->signature, 4);
		printf("'\n");

		printf("  length:\t\t\t%d bytes\n", mpfps->length * 16);
		printf("  version:\t\t\t1.%1d\n", mpfps->spec_rev);
		printf("  checksum:\t\t\t0x%02x\n", mpfps->checksum);

		/* bits 0:6 are RESERVED */
		if (mpfps->mpfb2 & 0x7f) {
			printf(" warning, MP feature byte 2: 0x%02x\n",
			       mpfps->mpfb2);
		}

		/* bit 7 is IMCRP */
		printf("  mode:\t\t\t\t%s\n", (mpfps->mpfb2 & 0x80) ?
		       "PIC" : "Virtual Wire");

		/* MP feature bytes 3-5 are expected to be ZERO */
		if (mpfps->mpfb3)
			printf(" warning, MP feature byte 3 NONZERO!\n");
		if (mpfps->mpfb4)
			printf(" warning, MP feature byte 4 NONZERO!\n");
		if (mpfps->mpfb5)
			printf(" warning, MP feature byte 5 NONZERO!\n");
	}
}

/*
 *
 */
static void MPConfigDefault(int featureByte)
{
	printf("  MP default config type: %d\n\n", featureByte);
	switch (featureByte) {
	case 1:
		printf("   bus: ISA, APIC: 82489DX\n");
		break;
	case 2:
		printf("   bus: EISA, APIC: 82489DX\n");
		break;
	case 3:
		printf("   bus: EISA, APIC: 82489DX\n");
		break;
	case 4:
		printf("   bus: MCA, APIC: 82489DX\n");
		break;
	case 5:
		printf("   bus: ISA+PCI, APIC: Integrated\n");
		break;
	case 6:
		printf("   bus: EISA+PCI, APIC: Integrated\n");
		break;
	case 7:
		printf("   bus: MCA+PCI, APIC: Integrated\n");
		break;
	default:
		printf("   future type\n");
		break;
	}

	switch (featureByte) {
	case 1:
	case 2:
	case 3:
	case 4:
		nbus = 1;
		break;
	case 5:
	case 6:
	case 7:
		nbus = 2;
		break;
	default:
		printf("   future type\n");
		break;
	}

	ncpu = 2;
	napic = 1;
	nintr = 16;
}

/*
 *
 */
static void MPConfigTableHeader(uint32_t pap)
{
	vm_offset_t paddr;
	mpcth_t cth;
	int x;
	int totalSize;
	int count, c;
	int type;

	if (pap == 0) {
		printf("MP Configuration Table Header MISSING!\n");
		exit(1);
	}

	/* convert physical address to virtual address */
	paddr = (vm_offset_t) pap;

	/* read in cth structure */
	seekEntry(paddr);
	readEntry(&cth, sizeof(cth));

	if (verbose) {
		printf("MP Config Table Header:\n\n");

		printf("  physical address:\t\t0x%08x\n", pap);

		printf("  signature:\t\t\t'");
		pnstr(cth.signature, 4);
		printf("'\n");

		printf("  base table length:\t\t%d\n", cth.base_table_length);

		printf("  version:\t\t\t1.%1d\n", cth.spec_rev);
		printf("  checksum:\t\t\t0x%02x\n", cth.checksum);

		printf("  OEM ID:\t\t\t'");
		pnstr(cth.oem_id, 8);
		printf("'\n");

		printf("  Product ID:\t\t\t'");
		pnstr(cth.product_id, 12);
		printf("'\n");

		printf("  OEM table pointer:\t\t0x%08x\n",
		       cth.oem_table_pointer);
		printf("  OEM table size:\t\t%d\n", cth.oem_table_size);

		printf("  entry count:\t\t\t%d\n", cth.entry_count);

		printf("  local APIC address:\t\t0x%08x\n", cth.apic_address);

		printf("  extended table length:\t%d\n",
		       cth.extended_table_length);
		printf("  extended table checksum:\t%d\n",
		       cth.extended_table_checksum);
	}

	totalSize = cth.base_table_length - sizeof(struct MPCTH);
	count = cth.entry_count;

	if (verbose) {
		if (verbose)
			puts(SEP_LINE);

		printf("MP Config Base Table Entries:\n\n");
	}

	/* initialze tables */
	for(x = 0; x < ARRAY_SIZE(busses); x++)
		busses[x] = UNKNOWN_BUSTYPE;
	for(x = 0; x < ARRAY_SIZE(apics); x++)
		apics[x] = 0xff;

	ncpu = 0;
	nbus = 0;
	napic = 0;
	nintr = 0;

	/* process all the CPUs */
	if (verbose) {
		printf("--\nProcessors:\tAPIC ID\tVersion\tState"
		       "\t\tFamily\tModel\tStep\tFlags\n");
	}
	for (c = count; c; c--) {
		if (readType() == 0)
			processorEntry();
		totalSize -= basetableEntryTypes[0].length;
	}

	/* process all the busses */
	printf("\t/* Bus: Bus ID  Type */\n");
	for (c = count; c; c--) {
		if (readType() == 1)
			busEntry();
		totalSize -= basetableEntryTypes[1].length;
	}

	/* process all the apics */
	printf("\t/* I/O APICs: APIC ID  Version  State  Address */\n");
	for (c = count; c; c--) {
		if (readType() == 2)
			ioApicEntry();
		totalSize -= basetableEntryTypes[2].length;
	}

	/* process all the I/O Ints */
	printf("\t/* I/O Ints: Type  Polarity  Trigger  Bus ID  IRQ  APIC ID  PIN#*/ \n");
	for (c = count; c; c--) {
		if (readType() == 3)
			intEntry();
		totalSize -= basetableEntryTypes[3].length;
	}

	/* process all the Local Ints */
	printf
	    ("\t/* Local Ints: Type  Polarity  Trigger  Bus ID  IRQ  APIC ID  PIN# */\n");
	for (c = count; c; c--) {
		if (readType() == 4)
			lintEntry();
		totalSize -= basetableEntryTypes[4].length;
	}

#if defined( EXTENDED_PROCESSING_READY )
	/* process any extended data */
	if ((totalSize = cth.extended_table_length)) {
		if (verbose)
			puts(SEP_LINE);

		printf("MP Config Extended Table Entries:\n\n");

		while (totalSize > 0) {
			switch (type = readType()) {
			case 128:
				sasEntry();
				break;
			case 129:
				bhdEntry();
				break;
			case 130:
				cbasmEntry();
				break;
			default:
				printf("Extended Table HOSED!\n");
				exit(1);
			}

			totalSize -= extendedtableEntryTypes[type - 128].length;
		}
	}
#endif				/* EXTENDED_PROCESSING_READY */

	/* process any OEM data */
	if (cth.oem_table_pointer && (cth.oem_table_size > 0)) {
#if defined( OEM_PROCESSING_READY )
#error your on your own here!
		/* convert OEM table pointer to virtual address */
		poemtp = (vm_offset_t) cth.oem_table_pointer;

		/* read in oem table structure */
		if ((oemdata = (void *)malloc(cth.oem_table_size)) == NULL)
			err(1, "oem malloc");

		seekEntry(poemtp);
		readEntry(oemdata, cth.oem_table_size);

	/** process it */

		free(oemdata);
#else
		printf
		    ("\nyou need to modify the source to handle OEM data!\n\n");
#endif				/* OEM_PROCESSING_READY */
	}

	fflush(stdout);

#if defined( RAW_DUMP )
	{
		int ofd;
		u_char dumpbuf[4096];

		ofd = open("/tmp/mpdump", O_CREAT | O_RDWR);
		seekEntry(paddr);
		readEntry(dumpbuf, 1024);
		write(ofd, dumpbuf, 1024);
		close(ofd);
	}
#endif				/* RAW_DUMP */
}

/*
 *
 */
static int readType(void)
{
	u_char type;

	if (read(pfd, &type, sizeof(u_char)) != sizeof(u_char))
		err(1, "type read; pfd: %d", pfd);

	if (lseek(pfd, -1, SEEK_CUR) < 0)
		err(1, "type seek");

	return (int)type;
}

/*
 *
 */
static void seekEntry(vm_offset_t addr)
{
	if (lseek(pfd, (off_t) addr, SEEK_SET) < 0)
		err(1, "/dev/mem seek");
}

/*
 *
 */
static void readEntry(void *entry, int size)
{
	if (read(pfd, entry, size) != size)
		err(1, "readEntry");
}

static void processorEntry(void)
{
	ProcEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	/* count it */
	++ncpu;
	if (noisy) {
		printf("\t\t%2d", entry.apicID);
		printf("\t 0x%2x", entry.apicVersion);

		printf("\t %s, %s",
		       (entry.cpuFlags & PROCENTRY_FLAG_BP) ? "BSP" : "AP",
		       (entry.
			cpuFlags & PROCENTRY_FLAG_EN) ? "usable" : "unusable");

		printf("\t %d\t %d\t %d",
		       (entry.cpuSignature >> 8) & 0x0f,
		       (entry.cpuSignature >> 4) & 0x0f,
		       entry.cpuSignature & 0x0f);

		printf("\t 0x%04x\n", entry.featureFlags);
	}
}

/*
 *
 */
static int lookupBusType(char *name)
{
	int x;

	for (x = 0; x < MAX_BUSTYPE; ++x)
		if (strcmp(busTypeTable[x].name, name) == 0)
			return busTypeTable[x].type;

	return UNKNOWN_BUSTYPE;
}

static void busEntry(void)
{
	char name[8];
	BusEntry entry;
	int i;
	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	/* count it */
	++nbus;

	if (verbose) {
		printf("\t\t%2d", entry.busID);
		printf("\t ");
		pnstr(entry.busType, 6);
		printf("\n");
	}

	memset(name, '\0', sizeof(name));
	for(i = 0; i < 6; i++) {
		switch(entry.busType[i]) {
		case ' ':
		case '\0':
			break;
		default:
			name[i] = entry.busType[i];
			break;
		}
	}

	if (entry.busID > ARRAY_SIZE(busses)) {
		fprintf(stderr, "busses array to small!\n");
		exit(1);
	}

	busses[entry.busID] = lookupBusType(name);
	printf("\tsmp_write_bus(mc, %d, \"", entry.busID);
	pnstr(entry.busType, 6);
	printf("\");\n");
}

static void ioApicEntry(void)
{
	IOApicEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	/* count it */
	++napic;

	if (noisy) {
		printf("\t\t%2d", entry.apicID);
		printf("\t 0x%02x", entry.apicVersion);
		printf("\t %s",
		       (entry.
			apicFlags & IOAPICENTRY_FLAG_EN) ? "usable" :
		       "unusable");
		printf("\t\t 0x%x\n", entry.apicAddress);
	}

	apics[entry.apicID] = entry.apicID;

	if (entry.apicFlags & IOAPICENTRY_FLAG_EN)
		printf("\tsmp_write_ioapic(mc, 0x%x, 0x%x, 0x%x);\n",
		       entry.apicID, entry.apicVersion, entry.apicAddress);
}

char *intTypes[] = {
	"mp_INT", "mp_NMI", "mp_SMI", "mp_ExtINT"
};

char *polarityMode[] = {
	"MP_IRQ_POLARITY_DEFAULT", "MP_IRQ_POLARITY_HIGH", "reserved",
	    "MP_IRQ_POLARITY_LOW"
};

char *triggerMode[] = {
	"MP_IRQ_TRIGGER_DEFAULT", "MP_IRQ_TRIGGER_EDGE", "reserved",
	    "MP_IRQ_TRIGGER_LEVEL"
};

static void intEntry(void)
{
	IntEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	/* count it */
	if ((int)entry.type == 3)
		++nintr;

	if (noisy) {
		printf("\n\t\t%s", intTypes[(int)entry.intType]);

		printf("\t%9s", polarityMode[(int)entry.intFlags & 0x03]);
		printf("%12s", triggerMode[((int)entry.intFlags >> 2) & 0x03]);

		printf("\t %5d", (int)entry.srcBusID);
		if (busses[(int)entry.srcBusID] == PCI)
			printf("\t%2d:%c",
			       ((int)entry.srcBusIRQ >> 2) & 0x1f,
			       ((int)entry.srcBusIRQ & 0x03) + 'A');
		else
			printf("\t 0x%x:0x%x(0x%x)",
			       (int)entry.srcBusIRQ >> 2,
			       (int)entry.srcBusIRQ & 3, (int)entry.srcBusIRQ);
		printf("\t %6d", (int)entry.dstApicID);
		printf("\t %3d\n", (int)entry.dstApicINT);
	}

	if (busses[(int)entry.srcBusID] == PCI) {
		printf("\tsmp_write_intsrc(mc, %s, %s|%s, 0x%x, (0x%02x << 2) | INT%c, 0x%x, 0x%x);\n",
		       intTypes[(int)entry.intType],
		       triggerMode[((int)entry.intFlags >> 2) & 0x03],
		       polarityMode[(int)entry.intFlags & 0x03],
		       (int)entry.srcBusID,
		       (int)entry.srcBusIRQ >> 2,
		       ((int)entry.srcBusIRQ & 3) + 'A',
		       (int)entry.dstApicID, (int)entry.dstApicINT);
	} else {
		printf("\tsmp_write_intsrc(mc, %s, %s|%s, 0x%x, 0x%x, 0x%x, 0x%x);\n",
		       intTypes[(int)entry.intType],
		       triggerMode[((int)entry.intFlags >> 2) & 0x03],
		       polarityMode[(int)entry.intFlags & 0x03],
		       (int)entry.srcBusID,
		       (int)entry.srcBusIRQ,
		       (int)entry.dstApicID, (int)entry.dstApicINT);
	}
}

static void lintEntry(void)
{
	IntEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	/* count it */
	if ((int)entry.type == 3)
		++nintr;

	if (noisy) {
		printf("\t\t%s", intTypes[(int)entry.intType]);

		printf("\t%9s", polarityMode[(int)entry.intFlags & 0x03]);
		printf("%12s", triggerMode[((int)entry.intFlags >> 2) & 0x03]);

		printf("\t %5d", (int)entry.srcBusID);
		if (busses[(int)entry.srcBusID] == PCI)
			printf("\t%2d:%c",
			       ((int)entry.srcBusIRQ >> 2) & 0x1f,
			       ((int)entry.srcBusIRQ & 0x03) + 'A');
		else
			printf("\t %3d", (int)entry.srcBusIRQ);
		printf("\t %6d", (int)entry.dstApicID);
		printf("\t %3d\n", (int)entry.dstApicINT);
	}
	printf
	    ("\tsmp_write_lintsrc(mc, %s, %s|%s, 0x%x, 0x%x, MP_APIC_ALL, 0x%x);\n",
	     intTypes[(int)entry.intType],
	     triggerMode[((int)entry.intFlags >> 2) & 0x03],
	     polarityMode[(int)entry.intFlags & 0x03], (int)entry.srcBusID,
	     (int)entry.srcBusIRQ, (int)entry.dstApicINT);

}

static void sasEntry(void)
{
	SasEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	printf("--\n%s\n", extendedtableEntryTypes[entry.type - 128].name);
	printf(" bus ID: %d", entry.busID);
	printf(" address type: ");
	switch (entry.addressType) {
	case 0:
		printf("I/O address\n");
		break;
	case 1:
		printf("memory address\n");
		break;
	case 2:
		printf("prefetch address\n");
		break;
	default:
		printf("UNKNOWN type\n");
		break;
	}

	printf(" address base: 0x%lx\n", entry.addressBase);
	printf(" address range: 0x%lx\n", entry.addressLength);
}

static void bhdEntry(void)
{
	BhdEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	printf("--\n%s\n", extendedtableEntryTypes[entry.type - 128].name);
	printf(" bus ID: %d", entry.busID);
	printf(" bus info: 0x%02x", entry.busInfo);
	printf(" parent bus ID: %d", entry.busParent);
}

static void cbasmEntry(void)
{
	CbasmEntry entry;

	/* read it into local memory */
	readEntry(&entry, sizeof(entry));

	printf("--\n%s\n", extendedtableEntryTypes[entry.type - 128].name);
	printf(" bus ID: %d", entry.busID);
	printf(" address modifier: %s\n", (entry.addressMod & 0x01) ?
	       "subtract" : "add");
	printf(" predefined range: 0x%08x", entry.predefinedRange);
}

/*
 * do a dmesg output
 */
static void doDmesg(void)
{
	if (verbose)
		puts(SEP_LINE);

	printf("dmesg output:\n\n");
	fflush(stdout);
	system("dmesg");
}

/*
 *  build "options" entries for the kernel config file
 */
static void doOptionList(void)
{
	if (verbose)
		puts(SEP_LINE);

	printf("# SMP kernel config file options:\n\n");
	printf("\n# Required:\n");
	printf("options		SMP\t\t\t# Symmetric MultiProcessor Kernel\n");
	printf("options		APIC_IO\t\t\t# Symmetric (APIC) I/O\n");

	printf("\n# Optional (built-in defaults will work in most cases):\n");
	printf("#options		NCPU=%d\t\t\t# number of CPUs\n", ncpu);
	printf("#options		NBUS=%d\t\t\t# number of busses\n",
	       nbus);
	printf("#options		NAPIC=%d\t\t\t# number of IO APICs\n",
	       napic);
	printf("#options		NINTR=%d\t\t# number of INTs\n",
	       (nintr < 24) ? 24 : nintr);
}

/*
 *
 */
static void pnstr(uint8_t * s, int c)
{
	uint8_t string[MAXPNSTR + 1];

	if (c > MAXPNSTR)
		c = MAXPNSTR;
	strncpy((char *)string, (char *)s, c);
	string[c] = '\0';
	printf("%s", string);
}
