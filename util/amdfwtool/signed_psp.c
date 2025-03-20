/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "amdfwtool.h"

/* Defines related to hashing signed binaries */
enum hash_header_ver {
	HASH_HDR_V1 = 1,
	HASH_HDR_V2,
};
/* Signature ID enums are defined by PSP based on the algorithm used. */
enum signature_id {
	SIG_ID_RSA2048,
	SIG_ID_RSA4096 = 2,
};

#define HASH_FILE_SUFFIX ".hash"
struct psp_fw_hash_file_info {
	int fd;
	bool present;
	struct psp_fw_hash_table hash_header;
};
static struct psp_fw_hash_file_info hash_files[MAX_NUM_HASH_TABLES];

#define UUID_MAGIC "gpd.ta.appID"

static uint16_t get_psp_fw_type(enum platform soc_id, struct amd_fw_header *header)
{
	switch (soc_id) {
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_FAEGAN:
		/* Fallback to fw_type if fw_id is not populated, which serves the same
		   purpose on older SoCs. */
		return header->fw_id ? header->fw_id : header->fw_type;
	default:
		return header->fw_type;
	}
}

static void get_psp_fw_uuid(void *buf, size_t buf_len, uint8_t *uuid)
{
	void *ptr = memmem(buf, buf_len, UUID_MAGIC, strlen(UUID_MAGIC));

	assert(ptr != NULL);

	memcpy(uuid, ptr + strlen(UUID_MAGIC), UUID_LEN_BYTES);
}

static int add_single_sha(amd_fw_entry_hash *entry, void *buf, enum platform soc_id,
								fwid_type_t fwid_type)
{
	uint8_t hash[SHA384_DIGEST_LENGTH];
	struct amd_fw_header *header = (struct amd_fw_header *)buf;
	/* Include only signed part for hash calculation. */
	size_t len = header->fw_size_signed + sizeof(struct amd_fw_header);
	uint8_t *body = (uint8_t *)buf;

	if (len > header->size_total)
		return -1;

	if (header->sig_id == SIG_ID_RSA4096) {
		SHA384(body, len, hash);
		entry->sha_len = SHA384_DIGEST_LENGTH;
	} else if (header->sig_id == SIG_ID_RSA2048) {
		SHA256(body, len, hash);
		entry->sha_len = SHA256_DIGEST_LENGTH;
	} else {
		fprintf(stderr, "%s: Unknown signature id: 0x%08x\n",
						__func__, header->sig_id);
		return -1;
	}

	memcpy(entry->sha, hash, entry->sha_len);
	entry->fwid_type = fwid_type;
	if (fwid_type == FWID_TYPE_UUID)
		get_psp_fw_uuid(buf, header->size_total, entry->uuid);
	else
		entry->fw_id = get_psp_fw_type(soc_id, header);
	entry->subtype = header->fw_subtype;

	return 0;
}

static int get_num_binaries(void *buf, size_t buf_size)
{
	struct amd_fw_header *header = (struct amd_fw_header *)buf;
	size_t total_len = 0;
	int num_binaries = 0;

	while (total_len < buf_size) {
		num_binaries++;
		total_len += header->size_total;
		header = (struct amd_fw_header *)(buf + total_len);
	}

	if (total_len != buf_size) {
		fprintf(stderr, "Malformed binary\n");
		return -1;
	}
	return num_binaries;
}

static int add_sha(amd_fw_entry *entry, void *buf, size_t buf_size, enum platform soc_id)
{
	struct amd_fw_header *header = (struct amd_fw_header *)buf;
	/* Include only signed part for hash calculation. */
	size_t total_len = 0;
	int num_binaries = get_num_binaries(buf, buf_size);

	if (num_binaries <= 0)
		return num_binaries;

	entry->hash_entries = calloc(num_binaries, sizeof(amd_fw_entry_hash));
	if (!entry->hash_entries) {
		fprintf(stderr, "Error allocating memory to add FW hash\n");
		return -1;
	}
	entry->num_hash_entries = num_binaries;

	/* Iterate through each binary */
	for (int i = 0; i < num_binaries; i++) {
		if (add_single_sha(&entry->hash_entries[i], buf + total_len, soc_id,
								entry->fwid_type)) {
			free(entry->hash_entries);
			return -1;
		}
		total_len += header->size_total;
		header = (struct amd_fw_header *)(buf + total_len);
	}

	return 0;
}

static void write_one_psp_firmware_hash_entry(int fd, amd_fw_entry_hash *entry,
								uint8_t hash_tbl_id)
{
	uint16_t subtype = entry->subtype;

	if (hash_files[hash_tbl_id].hash_header.version == HASH_HDR_V2) {
		write_or_fail(fd, entry->uuid, UUID_LEN_BYTES);
	} else {
		write_or_fail(fd, &entry->fw_id, sizeof(entry->fw_id));
		write_or_fail(fd, &subtype, sizeof(subtype));
	}
	write_or_fail(fd, entry->sha, entry->sha_len);
}

