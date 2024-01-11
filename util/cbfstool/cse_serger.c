/* SPDX-License-Identifier: GPL-2.0-only */
/* CSE Serger - Tool for stitching Intel CSE components */

#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "cse_serger.h"

#define NO_PARTITION_TYPE	(-1)

static struct params {
	bool print_sub_parts;
	const char *partition_name;
	int partition_type;
	const char *output_dir;
	const char *image_name;
	const char *version_str;
	const char *input_file;
	struct region layout_regions[BP_TOTAL];
	const char *layout_files[BP_TOTAL];
} params;

static const struct {
	const char *version_str;
	const struct bpdt_ops *ops;
} bpdt_ops_table[] = {
	{ "1.6", &bpdt_1_6_ops },
	{ "1.7", &bpdt_1_7_ops },
};

static const struct {
	enum subpart_hdr_version version;
	const struct subpart_hdr_ops *ops;
} subpart_hdr_ops_table[] = {
	{ SUBPART_HDR_VERSION_1, &subpart_hdr_1_ops },
	{ SUBPART_HDR_VERSION_2, &subpart_hdr_2_ops },
};

static const struct {
	enum subpart_entry_version version;
	const struct subpart_entry_ops *ops;
} subpart_entry_ops_table[] = {
	{ SUBPART_ENTRY_VERSION_1, &subpart_entry_1_ops },
};

enum bpdt_entry_type {
	SMIP = 0,
	CSE_RBE = 1,
	CSE_BUP = 2,
	UCODE = 3,
	IBB = 4,
	S_BPDT = 5,
	OBB = 6,
	CSE_MAIN = 7,
	ISH = 8,
	CSE_IDLM = 9,
	IFP_OVERRIDE = 10,
	UTOK = 11,
	UFS_PHY = 12,
	UFS_GPP = 13,
	PMC = 14,
	IUNIT = 15,
	NVM_CFG = 16,
	UEP = 17,
	OEM_KM = 20,
	PAVP = 22,
	IOM_FW = 23,
	NPHY_FW = 24,
	TBT_FW = 25,
	ICC = 32,

	MAX_SUBPARTS,
};

static struct {
	struct buffer input_buff;

	const struct bpdt_ops *bpdt_ops;
	const struct subpart_hdr_ops *subpart_hdr_ops;
	const struct subpart_entry_ops *subpart_entry_ops;

	bpdt_hdr_ptr bpdt_hdr;
	cse_layout_ptr cse_layout;
	struct bpdt_entry bpdt_entries[MAX_SUBPARTS];
	struct buffer subpart_buff[MAX_SUBPARTS];
	bool repack;
	size_t file_end_offset;
} ifwi;

#define SUBPART_WITH_ALT(_index, _rname, _name, _aname)	\
				[_index] = { _rname, _name, _aname }
#define SUBPART(_index, _rname, _name)				\
				SUBPART_WITH_ALT(_index, _rname, _name, "")

static const struct {
	const char *readable_name;
	const char *name;
	const char *alt_name;
} subparts[] = {
	SUBPART(SMIP, "OEM SMIP", "SMIP"),
	SUBPART(CSE_RBE, "CSE RBE", "RBEP"),
	SUBPART_WITH_ALT(CSE_BUP, "CSE BUP", "FTPR", "MFTP"),
	SUBPART(UCODE, "Microcode", "UCOD"),
	SUBPART(IBB, "Initial Boot Block", "IBBP"),
	SUBPART(S_BPDT, "Secondary BPDT", "SBDT"),
	SUBPART(OBB, "OEM Boot Block", "OBBP"),
	SUBPART(CSE_MAIN, "CSE Main", "NFTP"),
	SUBPART(ISH, "ISH Firmware", "ISHP"),
	SUBPART(CSE_IDLM, "CSE IDLM", "DLMP"),
	SUBPART(IFP_OVERRIDE, "IFP override", "IFPP"),
	SUBPART(UTOK, "Debug tokens", "UTOK"),
	SUBPART(UFS_PHY, "UFS Phy", "UFSP"),
	SUBPART(UFS_GPP, "UFS GPP", "UFSG"),
	SUBPART(PMC, "PMC Firmware", "PMCP"),
	SUBPART(IUNIT, "IUNIT Firmware", "IUNP"),
	SUBPART(NVM_CFG, "NVM CFG", "NVMC"),
	SUBPART(UEP, "UEP", "UEPP"),
	SUBPART(OEM_KM, "OEM Key Manifest", "OEMP"),
	SUBPART(PAVP, "PAVP", "PAVP"),
	SUBPART(IOM_FW, "IOM Firmware", "IOMP"),
	SUBPART(NPHY_FW, "NPHY Firmware", "NPHY"),
	SUBPART(TBT_FW, "TBT Firmware", "TBTP"),
	SUBPART(ICC, "ICC Firmware", "PCHC"),
};

