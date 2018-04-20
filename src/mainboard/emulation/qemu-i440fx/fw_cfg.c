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
 */

#include <compiler.h>
#include <string.h>
#include <swab.h>
#include <smbios.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/acpigen.h>
#include <commonlib/endian.h>

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
 * Starting with release 1.7 qemu provides ACPI tables via fw_cfg.
 * Main advantage is that new (virtual) hardware which needs acpi
 * support JustWorks[tm] without having to patch & update the firmware
 * (seabios, coreboot, ...) accordingly.
 *
 * Qemu provides a etc/table-loader file with instructions for the
 * firmware:
 *   - A "load" instruction to fetch ACPI data from fw_cfg.
 *   - A "pointer" instruction to patch a pointer.  This is needed to
 *     get table-to-table references right, it is basically a
 *     primitive dynamic linker for ACPI tables.
 *   - A "checksum" instruction to generate ACPI table checksums.
 *
 * If a etc/table-loader file is found we'll go try loading the acpi
 * tables from fw_cfg, otherwise we'll fallback to the ACPI tables
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
} __packed;
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
	uint8_t *ptr;
	int rc, i, j, src, dst, max;

	rc = fw_cfg_check_file("etc/table-loader");
	if (rc < 0)
		return 0;

	printk(BIOS_DEBUG, "QEMU: found ACPI tables in fw_cfg.\n");

	max = rc / sizeof(*s);
	s = malloc(rc);
	addrs = malloc(max * sizeof(*addrs));
	fw_cfg_load_file("etc/table-loader", s);

	current = start;
	for (i = 0; i < max && s[i].command != 0; i++) {
		void *cksum_data;
		uint32_t cksum;
		uint32_t addr4;
		uint64_t addr8;
		switch (s[i].command) {
		case BIOS_LINKER_LOADER_COMMAND_ALLOCATE:
			current = ALIGN(current, s[i].alloc.align);
			rc = fw_cfg_check_file(s[i].alloc.file);
			if (rc < 0)
				goto err;
			printk(BIOS_DEBUG, "QEMU: loading \"%s\" to 0x%lx (len %d)\n",
			       s[i].alloc.file, current, rc);
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
				ptr = (uint8_t *)addrs[dst];
				ptr += s[i].pointer.offset;
				addr4 = read_le32(ptr);
				addr4 += addrs[src];
				write_le32(ptr, addr4);
				break;

			case 8:
				ptr = (uint8_t *)addrs[dst];
				ptr += s[i].pointer.offset;
				addr8 = read_le64(ptr);
				addr8 += addrs[src];
				write_le64(ptr, addr8);
				break;

			default:
				/*
				 * Should not happen.  ACPI knows 1 and 2 byte ptrs
				 * too, but we are operating with 32bit offsets which
				 * would simply not fit in there ...
				 */
				printk(BIOS_DEBUG, "QEMU: acpi: unimplemented ptr size %d\n",
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

			ptr = (uint8_t *)(addrs[dst] + s[i].cksum.offset);
			cksum_data = (void *)(addrs[dst] + s[i].cksum.start);
			cksum = acpi_checksum(cksum_data, s[i].cksum.length);
			write_le8(ptr, cksum);
			break;

		default:
			printk(BIOS_DEBUG, "QEMU: acpi: unknown script cmd 0x%x @ %p\n",
			       s[i].command, s+i);
			goto err;
		};
	}

	printk(BIOS_DEBUG, "QEMU: loaded ACPI tables from fw_cfg.\n");
	free(s);
	free(addrs);
	return ALIGN(current, 16);

err:
	printk(BIOS_DEBUG, "QEMU: loading ACPI tables from fw_cfg failed.\n");
	free(s);
	free(addrs);
	return 0;
}

/* ---------------------------------------------------------------------- */
/* pick up smbios information from fw_cfg                                 */

static const char *type1_manufacturer;
static const char *type1_product_name;
static const char *type1_version;
static const char *type1_serial_number;
static const char *type1_family;
static u8 type1_uuid[16];

