/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <swab.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/acpigen.h>

#include "fw_cfg.h"
#include "fw_cfg_if.h"

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511

static unsigned char fw_cfg_detected = 0xff;
static FWCfgFiles *fw_files;

static int fw_cfg_present(void)
{
	static const char qsig[] = "QEMU";
	unsigned char sig[4];

	if (fw_cfg_detected == 0xff) {
		fw_cfg_get(FW_CFG_SIGNATURE, sig, sizeof(sig));
		fw_cfg_detected = (memcmp(sig, qsig, 4) == 0) ? 1 : 0;
		printk(BIOS_INFO, "QEMU: firmware config interface %s\n",
		       fw_cfg_detected ? "detected" : "not found");
	}
	return fw_cfg_detected;
}

void fw_cfg_get(int entry, void *dst, int dstlen)
{
	outw(entry, FW_CFG_PORT_CTL);
	insb(FW_CFG_PORT_DATA, dst, dstlen);
}

static void fw_cfg_init_file(void)
{
	u32 i, size, count = 0;

	if (fw_files != NULL)
		return;

	fw_cfg_get(FW_CFG_FILE_DIR, &count, sizeof(count));
	count = swab32(count);
	size = count * sizeof(FWCfgFile) + sizeof(count);
	printk(BIOS_DEBUG, "QEMU: %d files in fw_cfg\n", count);
	fw_files = malloc(size);
	fw_cfg_get(FW_CFG_FILE_DIR, fw_files, size);
	fw_files->count = swab32(fw_files->count);
	for (i = 0; i < count; i++) {
		fw_files->f[i].size   = swab32(fw_files->f[i].size);
		fw_files->f[i].select = swab16(fw_files->f[i].select);
		printk(BIOS_DEBUG, "QEMU:     %s [size=%d]\n",
		       fw_files->f[i].name, fw_files->f[i].size);
	}
}

static FWCfgFile *fw_cfg_find_file(const char *name)
{
	int i;

	fw_cfg_init_file();
	for (i = 0; i < fw_files->count; i++)
		if (strcmp(fw_files->f[i].name, name) == 0)
			return fw_files->f + i;
	return NULL;
}

int fw_cfg_check_file(const char *name)
{
	FWCfgFile *file;

	if (!fw_cfg_present())
		return -1;
	file = fw_cfg_find_file(name);
	if (!file)
		return -1;
	return file->size;
}

void fw_cfg_load_file(const char *name, void *dst)
{
	FWCfgFile *file;

	if (!fw_cfg_present())
		return;
	file = fw_cfg_find_file(name);
	if (!file)
		return;
	fw_cfg_get(file->select, dst, file->size);
}

int fw_cfg_max_cpus(void)
{
	unsigned short max_cpus;

	if (!fw_cfg_present())
		return -1;

	fw_cfg_get(FW_CFG_MAX_CPUS, &max_cpus, sizeof(max_cpus));
	return max_cpus;
}

/* ---------------------------------------------------------------------- */
/*
 * Starting with release 1.7 qemu provides acpi tables via fw_cfg.
 * Main advantage is that new (virtual) hardware which needs acpi
 * support JustWorks[tm] without having to patch & update the firmware
 * (seabios, coreboot, ...) accordingly.
 *
 * Qemu provides a etc/table-loader file with instructions for the
 * firmware:
 *   - A "load" instruction to fetch acpi data from fw_cfg.
 *   - A "pointer" instruction to patch a pointer.  This is needed to
 *     get table-to-table references right, it is basically a
 *     primitive dynamic linker for acpi tables.
 *   - A "checksum" instruction to generate acpi table checksums.
 *
 * If a etc/table-loader file is found we'll go try loading the acpi
 * tables from fw_cfg, otherwise we'll fallback to the acpi tables
 * compiled in.
 */

#define BIOS_LINKER_LOADER_FILESZ 56

struct BiosLinkerLoaderEntry {
    uint32_t command;
    union {
        /*
         * COMMAND_ALLOCATE - allocate a table from @alloc.file
         * subject to @alloc.align alignment (must be power of 2)
         * and @alloc.zone (can be HIGH or FSEG) requirements.
         *
         * Must appear exactly once for each file, and before
         * this file is referenced by any other command.
         */
        struct {
            char file[BIOS_LINKER_LOADER_FILESZ];
            uint32_t align;
            uint8_t zone;
        } alloc;

        /*
         * COMMAND_ADD_POINTER - patch the table (originating from
         * @dest_file) at @pointer.offset, by adding a pointer to the table
         * originating from @src_file. 1,2,4 or 8 byte unsigned
         * addition is used depending on @pointer.size.
         */
        struct {
            char dest_file[BIOS_LINKER_LOADER_FILESZ];
            char src_file[BIOS_LINKER_LOADER_FILESZ];
            uint32_t offset;
            uint8_t size;
        } pointer;