static const char *subpart_readable_name(enum bpdt_entry_type type)
{
	return subparts[type].readable_name;
}

static const char *subpart_name(enum bpdt_entry_type type)
{
	return subparts[type].name;
}

static const char *subpart_alt_name(enum bpdt_entry_type type)
{
	return subparts[type].alt_name;
}

static struct buffer *subpart_buff(int type)
{
	return &ifwi.subpart_buff[type];
}

static int subpart_get_type_from_name(const char *name)
{
	int i;

	for (i = 0; i < MAX_SUBPARTS; i++) {
		if (subpart_name(i) == NULL)
			continue;

		if (!strcmp(subpart_name(i), name))
			return i;

		if (!strcmp(subpart_alt_name(i), name))
			return i;
	}

	return -1;
}

static const struct bpdt_ops *get_bpdt_ops(const struct buffer *buff)
{
	assert(buff || params.version_str);

	for (size_t i = 0; i < ARRAY_SIZE(bpdt_ops_table); i++) {
		if (params.version_str) {
			if (!strcmp(params.version_str, bpdt_ops_table[i].version_str))
				return bpdt_ops_table[i].ops;
			else
				continue;
		}
		if (bpdt_ops_table[i].ops->match_version(buff))
			return bpdt_ops_table[i].ops;
	}

	return NULL;
}

static const struct subpart_hdr_ops *get_subpart_hdr_ops(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(subpart_hdr_ops_table); i++) {
		if (subpart_hdr_ops_table[i].version == ifwi.bpdt_ops->subpart_hdr_version)
			return subpart_hdr_ops_table[i].ops;
	}

	return NULL;
}

static const struct subpart_entry_ops *get_subpart_entry_ops(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(subpart_entry_ops_table); i++) {
		if (subpart_entry_ops_table[i].version == ifwi.bpdt_ops->subpart_entry_version)
			return subpart_entry_ops_table[i].ops;
	}

	return NULL;
}

static int subpart_read(struct buffer *input_buff)
{
	size_t input_size = buffer_size(input_buff);
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];
	struct buffer *buff;

	for (size_t i = 0; i < ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr); i++, e++) {
		if (e->size == 0)
			continue;

		if (e->type >= MAX_SUBPARTS) {
			ERROR("Invalid part type(%d)\n", e->type);
			return -1;
		}

		if (e->offset + e->size > input_size) {
			ERROR("Part(%d) exceeds file size. Part offset=0x%x, Part size = 0x%x, File size = 0x%zx\n",
			      e->type, e->offset, e->size, input_size);
			return -1;
		}

		buff = subpart_buff(e->type);
		if (buffer_size(buff) != 0) {
			ERROR("Multiple subparts of same type(%d %s)!\n",
			      e->type, subpart_name(e->type));
			return -1;
		}

		buffer_splice(buff, input_buff, e->offset, e->size);
	}

	return 0;
}

static struct bpdt_entry *find_bpdt_entry(uint32_t type)
{
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];

	for (size_t i = 0; i < ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr); i++, e++) {
		if (e->type == type)
			return e;
	}

	return NULL;
}

