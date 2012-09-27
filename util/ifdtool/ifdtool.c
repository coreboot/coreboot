/*
 * ifdtool - dump Intel Firmware Descriptor information
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ifdtool.h"

static fdbar_t *find_fd(char *image, int size)
{
	int i, found = 0;

	/* Scan for FD signature */
	for (i = 0; i < (size - 4); i += 4) {
		if (*(uint32_t *) (image + i) == 0x0FF0A55A) {
			found = 1;
			break;	// signature found.
		}
	}

	if (!found) {
		printf("No Flash Descriptor found in this image\n");
		return NULL;
	}

	printf("Found Flash Descriptor signature at 0x%08x\n", i);

	return (fdbar_t *) (image + i);
}

static region_t get_region(frba_t *frba, int region_type)
{
	region_t region;
	region.base = 0, region.limit = 0, region.size = 0;

	switch (region_type) {
	case 0:
		region.base = (frba->flreg0 & 0x00000fff) << 12;
		region.limit = ((frba->flreg0 & 0x0fff0000) >> 4) | 0xfff;
		break;
	case 1:
		region.base = (frba->flreg1 & 0x00000fff) << 12;
		region.limit = ((frba->flreg1 & 0x0fff0000) >> 4) | 0xfff;
		break;
	case 2:
		region.base = (frba->flreg2 & 0x00000fff) << 12;
		region.limit = ((frba->flreg2 & 0x0fff0000) >> 4) | 0xfff;
		break;
	case 3:
		region.base = (frba->flreg3 & 0x00000fff) << 12;
		region.limit = ((frba->flreg3 & 0x0fff0000) >> 4) | 0xfff;
		break;
	case 4:
		region.base = (frba->flreg4 & 0x00000fff) << 12;
		region.limit = ((frba->flreg4 & 0x0fff0000) >> 4) | 0xfff;
		break;
	default:
		fprintf(stderr, "Invalid region type.\n");
		exit (EXIT_FAILURE);
	}

	region.size = region.limit - region.base + 1;

	return region;
}

static const char *region_name(int region_type)
{
	static const char *regions[5] = {
		"Flash Descriptor",
		"BIOS",
		"Intel ME",
		"GbE",
		"Platform Data"
	};

	if (region_type < 0 || region_type > 4) {
		fprintf(stderr, "Invalid region type.\n");
		exit (EXIT_FAILURE);
	}

	return regions[region_type];
}

static const char *region_filename(int region_type)
{
	static const char *region_filenames[5] = {
		"flashregion_0_flashdescriptor.bin",
		"flashregion_1_bios.bin",
		"flashregion_2_intel_me.bin",
		"flashregion_3_gbe.bin",
		"flashregion_4_platform_data.bin"
	};

	if (region_type < 0 || region_type > 4) {
		fprintf(stderr, "Invalid region type.\n");
		exit (EXIT_FAILURE);
	}

	return region_filenames[region_type];
}

static void dump_region(int num, frba_t *frba)
{
	region_t region = get_region(frba, num);
	printf("  Flash Region %d (%s): %08x - %08x %s\n",
		       num, region_name(num), region.base, region.limit,
		       region.size < 1 ? "(unused)" : "");
}

static void dump_frba(frba_t * frba)
{
	printf("Found Region Section\n");
	printf("FLREG0:    0x%08x\n", frba->flreg0);
	dump_region(0, frba);
	printf("FLREG1:    0x%08x\n", frba->flreg1);
	dump_region(1, frba);
	printf("FLREG2:    0x%08x\n", frba->flreg2);
	dump_region(2, frba);
	printf("FLREG3:    0x%08x\n", frba->flreg3);
	dump_region(3, frba);
	printf("FLREG4:    0x%08x\n", frba->flreg4);
	dump_region(4, frba);
}

static void decode_spi_frequency(unsigned int freq)
{
	switch (freq) {
	case SPI_FREQUENCY_20MHZ:
		printf("20MHz");
		break;
	case SPI_FREQUENCY_33MHZ:
		printf("33MHz");
		break;
	case SPI_FREQUENCY_50MHZ:
		printf("50MHz");
		break;
	default:
		printf("unknown<%x>MHz", freq);
	}
}

