/* SPDX-License-Identifier: GPL-2.0-only */

#include <alloca.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <dirent.h>
#include <endian.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/bsd/ipchksum.h>
#include <commonlib/coreboot_tables.h>
#include <commonlib/helpers.h>
#include <commonlib/timestamp_serialized.h>
#include <commonlib/tpm_log_serialized.h>

#ifdef __OpenBSD__
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#include "cbmem_util.h"

struct mapping {
	void *virt;
	size_t offset;
	size_t virt_size;
	unsigned long long phys;
	size_t size;
};

/* File handle used to access /dev/mem */
static int mem_fd;
static struct mapping lbtable_mapping;

static unsigned long long system_page_size(void)
{
	static unsigned long long page_size;

	if (!page_size)
		page_size = getpagesize();

	return page_size;
}

static inline size_t size_to_mib(size_t sz)
{
	return sz >> 20;
}

/* Return mapping of physical address requested. */
static void *mapping_virt(const struct mapping *mapping)
{
	char *v = mapping->virt;

	if (v == NULL)
		return NULL;

	return v + mapping->offset;
}

/* Returns virtual address on success, NULL on error. mapping is filled in. */
static void *map_memory_with_prot(struct mapping *mapping, unsigned long long phys, size_t sz,
				  int prot)
{
	void *v;
	unsigned long long page_size;

	page_size = system_page_size();

	mapping->virt = NULL;
	mapping->offset = phys % page_size;
	mapping->virt_size = sz + mapping->offset;
	mapping->size = sz;
	mapping->phys = phys;

	if (size_to_mib(mapping->virt_size) == 0) {
		debug("Mapping %zuB of physical memory at 0x%llx (requested 0x%llx).\n",
		      mapping->virt_size, phys - mapping->offset, phys);
	} else {
		debug("Mapping %zuMB of physical memory at 0x%llx (requested 0x%llx).\n",
		      size_to_mib(mapping->virt_size), phys - mapping->offset, phys);
	}

	v = mmap(NULL, mapping->virt_size, prot, MAP_SHARED, mem_fd, phys - mapping->offset);

	if (v == MAP_FAILED) {
		debug("Mapping failed %zuB of physical memory at 0x%llx.\n", mapping->virt_size,
		      phys - mapping->offset);
		return NULL;
	}

	mapping->virt = v;

	if (mapping->offset != 0)
		debug("  ... padding virtual address with 0x%zx bytes.\n", mapping->offset);

	return mapping_virt(mapping);
}

/* Convenience helper for the common case of read-only mappings. */
static const void *map_memory(struct mapping *mapping, unsigned long long phys, size_t sz)
{
	return map_memory_with_prot(mapping, phys, sz, PROT_READ);
}

/* Returns 0 on success, < 0 on error. mapping is cleared if successful. */
static int unmap_memory(struct mapping *mapping)
{
	if (mapping->virt == NULL)
		return -1;

	munmap(mapping->virt, mapping->virt_size);
	mapping->virt = NULL;
	mapping->offset = 0;
	mapping->virt_size = 0;

	return 0;
}

/*
 * Some architectures map /dev/mem memory in a way that doesn't support
 * unaligned accesses. Most normal libc memcpy()s aren't safe to use in this
 * case, so build our own which makes sure to never do unaligned accesses on
 * *src (*dest is fine since we never map /dev/mem for writing).
 */
static void *aligned_memcpy(void *dest, const void *src, size_t n)
{
	uint8_t *d = dest;
	const volatile uint8_t *s = src; /* volatile to prevent optimization */

	while ((uintptr_t)s & (sizeof(size_t) - 1)) {
		if (n-- == 0)
			return dest;
		*d++ = *s++;
	}

	while (n >= sizeof(size_t)) {
		*(size_t *)d = *(const volatile size_t *)s;
		d += sizeof(size_t);
		s += sizeof(size_t);
		n -= sizeof(size_t);
	}

	while (n-- > 0)
		*d++ = *s++;

	return dest;
}