static struct bpdt_entry *new_bpdt_entry(void)
{
	size_t count = ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr);
	if (count == MAX_SUBPARTS) {
		ERROR("No space for new BPDT entry!\n");
		return NULL;
	}

	ifwi.bpdt_ops->inc_entry_count(ifwi.bpdt_hdr);

	return &ifwi.bpdt_entries[count];
}

static void set_file_end_offset(struct buffer *buff)
{
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];
	size_t end_offset;
	size_t count = ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr);

	ifwi.file_end_offset = ALIGN_UP(buffer_offset(buff), BUFF_SIZE_ALIGN);

	for (size_t i = 0; i < count; i++, e++) {
		end_offset = e->offset + e->size;
		if (end_offset > ifwi.file_end_offset)
			ifwi.file_end_offset = end_offset;

	}
}

static void read_bpdt_entries(struct buffer *buff)
{
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];
	size_t count = ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr);

	for (size_t i = 0; i < count; i++, e++) {
		READ_MEMBER(buff, e->type);
		READ_MEMBER(buff, e->offset);
		READ_MEMBER(buff, e->size);
	}
}

static int write_bpdt_entries(struct buffer *buff)
{
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];
	size_t count = ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr);

	if (buffer_size(buff) < count * sizeof(*e)) {
		ERROR("Not enough buffer space for bpdt entries!\n");
		return -1;
	}

	for (size_t i = 0; i < count; i++, e++) {
		WRITE_MEMBER(buff, e->type);
		WRITE_MEMBER(buff, e->offset);
		WRITE_MEMBER(buff, e->size);
	}

	return 0;
}

static void print_bpdt_entries(void)
{
	const size_t count = ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr);

	if (count == 0)
		return;

	const struct bpdt_entry *e = &ifwi.bpdt_entries[0];

	printf("\n * BPDT entries\n");

	printf("%-25s%-25s%-25s%-25s%-25s%-25s\n", "Entry #",
	       "Partition Name", "Human readable name", "Type", "Offset", "Size");

	printf("===================================================================="
	       "====================================================================\n");

	for (size_t i = 0; i < count; i++) {
		printf("%-25zd%-25s%-25s%-25d0x%-23x0x%-23x"
		       "\n", i+1, subpart_name(e[i].type), subpart_readable_name(e[i].type),
		       e[i].type, e[i].offset, e[i].size);
	}

	printf("===================================================================="
	       "====================================================================\n");
}

static int ifwi_parse(const char *image_name)
{
	struct buffer *input_buff = &ifwi.input_buff;
	struct buffer bpdt_buff;

	if (buffer_from_file(input_buff, image_name)) {
		ERROR("Failed to read input file %s\n", image_name);
		return -1;
	}

	buffer_clone(&bpdt_buff, input_buff);

	ifwi.bpdt_ops = get_bpdt_ops(&bpdt_buff);
	if (!ifwi.bpdt_ops) {
		ERROR("No matching bpdt_ops!\n");
		return -1;
	}

	ifwi.bpdt_hdr = ifwi.bpdt_ops->read_hdr(&bpdt_buff);
	if (ifwi.bpdt_hdr == NULL)
		return -1;

	read_bpdt_entries(&bpdt_buff);
	set_file_end_offset(&bpdt_buff);

	if (!ifwi.bpdt_ops->validate_checksum(ifwi.bpdt_hdr, &ifwi.bpdt_entries[0])) {
		ERROR("Checksum failed!\n");
		return -1;
	}

	ifwi.subpart_hdr_ops = get_subpart_hdr_ops();
	if (ifwi.subpart_hdr_ops == NULL) {
		ERROR("No matching subpart_hdr_ops for given BPDT!\n");
		return -1;
	}

	ifwi.subpart_entry_ops = get_subpart_entry_ops();
	if (ifwi.subpart_entry_ops == NULL) {
		ERROR("No matching subpart_entry_ops for given BPDT!\n");
		return -1;
	}

	return subpart_read(&ifwi.input_buff);
}

