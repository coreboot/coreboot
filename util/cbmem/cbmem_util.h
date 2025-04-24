/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/bsd/helpers.h>

extern int cbmem_util_verbose;

#define debug(x...)                                                            \
	do {                                                                   \
		if (cbmem_util_verbose > 1)                                    \
			printf("[%s:%d %s()] ", __FILE__, __LINE__, __func__); \
		if (cbmem_util_verbose)                                        \
			printf(x);                                             \
	} while (0)

#define die(x...)                   \
	do {                        \
		fprintf(stderr, x); \
		abort();            \
	} while (0)

struct cbmem_console {
	uint32_t size;
	uint32_t cursor;
	uint8_t body[];
} __packed;

#define CBMC_CURSOR_MASK ((1 << 28) - 1)
#define CBMC_OVERFLOW    (1 << 31)

/**
 * Function pointer type used by CBMEM foreach iteration calls.
 *
 * @param id CBMEM_ID_* value.
 * @param physical_address CBMEM entry address in physical memory.
 * @param buf heap-allocated buffer with CBMEM entry contents.
 * @param size is the size of CBMEM entry in bytes.
 * @param data callback-specific context data.
 *
 * @returns true if iteration should finish, false if it should continue.
 */
typedef bool (*cbmem_iterator_callback)(const uint32_t id, const uint64_t physical_address,
					const uint8_t *buf, const size_t size, void *data);

/* API for accessing CBMEM via /dev/mem */

/**
 * Initialize the driver.
 *
 * @param writeable tries to map CBMEM in R/W mode.
 *
 * @returns true on success, false otherwise.
 */
bool cbmem_devmem_init(bool writeable);

/**
 * Cleanup and terminate the driver. **MUST** be called if cbmem_devmem_init succeeded before.
 */
void cbmem_devmem_terminate(void);

/**
 * Get CBMEM entry as an allocated buffer.
 *
 * @param id CBMEM_ID_* value.
 * @param buf_out return pointer for the allocated buffer containing entry contents.
 * @param size_out size of returned buffer. Optional.
 * @param addr_out pointer to the output buffer for entry address in physical memory. Optional.
 *
 * @returns true on success, false otherwise.
 */
bool cbmem_devmem_get_cbmem_entry(uint32_t id, uint8_t **buf_out, size_t *size_out, uint64_t *addr_out);

/**
 * Write provided buffer contents to the CBMEM entry.
 *
 * @param id CBMEM_ID_* value.
 * @param buf pointer to the source buffer.
 * @param buf_size size of the source buffer.
 *
 * @returns true on success, false otherwise.
 */
bool cbmem_devmem_write_cbmem_entry(uint32_t id, uint8_t *buf, size_t buf_size);

/**
 * Backend-specific function iterating over CBMEM entries.
 *
 * @param cb user callback function to call during iteration.
 * @param data pointer to the context data for the callback.
 * @param with_contents tells whether the callback should get NULL (false) or copy of the entry (true).
 */
void cbmem_devmem_foreach_cbmem_entry(cbmem_iterator_callback cb, void *data,
				      bool with_contents);