static void fw_cfg_smbios_init(void)
{
	static int done = 0;
	uint16_t i, count = 0;
	FwCfgSmbios entry;
	char *buf;

	if (done)
		return;
	done = 1;

	fw_cfg_get(FW_CFG_SMBIOS_ENTRIES, &count, sizeof(count));
	for (i = 0; i < count; i++) {
		insb(FW_CFG_PORT_DATA, &entry, sizeof(entry));
		buf = malloc(entry.length - sizeof(entry));
		insb(FW_CFG_PORT_DATA, buf, entry.length - sizeof(entry));
		if (entry.headertype == SMBIOS_FIELD_ENTRY &&
		    entry.tabletype == 1) {
			switch (entry.fieldoffset) {
			case offsetof(struct smbios_type1, manufacturer):
				type1_manufacturer = strdup(buf);
				break;
			case offsetof(struct smbios_type1, product_name):
				type1_product_name = strdup(buf);
				break;
			case offsetof(struct smbios_type1, version):
				type1_version = strdup(buf);
				break;
			case offsetof(struct smbios_type1, serial_number):
				type1_serial_number = strdup(buf);
				break;
			case offsetof(struct smbios_type1, family):
				type1_family = strdup(buf);
				break;
			case offsetof(struct smbios_type1, uuid):
				memcpy(type1_uuid, buf, 16);
				break;
			}
		}
		free(buf);
	}
}

static unsigned long smbios_next(unsigned long current)
{
	struct smbios_type0 *t0;
	int l, count = 0;
	char *s;

	t0 = (void*)current;
	current += t0->length;
	for (;;) {
		s = (void*)current;
		l = strlen(s);
		if (!l)
			return current + (count ? 1 : 2);
		current += l + 1;
		count++;
	}
}

/*
 * Starting with version 2.1 qemu provides a full set of smbios tables
 * for the virtual hardware emulated, except type 0 (bios information).
 *
 * What we are going to do here is find the type0 table, keep it, and
 * override everything else generated by coreboot with the qemu smbios
 * tables.
 *
 * It's a bit hackish, but qemu is a special case (compared to real
 * hardware) and this way we don't need special qemu support in the
 * generic smbios code.
 */
unsigned long fw_cfg_smbios_tables(int *handle, unsigned long *current)
{
	struct smbios_type0 *t0;
	unsigned long start, end;
	int len, ret, i, count = 1;
	char *str;

	len = fw_cfg_check_file("etc/smbios/smbios-tables");
	if (len < 0)
		return 0;
	printk(BIOS_DEBUG, "QEMU: found smbios tables in fw_cfg (len %d).\n", len);

	/*
	 * Search backwards for "coreboot" (first string in type0 table,
	 * see src/arch/x86/boot/smbios.c), then find type0 table.
	 */
	for (i = 0; i < 16384; i++) {
		str = (char*)(*current - i);
		if (strcmp(str, "coreboot") == 0)
			break;
	}
	if (i == 16384)
		return 0;
	i += sizeof(struct smbios_type0) - 2;
	t0 = (struct smbios_type0*)(*current - i);
	if (t0->type != SMBIOS_BIOS_INFORMATION || t0->handle != 0)
		return 0;
	printk(BIOS_DEBUG, "QEMU: coreboot type0 table found at 0x%lx.\n",
	       *current - i);
	start = smbios_next(*current - i);

	/*
	 * Fetch smbios tables from qemu, go find the end marker.
	 * We'll exclude the end marker as coreboot will add one.
	 */
	printk(BIOS_DEBUG, "QEMU: loading smbios tables to 0x%lx\n", start);
	fw_cfg_load_file("etc/smbios/smbios-tables", (void*)start);
	end = start;
	do {
		t0 = (struct smbios_type0*)end;
		if (t0->type == SMBIOS_END_OF_TABLE)
			break;
		end = smbios_next(end);
		count++;
	} while (end < start + len);

	/* final fixups. */
	ret = end - *current;
	*current = end;
	*handle = count;
	return ret;
}

const char *smbios_mainboard_manufacturer(void)
{
	fw_cfg_smbios_init();
	return type1_manufacturer ?: CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

const char *smbios_mainboard_product_name(void)
{
	fw_cfg_smbios_init();
	return type1_product_name ?: CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

const char *smbios_mainboard_version(void)
{
	fw_cfg_smbios_init();
	return type1_version ?: CONFIG_MAINBOARD_VERSION;
}

const char *smbios_mainboard_serial_number(void)
{
	fw_cfg_smbios_init();
	return type1_serial_number ?: CONFIG_MAINBOARD_SERIAL_NUMBER;
}

void smbios_mainboard_set_uuid(u8 *uuid)
{
	fw_cfg_smbios_init();
	memcpy(uuid, type1_uuid, 16);
}