static int subpart_write(struct buffer *buff)
{
	struct bpdt_entry *e;
	struct buffer *s_buff;

	for (size_t i = 0; i < ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr); i++) {
		e = &ifwi.bpdt_entries[i];

		if (e->size == 0)
			continue;

		if (e->offset + e->size > buffer_size(buff)) {
			ERROR("Subpart end(0x%x) overflows buffer size(0x%zx)\n",
			      e->offset + e->size, buffer_size(buff));
			return -1;
		}

		s_buff = subpart_buff(e->type);

		if (buffer_size(s_buff) != e->size) {
			ERROR("Subpart buffer size does not match BPDT entry size!\n");
			return -1;
		}

		memcpy(buffer_get(buff) + e->offset, buffer_get(s_buff), e->size);
	}

	return 0;
}

static int ifwi_repack(void)
{
	if (!ifwi.repack)
		return 0;

	struct buffer output_buff;
	const size_t size = ifwi.file_end_offset;
	struct buffer bpdt_buff;

	if (buffer_create(&output_buff, size, "Output IFWI")) {
		ERROR("Unable to allocate output buff!\n");
		return -1;
	}

	buffer_clone(&bpdt_buff, &output_buff);

	ifwi.bpdt_ops->update_checksum(ifwi.bpdt_hdr, &ifwi.bpdt_entries[0]);

	if (ifwi.bpdt_ops->write_hdr(&bpdt_buff, ifwi.bpdt_hdr))
		return -1;

	if (write_bpdt_entries(&bpdt_buff))
		return -1;

	subpart_write(&output_buff);

	if (buffer_write_file(&output_buff, params.image_name)) {
		ERROR("File write error!\n");
		return -1;
	}

	printf("Image written successfully to %s.\n", params.image_name);
	return 0;
}

static bool should_process_partition(int type)
{
	if (params.partition_name) {
		const char *name = subpart_name(type);

		if (!name)
			return false;

		if (strcmp(params.partition_name, name))
			return false;
	} else if (params.partition_type != NO_PARTITION_TYPE) {
		if (params.partition_type != type)
			return false;
	}

	return true;
}

static int process_entries(int (*fn)(const struct bpdt_entry *e))
{
	struct bpdt_entry *e = &ifwi.bpdt_entries[0];
	bool found = false;

	for (size_t i = 0; i < ifwi.bpdt_ops->get_entry_count(ifwi.bpdt_hdr); i++, e++) {
		if (e->size == 0)
			continue;

		if (!should_process_partition(e->type))
			continue;

		if (fn(e))
			return -1;

		found = true;
	}

	if (!found && params.partition_name) {
		ERROR("Partition %s not found!\n", params.partition_name);
		return -1;
	}

	if (!found && params.partition_type != NO_PARTITION_TYPE) {
		ERROR("Partition type %d not found!\n", params.partition_type);
		return -1;
	}

	return 0;
}

static int print_subpart(const struct bpdt_entry *e)
{
	struct buffer buff;
	subpart_hdr_ptr hdr;

	printf("\n\n * Subpart entry #%d(%s)\n", e->type, subpart_readable_name(e->type));

	buffer_clone(&buff, subpart_buff(e->type));
	hdr = ifwi.subpart_hdr_ops->read(&buff);
	if (!hdr) {
		ERROR("Failed to read subpart header!\n");
		return -1;
	}

	ifwi.subpart_hdr_ops->print(hdr);
	ifwi.subpart_entry_ops->print(&buff, ifwi.subpart_hdr_ops->get_entry_count(hdr));
	ifwi.subpart_hdr_ops->free(hdr);

	return 0;
}