static void decode_component_density(unsigned int density)
{
	switch (density) {
	case COMPONENT_DENSITY_512KB:
		printf("512KB");
		break;
	case COMPONENT_DENSITY_1MB:
		printf("1MB");
		break;
	case COMPONENT_DENSITY_2MB:
		printf("2MB");
		break;
	case COMPONENT_DENSITY_4MB:
		printf("4MB");
		break;
	case COMPONENT_DENSITY_8MB:
		printf("8MB");
		break;
	case COMPONENT_DENSITY_16MB:
		printf("16MB");
		break;
	default:
		printf("unknown<%x>MB", density);
	}
}

static void dump_fcba(fcba_t * fcba)
{
	printf("\nFound Component Section\n");
	printf("FLCOMP     0x%08x\n", fcba->flcomp);
	printf("  Dual Output Fast Read Support:       %ssupported\n",
		(fcba->flcomp & (1 << 30))?"":"not ");
	printf("  Read ID/Read Status Clock Frequency: ");
	decode_spi_frequency((fcba->flcomp >> 27) & 7);
	printf("\n  Write/Erase Clock Frequency:         ");
	decode_spi_frequency((fcba->flcomp >> 24) & 7);
	printf("\n  Fast Read Clock Frequency:           ");
	decode_spi_frequency((fcba->flcomp >> 21) & 7);
	printf("\n  Fast Read Support:                   %ssupported",
		(fcba->flcomp & (1 << 20))?"":"not ");
	printf("\n  Read Clock Frequency:                ");
	decode_spi_frequency((fcba->flcomp >> 17) & 7);
	printf("\n  Component 2 Density:                 ");
	decode_component_density((fcba->flcomp >> 3) & 7);
	printf("\n  Component 1 Density:                 ");
	decode_component_density(fcba->flcomp & 7);
	printf("\n");
	printf("FLILL      0x%08x\n", fcba->flill);
	printf("  Invalid Instruction 3: 0x%02x\n",
		(fcba->flill >> 24) & 0xff);
	printf("  Invalid Instruction 2: 0x%02x\n",
		(fcba->flill >> 16) & 0xff);
	printf("  Invalid Instruction 1: 0x%02x\n",
		(fcba->flill >> 8) & 0xff);
	printf("  Invalid Instruction 0: 0x%02x\n",
		fcba->flill & 0xff);
	printf("FLPB       0x%08x\n", fcba->flpb);
	printf("  Flash Partition Boundary Address: 0x%06x\n\n",
		(fcba->flpb & 0xfff) << 12);
}

static void dump_fpsba(fpsba_t * fpsba)
{
	printf("Found PCH Strap Section\n");
	printf("PCHSTRP0:  0x%08x\n", fpsba->pchstrp0);
	printf("PCHSTRP1:  0x%08x\n", fpsba->pchstrp1);
	printf("PCHSTRP2:  0x%08x\n", fpsba->pchstrp2);
	printf("PCHSTRP3:  0x%08x\n", fpsba->pchstrp3);
	printf("PCHSTRP4:  0x%08x\n", fpsba->pchstrp4);
	printf("PCHSTRP5:  0x%08x\n", fpsba->pchstrp5);
	printf("PCHSTRP6:  0x%08x\n", fpsba->pchstrp6);
	printf("PCHSTRP7:  0x%08x\n", fpsba->pchstrp7);
	printf("PCHSTRP8:  0x%08x\n", fpsba->pchstrp8);
	printf("PCHSTRP9:  0x%08x\n", fpsba->pchstrp9);
	printf("PCHSTRP10: 0x%08x\n", fpsba->pchstrp10);
	printf("PCHSTRP11: 0x%08x\n", fpsba->pchstrp11);
	printf("PCHSTRP12: 0x%08x\n", fpsba->pchstrp12);
	printf("PCHSTRP13: 0x%08x\n", fpsba->pchstrp13);
	printf("PCHSTRP14: 0x%08x\n", fpsba->pchstrp14);
	printf("PCHSTRP15: 0x%08x\n", fpsba->pchstrp15);
	printf("PCHSTRP16: 0x%08x\n", fpsba->pchstrp16);
	printf("PCHSTRP17: 0x%08x\n\n", fpsba->pchstrp17);
}

