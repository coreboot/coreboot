/* SPDX-License-Identifier: GPL-2.0-only */

#include <endian.h>
#include <stdlib.h>
#include <string.h>
#include <smbios.h>
#include <console/console.h>
#include <arch/io.h>
#include <acpi/acpi.h>
#include <commonlib/endian.h>

#include "fw_cfg.h"
#include "fw_cfg_if.h"

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511
#define FW_CFG_DMA_ADDR_HIGH  0x0514
#define FW_CFG_DMA_ADDR_LOW   0x0518

static int fw_cfg_detected;
static uint8_t fw_ver;

static void fw_cfg_dma(int control, void *buf, int len);

static int fw_cfg_present(void)
{
	static const char qsig[] = "QEMU";
	unsigned char sig[FW_CFG_SIG_SIZE];
	int detected = 0;

	if (fw_cfg_detected == 0) {
		fw_cfg_get(FW_CFG_SIGNATURE, sig, sizeof(sig));
		detected = memcmp(sig, qsig, FW_CFG_SIG_SIZE) == 0;
		printk(BIOS_INFO, "QEMU: firmware config interface %s\n",
				detected ? "detected" : "not found");
		if (detected) {
			fw_cfg_get(FW_CFG_ID, &fw_ver, sizeof(fw_ver));
			printk(BIOS_INFO, "Firmware config version id: %d\n", fw_ver);
		}
		fw_cfg_detected = detected + 1;
	}
	return fw_cfg_detected - 1;
}

static void fw_cfg_select(uint16_t entry)
{
	outw(entry, FW_CFG_PORT_CTL);
}

static void fw_cfg_read(void *dst, int dstlen)
{
	if (fw_ver & FW_CFG_VERSION_DMA)
		fw_cfg_dma(FW_CFG_DMA_CTL_READ, dst, dstlen);
	else
		insb(FW_CFG_PORT_DATA, dst, dstlen);
}

void fw_cfg_get(uint16_t entry, void *dst, int dstlen)
{
	fw_cfg_select(entry);
	fw_cfg_read(dst, dstlen);
}

static int fw_cfg_find_file(FWCfgFile *file, const char *name)
{
	uint32_t count = 0;

	fw_cfg_select(FW_CFG_FILE_DIR);
	fw_cfg_read(&count, sizeof(count));
	count = be32_to_cpu(count);

	for (int i = 0; i < count; i++) {
		fw_cfg_read(file, sizeof(*file));
		if (strcmp(file->name, name) == 0) {
			file->size = be32_to_cpu(file->size);
			file->select = be16_to_cpu(file->select);
			printk(BIOS_INFO, "QEMU: firmware config: Found '%s'\n", name);
			return 0;
		}
	}
	printk(BIOS_INFO, "QEMU: firmware config: Couldn't find '%s'\n", name);
	return -1;
}

int fw_cfg_check_file(FWCfgFile *file, const char *name)
{
	if (!fw_cfg_present())
		return -1;
	return fw_cfg_find_file(file, name);
}

static int fw_cfg_e820_select(uint32_t *size)
{
	FWCfgFile file;

	if (!fw_cfg_present() || fw_cfg_find_file(&file, "etc/e820"))
		return -1;
	fw_cfg_select(file.select);
	*size = file.size;
	return 0;
}

static int fw_cfg_e820_read(FwCfgE820Entry *entry, uint32_t *size,
		uint32_t *pos)
{
	if (*pos + sizeof(*entry) > *size)
		return -1;

	fw_cfg_read(entry, sizeof(*entry));
	*pos += sizeof(*entry);
	return 0;
}

/* returns tolud on success or 0 on failure */
uintptr_t fw_cfg_tolud(void)
{
	FwCfgE820Entry e;
	uint64_t top = 0;
	uint32_t size = 0, pos = 0;

	if (fw_cfg_e820_select(&size) == 0) {
		while (!fw_cfg_e820_read(&e, &size, &pos)) {
			uint64_t limit = e.address + e.length;
			if (e.type == 1 && limit < 4ULL * GiB && limit > top)
				top = limit;
		}
	}
	return (uintptr_t)top;
}