static int cmd_print(void)
{
	ifwi.bpdt_ops->print_hdr(ifwi.bpdt_hdr);
	print_bpdt_entries();

	if (!params.print_sub_parts && !params.partition_name &&
	    params.partition_type == NO_PARTITION_TYPE)
		return 0;

	return process_entries(print_subpart);
}

static char *get_file_path(const char *name)
{
	size_t filename_len = strlen(name) + 1;

	/* output_dir name followed by '/' */
	if (params.output_dir)
		filename_len += strlen(params.output_dir) + 1;

	char *filepath = malloc(filename_len);
	if (!filepath)
		return NULL;

	snprintf(filepath, filename_len, "%s%s%s",
		 params.output_dir ? : "",
		 params.output_dir ? "/" : "",
		 name);

	return filepath;
}

static int write_partition_to_file(const struct bpdt_entry *e)
{
	size_t end_offset = e->offset + e->size - 1;

	if (end_offset > buffer_size(&ifwi.input_buff)) {
		ERROR("Offset out of bounds!\n");
		return -1;
	}

	const char *name = subpart_name(e->type);
	char *filepath = get_file_path(name);
	if (!filepath) {
		ERROR("Failed to allocate filepath!\n");
		return -1;
	}

	printf("Dumping %.4s in %s\n", name, filepath);

	struct buffer buff;
	buffer_splice(&buff, &ifwi.input_buff, e->offset, e->size);
	buffer_write_file(&buff, filepath);

	free(filepath);
	return 0;
}

static int cmd_dump(void)
{
	struct stat sb;

	if (params.output_dir && (stat(params.output_dir, &sb) == -1)) {
		ERROR("Failed to stat %s: %s\n", params.output_dir, strerror(errno));
		return -1;
	}

	return process_entries(write_partition_to_file);
}

static int cmd_print_layout(void)
{
	if (params.version_str == NULL) {
		ERROR("No version provided!\n");
		return -1;
	}

	const struct bpdt_ops *ops = get_bpdt_ops(NULL);
	if (!ops) {
		ERROR("No matching bpdt_ops!\n");
		return -1;
	}

	struct buffer buff;
	if (buffer_from_file(&buff, params.image_name)) {
		ERROR("Failed to read input file %s\n", params.image_name);
		return -1;
	}

	ifwi.cse_layout = ops->read_layout(&buff);
	if (!ifwi.cse_layout) {
		ERROR("Failed to read CSE layout!\n");
		return -1;
	}

	ops->print_layout(ifwi.cse_layout);

	return 0;
}

static int allocate_buffer(struct buffer *buff, struct buffer *wbuff, const char *str)
{
	if (params.version_str == NULL) {
		ERROR("No version provided!\n");
		return -1;
	}

	ifwi.bpdt_ops = get_bpdt_ops(NULL);
	if (!ifwi.bpdt_ops)
		return -1;

	if (buffer_create(buff, BUFF_SIZE_ALIGN, str)) {
		ERROR("Buffer creation error!\n");
		return -1;
	}

	void *data = buffer_get(buff);
	memset(data, 0xff, buffer_size(buff));

	buffer_clone(wbuff, buff);

	return 0;
}

static size_t get_cse_region_end_offset(void)
{
	size_t offset = 0;
	size_t end_offset;

	for (size_t i = 0; i < BP_TOTAL; i++) {
		end_offset = region_end(&params.layout_regions[i]);
		if (end_offset > offset)
			offset = end_offset;
	}

	return offset;
}

static int fill_layout_buffer(struct buffer *buff)
{
	struct buffer wbuff;

	if (allocate_buffer(buff, &wbuff, "CSE layout"))
		return -1;

	ifwi.cse_layout = ifwi.bpdt_ops->create_layout(&params.layout_regions[0]);
	if (!ifwi.cse_layout) {
		ERROR("Failed to create layout!\n");
		return -1;
	}

	if (ifwi.bpdt_ops->write_layout(&wbuff, ifwi.cse_layout)) {
		ERROR("Failed to write CSE layout!\n");
		return -1;
	}

	return 0;
}