static void decode_flmstr(uint32_t flmstr)
{
	printf("  Platform Data Region Write Access: %s\n",
		(flmstr & (1 << 28)) ? "enabled" : "disabled");
	printf("  GbE Region Write Access:           %s\n",
		(flmstr & (1 << 27)) ? "enabled" : "disabled");
	printf("  Intel ME Region Write Access:      %s\n",
		(flmstr & (1 << 26)) ? "enabled" : "disabled");
	printf("  Host CPU/BIOS Region Write Access: %s\n",
		(flmstr & (1 << 25)) ? "enabled" : "disabled");
	printf("  Flash Descriptor Write Access:     %s\n",
		(flmstr & (1 << 24)) ? "enabled" : "disabled");

	printf("  Platform Data Region Read Access:  %s\n",
		(flmstr & (1 << 20)) ? "enabled" : "disabled");
	printf("  GbE Region Read Access:            %s\n",
		(flmstr & (1 << 19)) ? "enabled" : "disabled");
	printf("  Intel ME Region Read Access:       %s\n",
		(flmstr & (1 << 18)) ? "enabled" : "disabled");
	printf("  Host CPU/BIOS Region Read Access:  %s\n",
		(flmstr & (1 << 17)) ? "enabled" : "disabled");
	printf("  Flash Descriptor Read Access:      %s\n",
		(flmstr & (1 << 16)) ? "enabled" : "disabled");

	printf("  Requester ID:                      0x%04x\n\n",
		flmstr & 0xffff);
}

static void dump_fmba(fmba_t * fmba)
{
	printf("Found Master Section\n");
	printf("FLMSTR1:   0x%08x (Host CPU/BIOS)\n", fmba->flmstr1);
	decode_flmstr(fmba->flmstr1);
	printf("FLMSTR2:   0x%08x (Intel ME)\n", fmba->flmstr2);
	decode_flmstr(fmba->flmstr2);
	printf("FLMSTR3:   0x%08x (GbE)\n", fmba->flmstr3);
	decode_flmstr(fmba->flmstr3);
}

static void dump_fmsba(fmsba_t * fmsba)
{
	printf("Found Processor Strap Section\n");
	printf("????:      0x%08x\n", fmsba->data[0]);
	printf("????:      0x%08x\n", fmsba->data[1]);
	printf("????:      0x%08x\n", fmsba->data[2]);
	printf("????:      0x%08x\n", fmsba->data[3]);
}

static void dump_jid(uint32_t jid)
{
	printf("    SPI Componend Device ID 1:          0x%02x\n",
		(jid >> 16) & 0xff);
	printf("    SPI Componend Device ID 0:          0x%02x\n",
		(jid >> 8) & 0xff);
	printf("    SPI Componend Vendor ID:            0x%02x\n",
		jid & 0xff);
}

