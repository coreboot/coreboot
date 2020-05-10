/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SMMSTORE_H_
#define _SMMSTORE_H_

#include <stddef.h>
#include <stdint.h>

#define SMMSTORE_RET_SUCCESS 0
#define SMMSTORE_RET_FAILURE 1
#define SMMSTORE_RET_UNSUPPORTED 2

#define SMMSTORE_CMD_CLEAR 1
#define SMMSTORE_CMD_READ 2
#define SMMSTORE_CMD_APPEND 3

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

/* SMM responder */
uint32_t smmstore_exec(uint8_t command, void *param);

/* implementation */
int smmstore_read_region(void *buf, ssize_t *bufsize);
int smmstore_append_data(void *key, uint32_t key_sz,
	void *value, uint32_t value_sz);
int smmstore_clear_region(void);
#endif