static int cmd_create_layout(void)
{
	struct buffer buff;

	if (fill_layout_buffer(&buff))
		return -1;

	buffer_write_file(&buff, params.image_name);
	return 0;
}

static int cmd_create_cse_region(void)
{
	size_t file_size = get_cse_region_end_offset();
	struct buffer cse_buff, layout_buff;

	if (fill_layout_buffer(&layout_buff))
		return -1;

	if (file_size == 0)
		file_size = buffer_size(&layout_buff);

	file_size = ALIGN_UP(file_size, BUFF_SIZE_ALIGN);
	if (buffer_create(&cse_buff, file_size, "CSE buff")) {
		ERROR("CSE buffer creation error!\n");
		return -1;
	}

	memset(buffer_get(&cse_buff), 0xff, buffer_size(&cse_buff));
	memcpy(buffer_get(&cse_buff), buffer_get(&layout_buff), buffer_size(&layout_buff));

	for (size_t i = 0; i < BP_TOTAL; i++) {
		struct buffer wbuff, rbuff;

		if (region_sz(&params.layout_regions[i]) == 0)
			continue;

		buffer_clone(&wbuff, &cse_buff);
		buffer_seek(&wbuff, region_offset(&params.layout_regions[i]));

		if (params.layout_files[i] == NULL) {
			if (i == 0) {
				ERROR("File name not provided for DP!\n");
			} else {
				ERROR("File name not provided for BP%zd!\n", i);
			}
			return -1;
		}

		if (buffer_from_file(&rbuff, params.layout_files[i])) {
			ERROR("Failed to read %s\n", params.layout_files[i]);
			return -1;
		}

		assert(buffer_size(&wbuff) >= buffer_size(&rbuff));
		memcpy(buffer_get(&wbuff), buffer_get(&rbuff), buffer_size(&rbuff));
	}

	buffer_write_file(&cse_buff, params.image_name);

	return 0;
}

static int cmd_create_bpdt(void)
{
	struct buffer buff;
	struct buffer wbuff;

	if (allocate_buffer(&buff, &wbuff, "BPDT header"))
		return -1;

	ifwi.bpdt_hdr = ifwi.bpdt_ops->create_hdr();
	if (!ifwi.bpdt_hdr) {
		ERROR("Failed to create BPDT header!\n");
		return -1;
	}

	ifwi.bpdt_ops->update_checksum(ifwi.bpdt_hdr, NULL);

	if (ifwi.bpdt_ops->write_hdr(&wbuff, ifwi.bpdt_hdr)) {
		ERROR("Failed to write BPDT header!\n");
		return -1;
	}

	buffer_write_file(&buff, params.image_name);
	return 0;
}

static int cmd_add(void)
{
	if (!params.partition_name && params.partition_type == NO_PARTITION_TYPE) {
		ERROR("Partition name/type is required for add!\n");
		return -1;
	}

	int type;

	if (params.partition_name) {
		type = subpart_get_type_from_name(params.partition_name);
		if (type == NO_PARTITION_TYPE) {
			ERROR("Invalid partition %s\n", params.partition_name);
			return -1;
		}
	} else {
		type = params.partition_type;
		if (type > MAX_SUBPARTS) {
			ERROR("Invalid type %d\n", type);
			return -1;
		}
	}

	struct bpdt_entry *e = find_bpdt_entry(type);
	if (e) {
		ERROR("Partition %s(%d) already exists!\n", params.partition_name ? : "", type);
		return -1;
	}

	e = new_bpdt_entry();
	if (e == NULL)
		return -1;

	e->type = type;
	e->offset = 0;
	e->size = 0;

	ifwi.repack = true;

	if (params.input_file == NULL)
		return 0;

	struct buffer *buff = subpart_buff(type);
	if (buffer_from_file_aligned_size(buff, params.input_file, BUFF_SIZE_ALIGN)) {
		ERROR("Failed to read input file %s\n", params.input_file);
		return -1;
	}

	e->offset = ALIGN_UP(ifwi.file_end_offset, BUFF_SIZE_ALIGN);
	e->size = buffer_size(buff);

	ifwi.file_end_offset = e->offset + e->size;

	return 0;
}

