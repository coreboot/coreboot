/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2005.
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *                                                                       
 *  You should have received a copy of the GNU General Public License    
 *  along with this program; if not, write to the Free Software          
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *                                                                   USA 
 */

#include "dtc.h"

char *join_path(char *path, char *name)
{
	int lenp = strlen(path);
	int lenn = strlen(name);
	int len;
	int needslash = 1;
	char *str;

	len = lenp + lenn + 2;
	if ((lenp > 0) && (path[lenp-1] == '/')) {
		needslash = 0;
		len--;
	}

	str = xmalloc(len);
	memcpy(str, path, lenp);
	if (needslash) {
		str[lenp] = '/';
		lenp++;
	}
	memcpy(str+lenp, name, lenn+1);
	return str;
}

void fill_fullpaths(struct node *tree, char *prefix)
{
	struct node *child;
	char *unit;

	tree->fullpath = join_path(prefix, tree->name);

	unit = strchr(tree->name, '@');
	if (unit)
		tree->basenamelen = unit - tree->name;
	else
		tree->basenamelen = strlen(tree->name);

	for_each_child(tree, child)
		fill_fullpaths(child, tree->fullpath);
}

static FILE *dtc_open_file(char *fname)
{
	FILE *f;

	if (streq(fname, "-"))
		f = stdin;
	else
		f = fopen(fname, "r");

	if (! f)
		die("Couldn't open \"%s\": %s\n", fname, strerror(errno));

	return f;
}

static void usage(void)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\tdtc [options] <input file>\n");
	fprintf(stderr, "\nOptions:\n");
	fprintf(stderr, "\t-I <input format>\n");
	fprintf(stderr, "\t\tInput formats are:\n");
	fprintf(stderr, "\t\t\tdts - device tree source text\n");
	fprintf(stderr, "\t\t\tdtb - device tree blob\n");
	fprintf(stderr, "\t\t\tfs - /proc/device-tree style directory\n");
	fprintf(stderr, "\t-O <output format>\n");
	fprintf(stderr, "\t\tOutput formats are:\n");
	fprintf(stderr, "\t\t\tdts - device tree source text\n");
	fprintf(stderr, "\t\t\tdtb - device tree blob\n");
	fprintf(stderr, "\t\t\tasm - assembler source\n");
	fprintf(stderr, "\t\t\tlinuxbios (or just lb) - linuxbios source\n");
	fprintf(stderr, "\t-V <output version>\n");
	fprintf(stderr, "\t\tBlob version to produce, defaults to 3 (relevant for dtb\n\t\tand asm output only)\n");
	fprintf(stderr, "\t-R <number>\n");
	fprintf(stderr, "\t\tMake space for <number> reserve map entries (relevant for \n\t\tdtb and asm output only)\n");
	fprintf(stderr, "\t-b <number>\n");
	fprintf(stderr, "\t\tSet the physical boot cpu\n");
	fprintf(stderr, "\t-f\n");
	fprintf(stderr, "\t\tForce - try to produce output even if the input tree has errors\n");
	exit(2);
}

int main(int argc, char *argv[])
{
	struct boot_info *bi;
	char *inform = "dts";
	char *outform = "dts";
	char *outname = "-";
	int force = 0;
	char *arg;
	int opt;
	FILE *inf = NULL;
	FILE *outf = NULL;
	int outversion = 3;
	int reservenum = 1;
	int boot_cpuid_phys = 0xfeedbeef;

	while ((opt = getopt(argc, argv, "I:O:o:V:R:fb:")) != EOF) {
		switch (opt) {
		case 'I':
			inform = optarg;
			break;
		case 'O':
			outform = optarg;
			break;
		case 'o':
			outname = optarg;
			break;
		case 'V':
			outversion = strtol(optarg, NULL, 0);
			break;
		case 'R':
			reservenum = strtol(optarg, NULL, 0);
			break;
		case 'f':
			force = 1;
			break;
		case 'b':
			boot_cpuid_phys = strtol(optarg, NULL, 0);
			break;
		default:
			usage();
		}
	}

	if (argc > (optind+1))
		usage();
	else if (argc < (optind+1))
		arg = "-";
	else
		arg = argv[optind];

	fprintf(stderr, "DTC: %s->%s  on file \"%s\"\n",
		inform, outform, arg);

	if (streq(inform, "dts")) {
		inf = dtc_open_file(arg);
		bi = dt_from_source(inf);
	} else if (streq(inform, "fs")) {
		bi = dt_from_fs(arg);
	} else if(streq(inform, "dtb")) {
		inf = dtc_open_file(arg);
		bi = dt_from_blob(inf);
	} else {
		die("Unknown input format \"%s\"\n", inform);
	}

	if (inf && (inf != stdin))
		fclose(inf);

	if (! bi || ! bi->dt)
		die("Couldn't read input tree\n");

	if (! check_device_tree(bi->dt, outversion, boot_cpuid_phys)) {
		fprintf(stderr, "Input tree has errors\n");
		if (! force)
			exit(1);
	}

	if (streq(outname, "-")) {
		outf = stdout;
	} else {
		outf = fopen(outname, "w");
		if (! outf)
			die("Couldn't open output file %s: %s\n",
			    outname, strerror(errno));
	}

	if (streq(outform, "dts")) {
		dt_to_source(outf, bi);
	} else if (streq(outform, "dtb")) {
		dt_to_blob(outf, bi, outversion, boot_cpuid_phys);
	} else if (streq(outform, "asm")) {
		dt_to_asm(outf, bi, outversion, boot_cpuid_phys);
	} else if (streq(outform, "C")) {
		dt_to_C(outf, bi, outversion, boot_cpuid_phys);
	} else if (streq(outform, "linuxbios") || streq(outform, "lb")) {
		dt_to_linuxbios(outf, bi, outversion, boot_cpuid_phys);
	} else if (streq(outform, "null")) {
		/* do nothing */
	} else {
		die("Unknown output format \"%s\"\n", outform);
	}
		
	exit(0);
}
