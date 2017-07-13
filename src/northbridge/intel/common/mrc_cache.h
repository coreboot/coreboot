#ifndef NORTHBRIDGE_INTEL_COMMON_MRC_CACHE_H
#define NORTHBRIDGE_INTEL_COMMON_MRC_CACHE_H

#include <compiler.h>

#define MRC_DATA_ALIGN           0x1000
#define MRC_DATA_SIGNATURE       (('M'<<0)|('R'<<8)|('C'<<16)|('D'<<24))

struct mrc_data_container {
	u32	mrc_signature;	// "MRCD"
	u32	mrc_data_size;	// Actual total size of this structure
	u32	mrc_checksum;	// IP style checksum
	u32	reserved;	// For header alignment
	u8	mrc_data[0];	// Variable size, platform/run time dependent.
} __packed;

struct mrc_data_container *find_current_mrc_cache(void);
struct mrc_data_container *store_current_mrc_cache(void *data, unsigned length);

#endif /* NORTHBRIDGE_INTEL_COMMON_MRC_CACHE_H */