/* Return < 0 on error, 0 on success. */
static int parse_cbtable(uint64_t address, size_t table_size)
{
	const uint8_t *buf;
	struct mapping header_mapping;
	size_t req_size;
	size_t i;

	req_size = table_size;
	/* Default to 4 KiB search space. */
	if (req_size == 0)
		req_size = 4 * 1024;

	debug("Looking for coreboot table at %" PRIx64 " %zd bytes.\n", address, req_size);

	buf = map_memory(&header_mapping, address, req_size);

	if (!buf)
		return -1;

	/* look at every 16 bytes */
	for (i = 0; i <= req_size - sizeof(struct lb_header); i += 16) {
		const struct lb_header *lbh;
		struct mapping table_mapping;

		lbh = (const struct lb_header *)&buf[i];
		if (memcmp(lbh->signature, "LBIO", sizeof(lbh->signature)) ||
		    !lbh->header_bytes || ipchksum(lbh, sizeof(*lbh))) {
			continue;
		}

		/* Map in the whole table to parse. */
		if (!map_memory(&table_mapping, address + i,
				lbh->header_bytes + lbh->table_bytes)) {
			debug("Couldn't map in table\n");
			continue;
		}

		const uint8_t *table_contents =
			&((uint8_t *)mapping_virt(&table_mapping))[lbh->header_bytes];
		if (ipchksum(table_contents, lbh->table_bytes) !=
		    lbh->table_checksum) {
			debug("Signature found, but wrong checksum.\n");
			unmap_memory(&table_mapping);
			continue;
		}

		debug("Found at %#" PRIx64 "\n", address + i);

		const struct lb_record *lbr_p;

		for (size_t offset = 0; offset < lbh->table_bytes; offset += lbr_p->size) {
			lbr_p = (const struct lb_record *)&table_contents[offset];
			debug("  coreboot table entry 0x%02x\n", lbr_p->tag);

			if (lbr_p->tag != LB_TAG_FORWARD)
				continue;

			/* This is a forwarding entry. Repeat the search at the new address. */
			struct lb_forward lbf_p = *(const struct lb_forward *)lbr_p;
			debug("    Found forwarding entry.\n");

			const uint64_t next_addr = lbf_p.forward;
			unmap_memory(&header_mapping);
			unmap_memory(&table_mapping);

			return parse_cbtable(next_addr, 0);
		}

		debug("correct coreboot table found.\n");
		unmap_memory(&header_mapping);
		lbtable_mapping = table_mapping;

		return 0;
	}

	unmap_memory(&header_mapping);

	return -1;
}