static void dump_vscc(uint32_t vscc)
{
	printf("    Lower Erase Opcode:                 0x%02x\n",
		vscc >> 24);
	printf("    Lower Write Enable on Write Status: 0x%02x\n",
		vscc & (1 << 20) ? 0x06 : 0x50);
	printf("    Lower Write Status Required:        %s\n",
		vscc & (1 << 19) ? "Yes" : "No");
	printf("    Lower Write Granularity:            %d bytes\n",
		vscc & (1 << 18) ? 64 : 1);
	printf("    Lower Block / Sector Erase Size:    ");
	switch ((vscc >> 16) & 0x3) {
	case 0:
		printf("256 Byte\n");
		break;
	case 1:
		printf("4KB\n");
		break;
	case 2:
		printf("8KB\n");
		break;
	case 3:
		printf("64KB\n");
		break;
	}

	printf("    Upper Erase Opcode:                 0x%02x\n",
		(vscc >> 8) & 0xff);
	printf("    Upper Write Enable on Write Status: 0x%02x\n",
		vscc & (1 << 4) ? 0x06 : 0x50);
	printf("    Upper Write Status Required:        %s\n",
		vscc & (1 << 3) ? "Yes" : "No");
	printf("    Upper Write Granularity:            %d bytes\n",
		vscc & (1 << 2) ? 64 : 1);
	printf("    Upper Block / Sector Erase Size:    ");
	switch (vscc & 0x3) {
	case 0:
		printf("256 Byte\n");
		break;
	case 1:
		printf("4KB\n");
		break;
	case 2:
		printf("8KB\n");
		break;
	case 3:
		printf("64KB\n");
		break;
	}
}

static void dump_vtba(vtba_t *vtba, int vtl)
{
	int i;
	int num = (vtl >> 1) < 8 ? (vtl >> 1) : 8;

	printf("ME VSCC table:\n");
	for (i = 0; i < num; i++) {
		printf("  JID%d:  0x%08x\n", i, vtba->entry[i].jid);
		dump_jid(vtba->entry[i].jid);
		printf("  VSCC%d: 0x%08x\n", i, vtba->entry[i].vscc);
		dump_vscc(vtba->entry[i].vscc);
	}
	printf("\n");
}

static void dump_oem(uint8_t *oem)
{
	int i, j;
	printf("OEM Section:\n");
	for (i = 0; i < 4; i++) {
		printf("%02x:", i << 4);
		for (j = 0; j < 16; j++)
			printf(" %02x", oem[(i<<4)+j]);
		printf ("\n");
	}
	printf ("\n");
}

static void dump_fd(char *image, int size)
{
	fdbar_t *fdb = find_fd(image, size);
	if (!fdb)
		exit(EXIT_FAILURE);

	printf("FLMAP0:    0x%08x\n", fdb->flmap0);
	printf("  NR:      %d\n", (fdb->flmap0 >> 24) & 7);
	printf("  FRBA:    0x%x\n", ((fdb->flmap0 >> 16) & 0xff) << 4);
	printf("  NC:      %d\n", ((fdb->flmap0 >> 8) & 3) + 1);
	printf("  FCBA:    0x%x\n", ((fdb->flmap0) & 0xff) << 4);

	printf("FLMAP1:    0x%08x\n", fdb->flmap1);
	printf("  ISL:     0x%02x\n", (fdb->flmap1 >> 24) & 0xff);
	printf("  FPSBA:   0x%x\n", ((fdb->flmap1 >> 16) & 0xff) << 4);
	printf("  NM:      %d\n", (fdb->flmap1 >> 8) & 3);
	printf("  FMBA:    0x%x\n", ((fdb->flmap1) & 0xff) << 4);

	printf("FLMAP2:    0x%08x\n", fdb->flmap2);
	printf("  PSL:     0x%04x\n", (fdb->flmap2 >> 8) & 0xffff);
	printf("  FMSBA:   0x%x\n", ((fdb->flmap2) & 0xff) << 4);

	printf("FLUMAP1:   0x%08x\n", fdb->flumap1);
	printf("  Intel ME VSCC Table Length (VTL):        %d\n",
		(fdb->flumap1 >> 8) & 0xff);
	printf("  Intel ME VSCC Table Base Address (VTBA): 0x%06x\n\n",
		(fdb->flumap1 & 0xff) << 4);
	dump_vtba((vtba_t *)
			(image + ((fdb->flumap1 & 0xff) << 4)),
			(fdb->flumap1 >> 8) & 0xff);
	dump_oem((uint8_t *)image + 0xf00);
	dump_frba((frba_t *)
			(image + (((fdb->flmap0 >> 16) & 0xff) << 4)));
	dump_fcba((fcba_t *) (image + (((fdb->flmap0) & 0xff) << 4)));
	dump_fpsba((fpsba_t *)
			(image + (((fdb->flmap1 >> 16) & 0xff) << 4)));
	dump_fmba((fmba_t *) (image + (((fdb->flmap1) & 0xff) << 4)));
	dump_fmsba((fmsba_t *) (image + (((fdb->flmap2) & 0xff) << 4)));
}