static int parse_region(struct region *r, char *arg)
{
	char *tok;

	tok = strtok(arg, ":");
	unsigned long offset = strtoul(tok, NULL, 0);

	tok = strtok(NULL, ":");
	unsigned long size = strtoul(tok, NULL, 0);

	if (region_create_untrusted(r, offset, size) != CB_SUCCESS) {
		ERROR("Invalid region: %lx:%lx\n", offset, size);
		return -1;
	}

	return 0;
}

static struct command {
	const char *name;
	const char *optstring;
	int (*cb)(void);
	bool parse_ifwi;
} commands[] = {
	{ "print", "n:st:?", cmd_print, true },
	{ "dump", "n:o:t:?", cmd_dump, true },
	{ "create-layout", "v:?", cmd_create_layout, false },
	{ "print-layout", "v:?", cmd_print_layout, false },
	{ "create-bpdt", "v:?", cmd_create_bpdt, false },
	{ "add", "f:n:t:v:?", cmd_add, true },
	{ "create-cse-region", "v:?", cmd_create_cse_region, false },
};

enum {
	LONGOPT_START = 256,
	LONGOPT_BP1 = LONGOPT_START,
	LONGOPT_BP2,
	LONGOPT_BP3,
	LONGOPT_BP4,
	LONGOPT_DATA,
	LONGOPT_BP1_FILE,
	LONGOPT_BP2_FILE,
	LONGOPT_BP3_FILE,
	LONGOPT_BP4_FILE,
	LONGOPT_DATA_FILE,

	LONGOPT_END,
};

static struct option long_options[] = {
	{"help",	    required_argument, 0, 'h'},
	{"parition_name",   required_argument, 0, 'n'},
	{"output_dir",      required_argument, 0, 'o'},
	{"sub_partition",   no_argument,       0, 's'},
	{"version",         required_argument, 0, 'v'},
	{"bp1",             required_argument, 0, LONGOPT_BP1},
	{"bp1_file",        required_argument, 0, LONGOPT_BP1_FILE},
	{"bp2",             required_argument, 0, LONGOPT_BP2},
	{"bp2_file",        required_argument, 0, LONGOPT_BP2_FILE},
	{"bp3",             required_argument, 0, LONGOPT_BP3},
	{"bp3_file",        required_argument, 0, LONGOPT_BP3_FILE},
	{"bp4",             required_argument, 0, LONGOPT_BP4},
	{"bp4_file",        required_argument, 0, LONGOPT_BP4_FILE},
	{"dp",              required_argument, 0, LONGOPT_DATA},
	{"dp_file",         required_argument, 0, LONGOPT_DATA_FILE},
	{NULL,		    0,                 0,  0 }
};

static bool valid_opt(size_t i, int c)
{
	/* Check if it is one of the optstrings supported by the command. */
	if (strchr(commands[i].optstring, c))
		return true;

	/*
	 * Check if it is one of the non-ASCII characters. Currently, the
	 * non-ASCII characters are only checked against the valid list
	 * irrespective of the command.
	 */
	return c >= LONGOPT_START && c < LONGOPT_END;
}

