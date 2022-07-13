/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SMMSTORE_H_
#define _SMMSTORE_H_

#include <stddef.h>
#include <stdint.h>

#define SMMSTORE_RET_SUCCESS 0
#define SMMSTORE_RET_FAILURE 1
#define SMMSTORE_RET_UNSUPPORTED 2

/* Version 1 */
#define SMMSTORE_CMD_CLEAR 1
#define SMMSTORE_CMD_READ 2
#define SMMSTORE_CMD_APPEND 3

/* Version 2 */
#define SMMSTORE_CMD_INIT 4
#define SMMSTORE_CMD_RAW_READ 5
#define SMMSTORE_CMD_RAW_WRITE 6
#define SMMSTORE_CMD_RAW_CLEAR 7

/* Version 1 */
struct smmstore_params_read {
	void *buf;
	ssize_t bufsize;
};

struct smmstore_params_append {
	void *key;
	size_t keysize;
	void *val;
	size_t valsize;
};

/* Version 2 */
/*
 * The Version 2 protocol separates the SMMSTORE into 64KiB blocks, each
 * of which can be read/written/cleared in an independent manner. The
 * data format isn't specified. See documentation page for more details.
 */

#define SMM_BLOCK_SIZE (64 * KiB)

/*
 * Sets the communication buffer to use for read and write operations.
 */
struct smmstore_params_init {
	uint32_t com_buffer;
	uint32_t com_buffer_size;
} __packed;

/*
 * Returns the number of blocks the SMMSTORE supports and their size.
 * For edk2 this should be at least two blocks with 64 KiB each.
 * The mmap_addr is set the memory mapped physical address of the SMMSTORE.
 */
struct smmstore_params_info {
	uint32_t num_blocks;
	uint32_t block_size;
	uint32_t mmap_addr;
} __packed;

/*
 * Reads a chunk of raw data with size @bufsize from the block specified by
 * @block_id starting at @bufoffset.
 * The read data is placed in memory pointed to by @buf.
 *
 * @block_id must be less than num_blocks
 * @bufoffset + @bufsize must be less than block_size
 */
struct smmstore_params_raw_write {
	uint32_t bufsize;
	uint32_t bufoffset;
	uint32_t block_id;
} __packed;

/*
 * Writes a chunk of raw data with size @bufsize to the block specified by
 * @block_id starting at @bufoffset.
 *
 * @block_id must be less than num_blocks
 * @bufoffset + @bufsize must be less than block_size
 */
struct smmstore_params_raw_read {
	uint32_t bufsize;
	uint32_t bufoffset;
	uint32_t block_id;
} __packed;

/*
 * Erases the specified block.
 *
 * @block_id must be less than num_blocks
 */
struct smmstore_params_raw_clear {
	uint32_t block_id;
} __packed;


/* SMM handler */
uint32_t smmstore_exec(uint8_t command, void *param);

/* Implementation of Version 1 */
int smmstore_read_region(void *buf, ssize_t *bufsize);
int smmstore_append_data(void *key, uint32_t key_sz, void *value, uint32_t value_sz);
int smmstore_clear_region(void);

/* Implementation of Version 2 */
int smmstore_init(void *buf, size_t len);
int smmstore_rawread_region(uint32_t block_id, uint32_t offset, uint32_t bufsize);
int smmstore_rawwrite_region(uint32_t block_id, uint32_t offset, uint32_t bufsize);
int smmstore_rawclear_region(uint32_t block_id);
#if ENV_RAMSTAGE
int smmstore_get_info(struct smmstore_params_info *info);
#endif

/* Advertise SMMSTORE v2 support */
struct lb_header;
void lb_smmstorev2(struct lb_header *header);

#endif