int fw_cfg_max_cpus(void)
{
	unsigned short max_cpus;

	if (!fw_cfg_present())
		return 0;

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
	FWCfgFile f;
	BiosLinkerLoaderEntry *s;
	unsigned long *addrs, current;
	uint8_t *ptr;
	int i, j, src, dst, max;

	if (fw_cfg_check_file(&f, "etc/table-loader"))
		return 0;

	printk(BIOS_DEBUG, "QEMU: found ACPI tables in fw_cfg.\n");

	max = f.size / sizeof(*s);
	s = malloc(f.size);
	addrs = malloc(max * sizeof(*addrs));
	fw_cfg_get(f.select, s, f.size);

	current = start;
	for (i = 0; i < max && s[i].command != 0; i++) {
		void *cksum_data;
		uint32_t cksum;
		uint32_t addr4;
		uint64_t addr8;
		switch (s[i].command) {
		case BIOS_LINKER_LOADER_COMMAND_ALLOCATE:
			current = ALIGN_UP(current, s[i].alloc.align);
			if (fw_cfg_check_file(&f, s[i].alloc.file))
				goto err;

			printk(BIOS_DEBUG, "QEMU: loading \"%s\" to 0x%lx (len %d)\n",
			       s[i].alloc.file, current, f.size);
			fw_cfg_get(f.select, (void *)current, f.size);
			addrs[i] = current;
			current += f.size;
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
	return ALIGN_UP(current, 16);

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
	struct smbios_header *header;
	int l, count = 0;
	char *s;

	header = (void *)current;
	current += header->length;
	for (;;) {
		s = (void *)current;
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
	FWCfgFile f;
	struct smbios_header *header;
	unsigned long start, end;
	int ret, i, count = 1;
	char *str;

	if (fw_cfg_check_file(&f, "etc/smbios/smbios-tables"))
		return 0;

	printk(BIOS_DEBUG, "QEMU: found smbios tables in fw_cfg (len %d).\n", f.size);

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
	header = (struct smbios_header *)(*current - i);
	if (header->type != SMBIOS_BIOS_INFORMATION || header->handle != 0)
		return 0;
	printk(BIOS_DEBUG, "QEMU: coreboot type0 table found at 0x%lx.\n",
	       *current - i);
	start = smbios_next(*current - i);

	/*
	 * Fetch smbios tables from qemu, go find the end marker.
	 * We'll exclude the end marker as coreboot will add one.
	 */
	printk(BIOS_DEBUG, "QEMU: loading smbios tables to 0x%lx\n", start);
	fw_cfg_get(f.select, (void *)start, f.size);
	end = start;
	do {
		header = (struct smbios_header *)end;
		if (header->type == SMBIOS_END_OF_TABLE)
			break;
		end = smbios_next(end);
		count++;
	} while (end < start + f.size);

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

void smbios_system_set_uuid(u8 *uuid)
{
	fw_cfg_smbios_init();
	memcpy(uuid, type1_uuid, 16);
}

/*
 * Configure DMA setup
 */

static void fw_cfg_dma(int control, void *buf, int len)
{
	volatile FwCfgDmaAccess dma;
	uintptr_t dma_desc_addr;
	uint32_t dma_desc_addr_hi, dma_desc_addr_lo;

	dma.control = be32_to_cpu(control);
	dma.length  = be32_to_cpu(len);
	dma.address = be64_to_cpu((uintptr_t)buf);

	dma_desc_addr = (uintptr_t)&dma;
	dma_desc_addr_lo = (uint32_t)(dma_desc_addr & 0xFFFFFFFFU);
	dma_desc_addr_hi = sizeof(uintptr_t) > sizeof(uint32_t)
				? (uint32_t)(dma_desc_addr >> 32) : 0;

	// Skip writing high half if unnecessary.
	if (dma_desc_addr_hi)
		outl(be32_to_cpu(dma_desc_addr_hi), FW_CFG_DMA_ADDR_HIGH);
	outl(be32_to_cpu(dma_desc_addr_lo), FW_CFG_DMA_ADDR_LOW);

	while (be32_to_cpu(dma.control) & ~FW_CFG_DMA_CTL_ERROR)
		;
}
