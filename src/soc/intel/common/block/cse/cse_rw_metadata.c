/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/cse.h>

#define HASH_TO_ARRAY(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,\
		      x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30,\
		      x31, x32) { 0x##x1, 0x##x2, 0x##x3, 0x##x4, 0x##x5, 0x##x6, 0x##x7,\
		      0x##x8, 0x##x9, 0x##x10, 0x##x11, 0x##x12, 0x##x13, 0x##x14, 0x##x15,\
		      0x##x16, 0x##x17, 0x##x18, 0x##x19, 0x##x20, 0x##x21, 0x##x22, 0x##x23,\
		      0x##x24, 0x##x25, 0x##x26, 0x##x27, 0x##x28, 0x##x29, 0x##x30, 0x##x31,\
		      0x##x32 }
#define HASH_BYTEARRAY(...) HASH_TO_ARRAY(__VA_ARGS__)

/*
 * This structure contains the CSE RW version and hash details which are filled during the
 * compile time.
 * Makefile will extract the following details and updates the structure variable via the
 * compile time flags.
 * CSE RW version: Extract the version string from the SOC_INTEL_CSE_RW_VERSION config and
 * assign the major, minor, hotfix and build versions.
 * CSE RW hash: Compute the hash of CSE RW binary in hex format using the openssl and use the
 * HASH_BYTEARRAY macro to convert the 64 character hex values into the array.
 */
struct cse_rw_metadata metadata = {
	.version = {
		.major = CSE_RW_MAJOR,
		.minor = CSE_RW_MINOR,
		.build = CSE_RW_BUILD,
		.hotfix = CSE_RW_HOTFIX,
	},
	.sha256 = HASH_BYTEARRAY(CSE_RW_SHA256),
};