static void open_psp_fw_hash_files(const char *file_prefix)
{
	size_t hash_file_strlen;
	char *hash_file_name;

	/* Hash Table ID is part of the file name. For now only single digit ID is
	   supported and is sufficient. Hence assert MAX_NUM_HASH_TABLES < 10 before
	   constructing file name. Revisit later when > 10 hash tables are required. */
	assert(MAX_NUM_HASH_TABLES < 10);
	/* file_prefix + ".[1-9]" + ".hash" + '\0' */
	hash_file_strlen = strlen(file_prefix) + 2 + strlen(HASH_FILE_SUFFIX) + 1;
	hash_file_name = malloc(hash_file_strlen);
	if (!hash_file_name)  {
		fprintf(stderr, "malloc(%lu) failed\n", hash_file_strlen);
		exit(-1);
	}

	for (unsigned int i = 0; i < MAX_NUM_HASH_TABLES; i++) {
		/* Hash table IDs are expected to be contiguous and hence holes are not
		   expected. */
		if (!hash_files[i].present)
			break;

		if (i)
			snprintf(hash_file_name, hash_file_strlen, "%s.%d%s",
				 file_prefix, i, HASH_FILE_SUFFIX);
		else
			/* Default file name without number for backwards compatibility. */
			snprintf(hash_file_name, hash_file_strlen, "%s%s",
				 file_prefix, HASH_FILE_SUFFIX);

		hash_files[i].fd = open(hash_file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
		if (hash_files[i].fd < 0) {
			fprintf(stderr, "Error opening file: %s: %s\n",
					hash_file_name, strerror(errno));
			free(hash_file_name);
			exit(-1);
		}
	}
	free(hash_file_name);
}

static void close_psp_fw_hash_files(void)
{
	for (unsigned int i = 0; i < MAX_NUM_HASH_TABLES; i++) {
		if (!hash_files[i].present)
			break;

		close(hash_files[i].fd);
	}
}

static void write_psp_firmware_hash(amd_fw_entry *fw_table)
{
	uint8_t hash_tbl_id;

	for (unsigned int i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		hash_tbl_id = fw_table[i].hash_tbl_id;
		assert(hash_files[hash_tbl_id].present);

		for (unsigned int j = 0; j < fw_table[i].num_hash_entries; j++) {
			if (fw_table[i].hash_entries[j].sha_len == SHA256_DIGEST_LENGTH) {
				hash_files[hash_tbl_id].hash_header.no_of_entries_256++;
			} else if (fw_table[i].hash_entries[j].sha_len ==
								SHA384_DIGEST_LENGTH) {
				hash_files[hash_tbl_id].hash_header.no_of_entries_384++;
			} else if (fw_table[i].hash_entries[j].sha_len) {
				fprintf(stderr, "%s: Error invalid sha_len %d\n",
						__func__, fw_table[i].hash_entries[j].sha_len);
				exit(-1);
			}
		}
	}

	for (unsigned int i = 0; i < MAX_NUM_HASH_TABLES; i++) {
		if (!hash_files[i].present)
			continue;
		write_or_fail(hash_files[i].fd, &hash_files[i].hash_header,
						sizeof(hash_files[i].hash_header));
		/* Add a reserved field as expected by version 2 header */
		if (hash_files[i].hash_header.version == HASH_HDR_V2) {
			uint16_t reserved = 0;

			write_or_fail(hash_files[i].fd, &reserved, sizeof(reserved));
		}
	}

	/* Add all the SHA256 hash entries first followed by SHA384 entries. PSP verstage
	   processes the table in that order. Mixing and matching SHA256 and SHA384 entries
	   will cause the hash verification failure at run-time. */
	for (unsigned int i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		hash_tbl_id = fw_table[i].hash_tbl_id;
		for (unsigned int j = 0; j < fw_table[i].num_hash_entries; j++) {
			if (fw_table[i].hash_entries[j].sha_len == SHA256_DIGEST_LENGTH)
				write_one_psp_firmware_hash_entry(hash_files[hash_tbl_id].fd,
						&fw_table[i].hash_entries[j], hash_tbl_id);
		}
	}

	for (unsigned int i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		hash_tbl_id = fw_table[i].hash_tbl_id;
		for (unsigned int j = 0; j < fw_table[i].num_hash_entries; j++) {
			if (fw_table[i].hash_entries[j].sha_len == SHA384_DIGEST_LENGTH)
				write_one_psp_firmware_hash_entry(hash_files[hash_tbl_id].fd,
						&fw_table[i].hash_entries[j], hash_tbl_id);
		}
	}

	for (unsigned int i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (!fw_table[i].num_hash_entries || !fw_table[i].hash_entries)
			continue;

		free(fw_table[i].hash_entries);
		fw_table[i].hash_entries = NULL;
		fw_table[i].num_hash_entries = 0;
	}
}

static void update_hash_files_config(amd_fw_entry *fw_table)
{
	uint16_t version = fw_table->fwid_type == FWID_TYPE_UUID ? HASH_HDR_V2 : HASH_HDR_V1;

	hash_files[fw_table->hash_tbl_id].present = true;
	if (version > hash_files[fw_table->hash_tbl_id].hash_header.version)
		hash_files[fw_table->hash_tbl_id].hash_header.version = version;
}

/**
 * process_signed_psp_firmwares() - Process the signed PSP binaries to keep them separate
 * @signed_rom:	Output file path grouping all the signed PSP binaries.
 * @fw_table:	Table of all the PSP firmware entries/binaries to be processed.
 * @signed_start_addr:	Offset of the FMAP section, within the flash device, to hold
 *                      the signed PSP binaries.
 * @soc_id:	SoC ID of the PSP binaries.
 */
void process_signed_psp_firmwares(const char *signed_rom,
		amd_fw_entry *fw_table,
		uint64_t signed_start_addr,
		enum platform soc_id)
{
	unsigned int i;
	int fd;
	int signed_rom_fd;
	ssize_t bytes, align_bytes;
	uint8_t *buf;
	struct amd_fw_header header;
	struct stat fd_stat;
	/* Every blob in amdfw*.rom has to start at address aligned to 0x100. Prepare an
	   alignment data with 0xff to pad the blobs and meet the alignment requirement. */
	uint8_t align_data[BLOB_ALIGNMENT - 1];

	memset(align_data, 0xff, sizeof(align_data));
	signed_rom_fd = open(signed_rom, O_RDWR | O_CREAT | O_TRUNC,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (signed_rom_fd < 0) {
		fprintf(stderr, "Error opening file: %s: %s\n",
				signed_rom, strerror(errno));
		return;
	}

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		fw_table[i].num_hash_entries = 0;
		fw_table[i].hash_entries = NULL;

		if (!(fw_table[i].filename) || fw_table[i].skip_hashing)
			continue;

		memset(&header, 0, sizeof(header));

		fd = open(fw_table[i].filename, O_RDONLY);
		if (fd < 0) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "Error opening file: %s: %s\n",
					fw_table[i].filename, strerror(errno));
			continue;
		}

		if (fstat(fd, &fd_stat)) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "fstat error: %s\n", strerror(errno));
			close(fd);
			continue;
		}

		bytes = read_from_file_to_buf(fd, &header, sizeof(struct amd_fw_header));
		if (bytes != (ssize_t)sizeof(struct amd_fw_header)) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "%s: Error reading header from %s\n",
						__func__, fw_table[i].filename);
			close(fd);
			continue;
		}

		/* If firmware header looks like invalid, assume it's not signed */
		if (!header.fw_type && !header.fw_id) {
			fprintf(stderr, "%s: Invalid FWID for %s\n",
					__func__, fw_table[i].filename);
			close(fd);
			continue;
		}


		/* PSP binary is not signed and should not be part of signed PSP binaries
		   set. */
		if (header.sig_opt != 1) {
			close(fd);
			continue;
		}

		buf = malloc(fd_stat.st_size);
		if (!buf) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "%s: failed to allocate memory with size %lld\n",
							__func__, (long long)fd_stat.st_size);
			close(fd);
			continue;
		}

		lseek(fd, SEEK_SET, 0);
		bytes = read_from_file_to_buf(fd, buf, fd_stat.st_size);
		if (bytes != fd_stat.st_size) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "%s: failed to read %s\n",
					__func__, fw_table[i].filename);
			free(buf);
			close(fd);
			continue;
		}

		bytes = write_from_buf_to_file(signed_rom_fd, buf, fd_stat.st_size);
		if (bytes != fd_stat.st_size) {
			/* Keep the file along with set of unsigned PSP binaries & continue. */
			fprintf(stderr, "%s: failed to write %s\n",
					__func__, fw_table[i].filename);
			free(buf);
			close(fd);
			continue;
		}

		/* Write Blob alignment bytes */
		align_bytes = 0;
		if (fd_stat.st_size & (BLOB_ALIGNMENT - 1)) {
			align_bytes = BLOB_ALIGNMENT -
				(fd_stat.st_size & (BLOB_ALIGNMENT - 1));
			bytes = write_from_buf_to_file(signed_rom_fd, align_data, align_bytes);
			if (bytes != align_bytes) {
				fprintf(stderr, "%s: failed to write alignment data for %s\n",
								__func__, fw_table[i].filename);
				lseek(signed_rom_fd, SEEK_CUR, -fd_stat.st_size);
				free(buf);
				close(fd);
				continue;
			}
		}

		if (add_sha(&fw_table[i], buf, fd_stat.st_size, soc_id))
			exit(-1);

		/* File is successfully processed and is part of signed PSP binaries set. */
		fw_table[i].addr_signed = signed_start_addr;
		fw_table[i].file_size = (uint32_t)fd_stat.st_size;
		update_hash_files_config(&fw_table[i]);

		signed_start_addr += fd_stat.st_size + align_bytes;

		free(buf);
		close(fd);
	}

	close(signed_rom_fd);

	open_psp_fw_hash_files(signed_rom);
	write_psp_firmware_hash(fw_table);
	close_psp_fw_hash_files();
}
