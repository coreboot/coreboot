/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef TESTS_LIB_CBFS_UTIL_H
#define TESTS_LIB_CBFS_UTIL_H

#include <cbfs.h>
#include <symbols.h>
#include <tests/test.h>

#define BE32(be32) EMPTY_WRAP(\
	((be32) >> 24) & 0xff, ((be32) >> 16) & 0xff, \
	((be32) >> 8) & 0xff, ((be32) >> 0) & 0xff)

#define BE64(be64) EMPTY_WRAP( \
	BE32(((be64) >> 32) & 0xFFFFFFFF), \
	BE32(((be64) >> 0) & 0xFFFFFFFF))

#define LE32(val32) EMPTY_WRAP(\
	((val32) >> 0) & 0xff, ((val32) >> 8) & 0xff, \
	((val32) >> 16) & 0xff, ((val32) >> 24) & 0xff)

#define LE64(val64) EMPTY_WRAP( \
	BE32(((val64) >> 0) & 0xFFFFFFFF), \
	BE32(((val64) >> 32) & 0xFFFFFFFF))

#define FILENAME_SIZE 16

struct cbfs_test_file {
	struct cbfs_file header;
	u8 filename[FILENAME_SIZE];
	u8 attrs_and_data[200];
};

#define TEST_CBFS_CACHE_SIZE (2 * MiB)
#define TEST_MCACHE_SIZE (2 * MiB)

#define HEADER_INITIALIZER(ftype, attr_len, file_len) { \
	.magic = CBFS_FILE_MAGIC, \
	.len = cpu_to_be32(file_len), \
	.type = cpu_to_be32(ftype), \
	.attributes_offset = \
		cpu_to_be32(attr_len ? sizeof(struct cbfs_file) + FILENAME_SIZE : 0), \
	.offset = cpu_to_be32(sizeof(struct cbfs_file) + FILENAME_SIZE + attr_len), \
}

#define HASH_ATTR_SIZE (offsetof(struct cbfs_file_attr_hash, hash.raw) + VB2_SHA256_DIGEST_SIZE)

/* This macro basically does nothing but suppresses linter messages */
#define EMPTY_WRAP(...) __VA_ARGS__

#define TEST_DATA_1_FILENAME "test/data/1"
#define TEST_DATA_1_SIZE sizeof((u8[]){TEST_DATA_1})
#define TEST_DATA_1 EMPTY_WRAP( \
	'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', \
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', \
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', \
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', \
	'[', '\\', ']', '^', '_', '`', \
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', \
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z')

#define TEST_DATA_2_FILENAME "test/data/2"
#define TEST_DATA_2_SIZE sizeof((u8[]){TEST_DATA_2})
#define TEST_DATA_2 EMPTY_WRAP( \
	0x9d, 0xa9, 0x91, 0xac, 0x5d, 0xb2, 0x70, 0x76, 0x37, 0x94, 0x94, 0xa8, 0x8b, 0x78, \
	0xb9, 0xaa, 0x1a, 0x8e, 0x9a, 0x16, 0xbe, 0xdc, 0x29, 0x42, 0x46, 0x58, 0xd4, 0x37, \
	0x94, 0xca, 0x05, 0xdb, 0x54, 0xfa, 0xd8, 0x6e, 0x54, 0xd8, 0x30, 0x46, 0x5d, 0x62, \
	0xc2, 0xce, 0xd8, 0x74, 0x60, 0xaf, 0x83, 0x8f, 0xfa, 0x97, 0xdd, 0x6e, 0xcb, 0x60, \
	0xfa, 0xed, 0x8b, 0x55, 0x9e, 0xc1, 0xc2, 0x18, 0x4f, 0xe2, 0x28, 0x7e, 0xd7, 0x2f, \
	0xa2, 0x86, 0xfb, 0x4d, 0x3e, 0x00, 0x5a, 0xf7, 0xc2, 0xad, 0x0e, 0xa7, 0xa2, 0xf7, \
	0x38, 0x66, 0xe6, 0x5c, 0x76, 0x98, 0x89, 0x63, 0xeb, 0xc5, 0xf5, 0xb7, 0xa7, 0x58, \
	0xe0, 0xf0, 0x2e, 0x2f, 0xb0, 0x95, 0xb7, 0x43, 0x28, 0x19, 0x2d, 0xef, 0x1a, 0xb3, \
	0x42, 0x31, 0x55, 0x0f, 0xbc, 0xcd, 0x01, 0xe5, 0x39, 0x18, 0x88, 0x83, 0xb2, 0xc5, \
	0x4b, 0x3b, 0x38, 0xe7)

#define TEST_DATA_INT_1_FILENAME "test-int-1"
#define TEST_DATA_INT_1_SIZE 8
#define TEST_DATA_INT_1 0xFEDCBA9876543210ULL

#define TEST_DATA_INT_2_FILENAME "test-int-2"
#define TEST_DATA_INT_2_SIZE 8
#define TEST_DATA_INT_2 0x10FE32DC54A97698ULL

#define TEST_DATA_INT_3_FILENAME "test-int-3"
#define TEST_DATA_INT_3_SIZE 8
#define TEST_DATA_INT_3 0xFA57F003B0036667ULL

#define TEST_SHA256 EMPTY_WRAP( \
	0xef, 0xc7, 0xb1, 0x0a, 0xbf, 0x54, 0x2f, 0xaa, 0x12, 0xa6, 0xeb, 0xf, 0xff, 0xf4, \
	0x19, 0xc1, 0x63, 0xf4, 0x60, 0x50, 0xc5, 0xb0, 0xbe, 0x37, 0x32, 0x11, 0x19, 0x63, \
	0x61, 0xe0, 0x53, 0xe0)

#define INVALID_SHA256 EMPTY_WRAP( \
	'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'n', 'o', 't', ' ', 'a', ' ', \
	'v', 'a', 'l', 'i', 'd', ' ', 'S', 'H', 'A', '2', '5', '6', '!', '!', \
	'!', '!', '!', '!')

extern const u8 test_data_1[TEST_DATA_1_SIZE];
extern const u8 test_data_2[TEST_DATA_2_SIZE];
extern const u8 test_data_int_1[TEST_DATA_INT_1_SIZE];
extern const u8 test_data_int_2[TEST_DATA_INT_2_SIZE];
extern const u8 test_data_int_3[TEST_DATA_INT_3_SIZE];

extern const u8 good_hash[VB2_SHA256_DIGEST_SIZE];
extern const u8 bad_hash[VB2_SHA256_DIGEST_SIZE];

extern const struct cbfs_test_file file_no_hash;
extern const struct cbfs_test_file file_valid_hash;
extern const struct cbfs_test_file file_broken_hash;
extern const struct cbfs_test_file test_file_1;
extern const struct cbfs_test_file test_file_2;
extern const struct cbfs_test_file test_file_int_1;
extern const struct cbfs_test_file test_file_int_2;
extern const struct cbfs_test_file test_file_int_3;

#endif /* TESTS_LIB_CBFS_UTIL_H */
