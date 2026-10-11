/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_AMPERE_ALTRA_ATF_H
#define SOC_AMPERE_ALTRA_ATF_H

#include <stdint.h>
#include <uuid.h>

#define MM_VERSION 0x84000040
//#define MM_COMMUNICATE 0x84000041 // AARCH32
#define MM_COMMUNICATE 0xC4000041 // AARCH64
//#define MM_COMMUNICATE 0xc4000041

/* Return codes of the MM_* SMC calls (not part of SMCCC) */
enum mm_return_values {
	MM_SUCCESS = 0,
	MM_NOT_SUPPORTED = -1,
	MM_INVALID_PARAMS = -2,
	MM_DENIED = -3,
	MM_NO_MEMORY = -4,
};

union mm_version {
	struct {
		uint32_t minor : 16;
		uint32_t major : 15;
		uint32_t must_be_zero : 1;
	};
	uint32_t raw;
};

struct __packed mm_communicate {
	guid_t guid;
	size_t msg_length;
	union __packed {
		struct __packed {
			uint64_t dw0;
			uint64_t dw1;
			uint64_t dw2;
			uint64_t dw3;
			uint64_t dw4;
		};
		struct __packed {
			uint64_t status;
			uint64_t device_base;
			uint64_t page_size;
			uint64_t sector_size;
			uint64_t device_size;
		};
	};
};

uint32_t atf_mm_version(void);
uint32_t atf_mm_communicate(void);

#endif
