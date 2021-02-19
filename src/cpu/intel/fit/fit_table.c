/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

struct fit_entry {
	uint8_t address[sizeof(uint64_t)];
	uint32_t size_reserved;
	uint16_t version;
	uint8_t  type_checksum_valid;
	uint8_t  checksum;
} __packed;

__attribute((used)) static struct fit_entry fit_table[CONFIG_CPU_INTEL_NUM_FIT_ENTRIES + 1] = {
	[0] = {
		.address = {'_', 'F', 'I', 'T', '_', ' ', ' ', ' '},
		.size_reserved = 1,
		.version = 0x100,
		.type_checksum_valid = 0x80,
		.checksum = 0x7d,
	}
};