#if defined(__arm__) || defined(__aarch64__)
static void dt_update_cells(const char *name, int *addr_cells_ptr, int *size_cells_ptr)
{
	if (*addr_cells_ptr >= 0 && *size_cells_ptr >= 0)
		return;

	int buffer;
	size_t nlen = strlen(name);
	char *prop = alloca(nlen + sizeof("/#address-cells"));
	strcpy(prop, name);

	if (*addr_cells_ptr < 0) {
		strcpy(prop + nlen, "/#address-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*addr_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}

	if (*size_cells_ptr < 0) {
		strcpy(prop + nlen, "/#size-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*size_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}
}

static char *dt_find_compat(const char *parent, const char *compat, int *addr_cells_ptr,
			    int *size_cells_ptr)
{
	char *ret = NULL;
	struct dirent *entry;
	DIR *dir;

	if (!(dir = opendir(parent))) {
		perror(parent);
		return NULL;
	}

	/* Loop through all files in the directory (DT node). */
	while ((entry = readdir(dir))) {
		/* We only care about compatible props or subnodes. */
		if (entry->d_name[0] == '.' ||
		    !((entry->d_type & DT_DIR) || !strcmp(entry->d_name, "compatible")))
			continue;

		/* Assemble the file name (on the stack, for speed). */
		size_t plen = strlen(parent);
		char *name = alloca(plen + strlen(entry->d_name) + 2);

		strcpy(name, parent);
		name[plen] = '/';
		strcpy(name + plen + 1, entry->d_name);

		/* If it's a subnode, recurse. */
		if (entry->d_type & DT_DIR) {
			ret = dt_find_compat(name, compat, addr_cells_ptr, size_cells_ptr);

			/* There is only one matching node to find, abort. */
			if (ret) {
				/* Gather cells values on the way up. */
				dt_update_cells(parent, addr_cells_ptr, size_cells_ptr);
				break;
			}
			continue;
		}

		/* If it's a compatible string, see if it's the right one. */
		int fd = open(name, O_RDONLY);
		int clen = strlen(compat);
		char *buffer = alloca(clen + 1);

		if (fd < 0) {
			perror(name);
			continue;
		}

		if (read(fd, buffer, clen + 1) < 0) {
			perror(name);
			close(fd);
			continue;
		}
		close(fd);

		if (!strcmp(compat, buffer)) {
			/* Initialize these to "unset" for the way up. */
			*addr_cells_ptr = *size_cells_ptr = -1;

			/* Can't leave string on the stack or we'll lose it! */
			ret = strdup(parent);
			break;
		}
	}

	closedir(dir);
	return ret;
}
#endif /* defined(__arm__) || defined(__aarch64__) */

bool cbmem_devmem_init(bool writeable)
{
	mem_fd = open("/dev/mem", writeable ? O_RDWR : O_RDONLY, 0);
	if (mem_fd < 0) {
		fprintf(stderr, "Failed to gain memory access: %s\n", strerror(errno));
		return false;
	}

#if defined(__arm__) || defined(__aarch64__)
	int addr_cells, size_cells;
	char *coreboot_node =
		dt_find_compat("/proc/device-tree", "coreboot", &addr_cells, &size_cells);

	if (!coreboot_node) {
		fprintf(stderr, "Could not find 'coreboot' compatible node!\n");
		return false;
	}

	if (addr_cells < 0) {
		fprintf(stderr, "Warning: no #address-cells node in tree!\n");
		addr_cells = 1;
	}

	int nlen = strlen(coreboot_node);
	char *reg = alloca(nlen + sizeof("/reg"));

	strcpy(reg, coreboot_node);
	strcpy(reg + nlen, "/reg");
	free(coreboot_node);

	int fd = open(reg, O_RDONLY);
	if (fd < 0) {
		perror(reg);
		return false;
	}

	int i;
	size_t size_to_read = addr_cells * 4 + size_cells * 4;
	uint8_t *dtbuffer = alloca(size_to_read);
	if (read(fd, dtbuffer, size_to_read) < 0) {
		perror(reg);
		return false;
	}
	close(fd);

	/* No variable-length byte swap function anywhere in C... how sad. */
	uint64_t baseaddr = 0;
	for (i = 0; i < addr_cells * 4; i++) {
		baseaddr <<= 8;
		baseaddr |= *dtbuffer;
		dtbuffer++;
	}
	uint64_t cb_table_size = 0;
	for (i = 0; i < size_cells * 4; i++) {
		cb_table_size <<= 8;
		cb_table_size |= *dtbuffer;
		dtbuffer++;
	}

	parse_cbtable(baseaddr, cb_table_size);
#else
	unsigned long long possible_base_addresses[] = {0, 0xf0000};

	/* Find and parse coreboot table */
	for (size_t j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		if (!parse_cbtable(possible_base_addresses[j], 0))
			break;
	}
#endif

	if (mapping_virt(&lbtable_mapping) == NULL) {
		debug("Table not found.\n");
		return false;
	}

	return true;
}

void cbmem_devmem_terminate(void)
{
	unmap_memory(&lbtable_mapping);
	close(mem_fd);
	mem_fd = -1;
}

/* This is a work-around for a nasty problem introduced by initially having
 * pointer sized entries in the lb_cbmem_ref structures. This caused problems
 * on 64bit x86 systems because coreboot is 32bit on those systems.
 * When the problem was found, it was corrected, but there are a lot of
 * systems out there with a firmware that does not produce the right
 * lb_cbmem_ref structure. Hence we try to autocorrect this issue here.
 */
static struct lb_cbmem_ref parse_cbmem_ref(const struct lb_cbmem_ref *cbmem_ref)
{
	struct lb_cbmem_ref ret;

	aligned_memcpy(&ret, cbmem_ref, sizeof(ret));

	if (cbmem_ref->size < sizeof(*cbmem_ref))
		ret.cbmem_addr = (uint32_t)ret.cbmem_addr;

	debug("      cbmem_addr = %" PRIx64 "\n", ret.cbmem_addr);

	return ret;
}

static uint32_t cbmem_id_to_lb_tag(uint32_t tag)
{
	/* Minimal subset. Expand based on the CBMEM to coreboot table
	   records mapping in lib/coreboot_table.c */
	switch (tag) {
	case CBMEM_ID_TIMESTAMP:
		return LB_TAG_TIMESTAMPS;
	case CBMEM_ID_CONSOLE:
		return LB_TAG_CBMEM_CONSOLE;
	case CBMEM_ID_TPM_CB_LOG:
		return LB_TAG_TPM_CB_LOG;
	}
	return LB_TAG_UNUSED;
}

static bool cbmem_devmem_probe_cbmem_entry(uint32_t id, uint64_t *addr_out, size_t *size_out)
{
	const uint8_t *table;
	const struct lb_header *lbh;
	const uint32_t legacy_tag = cbmem_id_to_lb_tag(id);
	struct lb_cbmem_ref *ref = NULL;
	const struct lb_record *lbr = NULL;

	table = mapping_virt(&lbtable_mapping);
	if (table == NULL)
		die("No correct coreboot table found\n");

	lbh = (const struct lb_header *)table;
	table = &table[lbh->header_bytes];

	/* Fast track for the coreboot table. */
	if (id == CBMEM_ID_CBTABLE) {
		*addr_out = lbtable_mapping.phys;
		*size_out = lbtable_mapping.size;
		return true;
	}

	for (size_t offset = 0; offset < lbh->table_bytes - sizeof(struct lb_cbmem_entry);) {
		lbr = (const void *)(table + offset);
		offset += lbr->size;

		/* Store coreboot table entry for later if CBMEM entry does not exist.
		   CBMEM entry stores size including the reserved area, so prefer it,
		   so more potential data and/or space is available. */
		if (legacy_tag != LB_TAG_UNUSED && lbr->tag == legacy_tag)
			ref = (struct lb_cbmem_ref *)lbr;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		struct lb_cbmem_entry lbe;
		aligned_memcpy(&lbe, lbr, sizeof(lbe));
		if (lbe.id != id)
			continue;

		*addr_out = lbe.address;
		*size_out = lbe.entry_size;
		return true;
	}

	/* No mapping and/or no potential reference means that
	   the requested entry does not exit. */
	if (legacy_tag == LB_TAG_UNUSED || ref == NULL)
		return false;

	debug("Found coreboot table record equivalent of CBMEM entry id: %#x, tag: %#x\n", id,
	      legacy_tag);

	const struct lb_cbmem_ref lbc = parse_cbmem_ref(ref);
	size_t header_map_size = 0;

	/* Process legacy coreboot table entries */
	switch (lbc.tag) {
	case LB_TAG_TIMESTAMPS:
		header_map_size = sizeof(struct timestamp_table);
		break;
	case LB_TAG_CBMEM_CONSOLE:
		header_map_size = sizeof(struct cbmem_console);
		break;
	case LB_TAG_TPM_CB_LOG:
		header_map_size = sizeof(struct tpm_cb_log_table);
		break;
	}

	struct mapping entry_mapping;
	const void *entry_header = NULL;

	entry_header = map_memory(&entry_mapping, lbc.cbmem_addr, header_map_size);
	if (!entry_header)
		die("Unable to map header for coreboot table entry id: %#x\n", legacy_tag);

	*addr_out = lbc.cbmem_addr;

	switch (legacy_tag) {
	case LB_TAG_TIMESTAMPS: {
		const struct timestamp_table *tst_p = entry_header;
		*size_out = sizeof(*tst_p) + tst_p->num_entries * sizeof(tst_p->entries[0]);
		break;
	}
	case LB_TAG_CBMEM_CONSOLE: {
		const struct cbmem_console *console_p = entry_header;
		*size_out = sizeof(*console_p) + console_p->size;
		break;
	}
	case LB_TAG_TPM_CB_LOG: {
		const struct tpm_cb_log_table *tclt_p = entry_header;
		*size_out = sizeof(*tclt_p) + tclt_p->num_entries * sizeof(tclt_p->entries[0]);
		break;
	}
	}

	unmap_memory(&entry_mapping);

	return true;
}

static void fetch_cbmem_entry(const uint32_t id, const uint64_t addr, const size_t size,
			      uint8_t **buf_out)
{
	struct mapping cbmem_mapping;
	const uint8_t *buf = map_memory(&cbmem_mapping, addr, size);
	if (!buf)
		die("Unable to map CBMEM entry id: %#x, size: %zu\n", id, size);

	*buf_out = malloc(size);
	if (!*buf_out) {
		unmap_memory(&cbmem_mapping);
		die("Unable to allocate memory for CBMEM entry id: %#x, size: %zu\n", id, size);
	}

	aligned_memcpy(*buf_out, buf, size);
	unmap_memory(&cbmem_mapping);
}

bool cbmem_devmem_get_cbmem_entry(uint32_t id, uint8_t **buf_out, size_t *size_out,
				  uint64_t *addr_out)
{
	uint64_t addr;
	size_t size;

	if (!cbmem_devmem_probe_cbmem_entry(id, &addr, &size)) {
		debug("CBMEM entry not found. CBMEM id: %#x\n", id);
		return false;
	}

	fetch_cbmem_entry(id, addr, size, buf_out);

	if (size_out)
		*size_out = size;
	if (addr_out)
		*addr_out = addr;

	return true;
}

bool cbmem_devmem_write_cbmem_entry(uint32_t id, uint8_t *buf, size_t buf_size)
{
	uint64_t addr = 0;
	size_t size = 0;
	uint8_t *origin_buf = NULL;
	struct mapping mapping;

	if (!cbmem_devmem_probe_cbmem_entry(id, &addr, &size)) {
		debug("CBMEM entry not found. CBMEM id: %#x\n", id);
		return false;
	}

	if (buf_size > size)
		die("Attempting to write %zu bytes to CBMEM entry id: %#x of %zu bytes. Operation not possible.\n",
		    buf_size, id, size);

	origin_buf = map_memory_with_prot(&mapping, addr, size, PROT_READ | PROT_WRITE);
	if (!origin_buf)
		die("Unable to map CBMEM entry id: %#x, size: %zu for read-write access.\n", id,
		    size);

	aligned_memcpy(origin_buf, buf, buf_size);
	unmap_memory(&mapping);

	return true;
}

void cbmem_devmem_foreach_cbmem_entry(cbmem_iterator_callback cb, void *data,
				      bool with_contents)
{
	uint8_t *table = NULL;
	size_t table_size = 0;

	if (!cbmem_devmem_get_cbmem_entry(CBMEM_ID_CBTABLE, &table, &table_size, NULL))
		die("coreboot table not found.\n");

	const struct lb_header *lbh = (const struct lb_header *)table;
	const struct lb_record *lbr = NULL;
	bool should_iteration_end = false;
	size_t offset = 0;
	while (offset < lbh->table_bytes - sizeof(struct lb_cbmem_entry) &&
	       should_iteration_end == false) {
		lbr = (const struct lb_record *)&table[lbh->header_bytes + offset];
		offset += lbr->size;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		const struct lb_cbmem_entry *lbe = (const struct lb_cbmem_entry *)lbr;
		uint8_t *buf = NULL;
		if (with_contents)
			fetch_cbmem_entry(lbe->id, lbe->address, lbe->entry_size, &buf);

		should_iteration_end = cb(lbe->id, lbe->address, buf, lbe->entry_size, data);

		if (with_contents)
			free(buf);
	}

	free(table);
}