        /*
         * COMMAND_ADD_CHECKSUM - calculate checksum of the range specified by
         * @cksum_start and @cksum_length fields,
         * and then add the value at @cksum.offset.
         * Checksum simply sums -X for each byte X in the range
         * using 8-bit math.
         */
        struct {
            char file[BIOS_LINKER_LOADER_FILESZ];
            uint32_t offset;
            uint32_t start;
            uint32_t length;
        } cksum;

        /* padding */
        char pad[124];
    };
} __attribute__((packed));
typedef struct BiosLinkerLoaderEntry BiosLinkerLoaderEntry;

enum {
    BIOS_LINKER_LOADER_COMMAND_ALLOCATE     = 0x1,
    BIOS_LINKER_LOADER_COMMAND_ADD_POINTER  = 0x2,
    BIOS_LINKER_LOADER_COMMAND_ADD_CHECKSUM = 0x3,
};

enum {
    BIOS_LINKER_LOADER_ALLOC_ZONE_HIGH = 0x1,
    BIOS_LINKER_LOADER_ALLOC_ZONE_FSEG = 0x2,
};

unsigned long fw_cfg_acpi_tables(unsigned long start)
{
	BiosLinkerLoaderEntry *s;
	unsigned long *addrs, current;
	uint32_t *ptr4;
	uint64_t *ptr8;
	int rc, i, j, src, dst, max;

	rc = fw_cfg_check_file("etc/table-loader");
	if (rc < 0)
		return 0;

	printk(BIOS_DEBUG, "QEMU: found acpi tables in fw_cfg.\n");

	max = rc / sizeof(*s);
	s = malloc(rc);
	addrs = malloc(max * sizeof(*addrs));
	fw_cfg_load_file("etc/table-loader", s);

	current = start;
	for (i = 0; i < max && s[i].command != 0; i++) {
		switch (s[i].command) {
		case BIOS_LINKER_LOADER_COMMAND_ALLOCATE:
			current += (s[i].alloc.align-1);
			current &= ~(s[i].alloc.align-1);
			printk(BIOS_DEBUG, "QEMU: loading \"%s\" to 0x%lx\n",
			       s[i].alloc.file, current);

			rc = fw_cfg_check_file(s[i].alloc.file);
			if (rc < 0)
				goto err;
			fw_cfg_load_file(s[i].alloc.file, (void*)current);
			addrs[i] = current;
			current += rc;
			break;

		case BIOS_LINKER_LOADER_COMMAND_ADD_POINTER:
			src = -1; dst = -1;
			for (j = 0; j < i; j++) {
				if (s[j].command != BIOS_LINKER_LOADER_COMMAND_ALLOCATE)
					continue;
				if (strcmp(s[j].alloc.file, s[i].pointer.dest_file) == 0)
					dst = j;
				if (strcmp(s[j].alloc.file, s[i].pointer.src_file) == 0)
					src = j;
			}
			if (src == -1 || dst == -1)
				goto err;

			switch (s[i].pointer.size) {
			case 4:
				ptr4 = (uint32_t*)(addrs[dst]+s[i].pointer.offset);
				*ptr4 += addrs[src];
				break;

			case 8:
				ptr8 = (uint64_t*)(addrs[dst]+s[i].pointer.offset);
				*ptr8 += addrs[src];
				break;

			default:
				printk(BIOS_DEBUG, "QEMU: acpi: FIXME: ptr size %d\n",
				       s[i].pointer.size);
				goto err;
			}
			break;

		case BIOS_LINKER_LOADER_COMMAND_ADD_CHECKSUM:
			dst = -1;
			for (j = 0; j < i; j++) {
				if (s[j].command != BIOS_LINKER_LOADER_COMMAND_ALLOCATE)
					continue;
				if (strcmp(s[j].alloc.file, s[i].cksum.file) == 0)
					dst = j;
			}
			if (dst == -1)
				goto err;

			ptr4 = (uint32_t*)(addrs[dst]+s[i].cksum.offset);
			*ptr4 = 0;
			*ptr4 = acpi_checksum((void *)(addrs[dst]+s[i].cksum.start),
					      s[i].cksum.length);
			break;

		default:
			printk(BIOS_DEBUG, "QEMU: acpi: unknown script cmd 0x%x @ %p\n",
			       s[i].command, s+i);
			goto err;
		};
	}

	printk(BIOS_DEBUG, "QEMU: loaded acpi tables from fw_cfg.\n");
	free(s);
	free(addrs);
	return ALIGN(current, 16);

err:
	printk(BIOS_DEBUG, "QEMU: loading acpi tables from fw_cfg failed.\n");
	free(s);
	free(addrs);
	return 0;
}