static void write_regions(char *image, int size)
{
	int i;

	fdbar_t *fdb = find_fd(image, size);
	if (!fdb)
		exit(EXIT_FAILURE);

	frba_t *frba =
	    (frba_t *) (image + (((fdb->flmap0 >> 16) & 0xff) << 4));

	for (i = 0; i<5; i++) {
		region_t region = get_region(frba, i);
		dump_region(i, frba);
		if (region.size > 0) {
			int region_fd;
			region_fd = open(region_filename(i),
					 O_WRONLY | O_CREAT | O_TRUNC,
					 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if (write(region_fd, image + region.base, region.size) != region.size)
				perror("Error while writing");
			close(region_fd);
		}
	}
}

static void write_image(char *filename, char *image, int size)
{
	char new_filename[FILENAME_MAX]; // allow long file names
	int new_fd;

	strncpy(new_filename, filename, FILENAME_MAX);
	strncat(new_filename, ".new", FILENAME_MAX - strlen(filename));

	printf("Writing new image to %s\n", new_filename);

	// Now write out new image
	new_fd = open(new_filename,
			 O_WRONLY | O_CREAT | O_TRUNC,
			 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (write(new_fd, image, size) != size)
		perror("Error while writing");
	close(new_fd);
}

static void set_spi_frequency(char *filename, char *image, int size,
			      enum spi_frequency freq)
{
	fdbar_t *fdb = find_fd(image, size);
	fcba_t *fcba = (fcba_t *) (image + (((fdb->flmap0) & 0xff) << 4));

	/* clear bits 21-29 */
	fcba->flcomp &= ~0x3fe00000;
	/* Read ID and Read Status Clock Frequency */
	fcba->flcomp |= freq << 27;
	/* Write and Erase Clock Frequency */
	fcba->flcomp |= freq << 24;
	/* Fast Read Clock Frequency */
	fcba->flcomp |= freq << 21;

	write_image(filename, image, size);
}

static void set_em100_mode(char *filename, char *image, int size)
{
	fdbar_t *fdb = find_fd(image, size);
	fcba_t *fcba = (fcba_t *) (image + (((fdb->flmap0) & 0xff) << 4));

	fcba->flcomp &= ~(1 << 30);
	set_spi_frequency(filename, image, size, SPI_FREQUENCY_20MHZ);
}

static void lock_descriptor(char *filename, char *image, int size)
{
	fdbar_t *fdb = find_fd(image, size);
	fmba_t *fmba = (fmba_t *) (image + (((fdb->flmap1) & 0xff) << 4));
	/* TODO: Dynamically take Platform Data Region and GbE Region
	 * into regard.
	 */
	fmba->flmstr1 = 0x0a0b0000;
	fmba->flmstr2 = 0x0c0d0000;
	fmba->flmstr3 = 0x08080118;

	write_image(filename, image, size);
}

static void unlock_descriptor(char *filename, char *image, int size)
{
	fdbar_t *fdb = find_fd(image, size);
	fmba_t *fmba = (fmba_t *) (image + (((fdb->flmap1) & 0xff) << 4));
	fmba->flmstr1 = 0xffff0000;
	fmba->flmstr2 = 0xffff0000;
	fmba->flmstr3 = 0x08080118;

	write_image(filename, image, size);
}

void inject_region(char *filename, char *image, int size, int region_type,
		   char *region_fname)
{
	fdbar_t *fdb = find_fd(image, size);
	if (!fdb)
		exit(EXIT_FAILURE);
	frba_t *frba =
	    (frba_t *) (image + (((fdb->flmap0 >> 16) & 0xff) << 4));

	region_t region = get_region(frba, region_type);
	if (region.size <= 0xfff) {
		fprintf(stderr, "Region %s is disabled in target. Not injecting.\n",
				region_name(region_type));
		exit(EXIT_FAILURE);
	}

	int region_fd = open(region_fname, O_RDONLY);
	if (region_fd == -1) {
		perror("Could not open file");
		exit(EXIT_FAILURE);
	}
	struct stat buf;
	if (fstat(region_fd, &buf) == -1) {
		perror("Could not stat file");
		exit(EXIT_FAILURE);
	}
	int region_size = buf.st_size;

	printf("File %s is %d bytes\n", region_fname, region_size);

	if ( (region_size > region.size) || ((region_type != 1) &&
		(region_size > region.size))) {
		fprintf(stderr, "Region %s is %d(0x%x) bytes. File is %d(0x%x)"
				" bytes. Not injecting.\n",
				region_name(region_type), region.size,
				region.size, region_size, region_size);
		exit(EXIT_FAILURE);
	}

	int offset = 0;
	if ((region_type == 1) && (region_size < region.size)) {
		fprintf(stderr, "Region %s is %d(0x%x) bytes. File is %d(0x%x)"
				" bytes. Padding before injecting.\n",
				region_name(region_type), region.size,
				region.size, region_size, region_size);
		offset = region.size - region_size;
		memset(image + region.base, 0xff, offset);
	}

	if (size < region.base + offset + region_size) {
		fprintf(stderr, "Output file is too small. (%d < %d)\n",
			size, region.base + offset + region_size);
		exit(EXIT_FAILURE);
	}

	if (read(region_fd, image + region.base + offset, region_size)
							!= region_size) {
		perror("Could not read file");
		exit(EXIT_FAILURE);
	}

	close(region_fd);

	printf("Adding %s as the %s section of %s\n",
	       region_fname, region_name(region_type), filename);
	write_image(filename, image, size);
}

static void print_version(void)
{
	printf("ifdtool v%s -- ", IFDTOOL_VERSION);
	printf("Copyright (C) 2011 Google Inc.\n\n");
	printf
	    ("This program is free software: you can redistribute it and/or modify\n"
	     "it under the terms of the GNU General Public License as published by\n"
	     "the Free Software Foundation, version 2 of the License.\n\n"
	     "This program is distributed in the hope that it will be useful,\n"
	     "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	     "GNU General Public License for more details.\n\n"
	     "You should have received a copy of the GNU General Public License\n"
	     "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vhdix?] <filename>\n", name);
	printf("\n"
	       "   -d | --dump:                      dump intel firmware descriptor\n"
	       "   -x | --extract:                   extract intel fd modules\n"
	       "   -i | --inject <region>:<module>   inject file <module> into region <region>\n"
	       "   -s | --spifreq <20|33|50>         set the SPI frequency\n"
	       "   -e | --em100                      set SPI frequency to 20MHz and disable\n"
	       "                                     Dual Output Fast Read Support\n"
	       "   -l | --lock                       Lock firmware descriptor and ME region\n"
	       "   -u | --unlock                     Unlock firmware descriptor and ME region\n"
	       "   -v | --version:                   print the version\n"
	       "   -h | --help:                      print this help\n\n"
	       "<region> is one of Descriptor, BIOS, ME, GbE, Platform\n"
	       "\n");
}

int main(int argc, char *argv[])
{
	int opt, option_index = 0;
	int mode_dump = 0, mode_extract = 0, mode_inject = 0, mode_spifreq = 0;
	int mode_em100 = 0, mode_locked = 0, mode_unlocked = 0;
	char *region_type_string = NULL, *region_fname = NULL;
	int region_type = -1, inputfreq = 0;
	enum spi_frequency spifreq = SPI_FREQUENCY_20MHZ;

	static struct option long_options[] = {
		{"dump", 0, NULL, 'd'},
		{"extract", 0, NULL, 'x'},
		{"inject", 1, NULL, 'i'},
		{"spifreq", 1, NULL, 's'},
		{"em100", 0, NULL, 'e'},
		{"lock", 0, NULL, 'l'},
		{"unlock", 0, NULL, 'u'},
		{"version", 0, NULL, 'v'},
		{"help", 0, NULL, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "dxi:s:eluvh?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'd':
			mode_dump = 1;
			break;
		case 'x':
			mode_extract = 1;
			break;
		case 'i':
			// separate type and file name
			region_type_string = strdup(optarg);
			region_fname = strchr(region_type_string, ':');
			if (!region_fname) {
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			region_fname[0] = '\0';
			region_fname++;
			// Descriptor, BIOS, ME, GbE, Platform
			// valid type?
			if (!strcasecmp("Descriptor", region_type_string))
				region_type = 0;
			else if (!strcasecmp("BIOS", region_type_string))
				region_type = 1;
			else if (!strcasecmp("ME", region_type_string))
				region_type = 2;
			else if (!strcasecmp("GbE", region_type_string))
				region_type = 3;
			else if (!strcasecmp("Platform", region_type_string))
				region_type = 4;
			if (region_type == -1) {
				fprintf(stderr, "No such region type: '%s'\n\n",
					region_type_string);
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			mode_inject = 1;
			break;
		case 's':
			// Parse the requested SPI frequency
			inputfreq = strtol(optarg, NULL, 0);
			switch (inputfreq) {
			case 20:
				spifreq = SPI_FREQUENCY_20MHZ;
				break;
			case 33:
				spifreq = SPI_FREQUENCY_33MHZ;
				break;
			case 50:
				spifreq = SPI_FREQUENCY_50MHZ;
				break;
			default:
				fprintf(stderr, "Invalid SPI Frequency: %d\n",
					inputfreq);
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			mode_spifreq = 1;
			break;
		case 'e':
			mode_em100 = 1;
			break;
		case 'l':
			mode_locked = 1;
			if (mode_unlocked == 1) {
				fprintf(stderr, "Locking/Unlocking FD and ME are mutually exclusive\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'u':
			mode_unlocked = 1;
			if (mode_locked == 1) {
				fprintf(stderr, "Locking/Unlocking FD and ME are mutually exclusive\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		}
	}

	if ((mode_dump + mode_extract + mode_inject +
		(mode_spifreq | mode_em100 | mode_unlocked |
		 mode_locked)) > 1) {
		fprintf(stderr, "You may not specify more than one mode.\n\n");
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((mode_dump + mode_extract + mode_inject + mode_spifreq +
	     mode_em100 + mode_locked + mode_unlocked) == 0) {
		fprintf(stderr, "You need to specify a mode.\n\n");
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if (optind + 1 != argc) {
		fprintf(stderr, "You need to specify a file.\n\n");
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	char *filename = argv[optind];
	int bios_fd = open(filename, O_RDONLY);
	if (bios_fd == -1) {
		perror("Could not open file");
		exit(EXIT_FAILURE);
	}
	struct stat buf;
	if (fstat(bios_fd, &buf) == -1) {
		perror("Could not stat file");
		exit(EXIT_FAILURE);
	}
	int size = buf.st_size;

	printf("File %s is %d bytes\n", filename, size);

	char *image = malloc(size);
	if (!image) {
		printf("Out of memory.\n");
		exit(EXIT_FAILURE);
	}

	if (read(bios_fd, image, size) != size) {
		perror("Could not read file");
		exit(EXIT_FAILURE);
	}

	close(bios_fd);

	if (mode_dump)
		dump_fd(image, size);

	if (mode_extract)
		write_regions(image, size);

	if (mode_inject)
		inject_region(filename, image, size, region_type,
				region_fname);

	if (mode_spifreq)
		set_spi_frequency(filename, image, size, spifreq);

	if (mode_em100)
		set_em100_mode(filename, image, size);

	if(mode_locked)
		lock_descriptor(filename, image, size);

	if (mode_unlocked)
		unlock_descriptor(filename, image, size);

	free(image);

	return 0;
}