static void usage(const char *name)
{
	printf("%s: Utility for stitching CSE components\n"
	       "USAGE:\n"
	       " %s FILE COMMAND\n\n"
	       "COMMANDs:\n"
	       " print [-s][-n NAME][-t TYPE]\n"
	       " dump [-o DIR][-n NAME]\n"
	       " create-layout --dp <offset:size> --bp* <offset:size> -v VERSION\n"
	       " create-cse-region --dp <offset:size> --dp_file <FILE> --bp* <offset:size>"
	       " --bp*_file <FILE> -v VERSION\n"
	       " print-layout -v VERSION\n"
	       " create-bpdt -v VERSION\n"
	       " add [-n NAME][-t TYPE][-f INPUT_FILE]\n"
	       "\nOPTIONS:\n"
	       " -f INPUT_FILE       : Input file\n"
	       " -n NAME             : Sub-partition name\n"
	       " -o DIR              : Output directory\n"
	       " -s                  : Print sub-partition info\n"
	       " -t TYPE             : Sub-partition type\n"
	       " -v VERSION          : BPDT version\n"
	       " --dp <offset:size>  : Offset and size of data partition\n"
	       " --dp_file <FILE>    : File for data partition\n"
	       " --bp1 <offset:size> : Offset and size of BP1\n"
	       " --bp1_file <FILE>   : File for BP1 partition\n"
	       " --bp2 <offset:size> : Offset and size of BP2\n"
	       " --bp2_file <FILE>   : File for BP2 partition\n"
	       " --bp3 <offset:size> : Offset and size of BP3\n"
	       " --bp3_file <FILE>   : File for BP3 partition\n"
	       " --bp4 <offset:size> : Offset and size of BP4\n"
	       " --bp4_file <FILE>   : File for BP4 partition\n"
	       "\n",
	       name, name);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Incorrect number of args(%d)!\n", argc);
		usage(argv[0]);
		return 1;
	}

	const char *prog_name = argv[0];
	const char *image_name = argv[1];
	const char *cmd = argv[2];

	size_t i;

	params.partition_type = NO_PARTITION_TYPE;
	params.image_name = image_name;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(cmd, commands[i].name))
			continue;

		int c;
		int option_index;

		while (1) {
			c = getopt_long(argc, argv, commands[i].optstring,
					long_options, &option_index);

			if (c == -1)
				break;

			if (!valid_opt(i, c)) {
				if (c < LONGOPT_START) {
					ERROR("Invalid option -- '%c'\n", c);
				} else {
					ERROR("Invalid option -- '%d'\n", c);
				}
				usage(prog_name);
				return 1;
			}

			switch (c) {
			case 'f':
				params.input_file = optarg;
				break;
			case 'n':
				params.partition_name = optarg;
				break;
			case 'o':
				params.output_dir = optarg;
				break;
			case 's':
				params.print_sub_parts = true;
				break;
			case 'v':
				params.version_str = optarg;
				break;
			case 't':
				params.partition_type = atoi(optarg);
				break;
			case LONGOPT_BP1:
				if (parse_region(&params.layout_regions[BP1], optarg))
					return 1;
				break;
			case LONGOPT_BP2:
				if (parse_region(&params.layout_regions[BP2], optarg))
					return 1;
				break;
			case LONGOPT_BP3:
				if (parse_region(&params.layout_regions[BP3], optarg))
					return 1;
				break;
			case LONGOPT_BP4:
				if (parse_region(&params.layout_regions[BP4], optarg))
					return 1;
				break;
			case LONGOPT_DATA:
				if (parse_region(&params.layout_regions[DP], optarg))
					return 1;
				break;
			case LONGOPT_BP1_FILE:
				params.layout_files[BP1] = optarg;
				break;
			case LONGOPT_BP2_FILE:
				params.layout_files[BP2] = optarg;
				break;
			case LONGOPT_BP3_FILE:
				params.layout_files[BP3] = optarg;
				break;
			case LONGOPT_BP4_FILE:
				params.layout_files[BP4] = optarg;
				break;
			case LONGOPT_DATA_FILE:
				params.layout_files[DP] = optarg;
				break;
			case 'h':
			case '?':
			default:
				usage(prog_name);
				return 1;
			}
		}

		break;
	}

	if (i == ARRAY_SIZE(commands)) {
		printf("No command match %s!\n", cmd);
		usage(prog_name);
		return 1;
	}

	if (commands[i].parse_ifwi && ifwi_parse(image_name))
		return 1;

	if (commands[i].cb())
		return 1;

	return ifwi_repack();
}
