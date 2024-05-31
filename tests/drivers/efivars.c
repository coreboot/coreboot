/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/efi/efivars.h>
#include <vendorcode/intel/edk2/UDK2017/MdePkg/Include/Pi/PiFirmwareVolume.h>
#include <vendorcode/intel/edk2/UDK2017/MdeModulePkg/Include/Guid/VariableFormat.h>
#include <string.h>
#include <tests/test.h>
#include <types.h>

/* Dummy firmware volume header for a 0x30000 byte partition with a single entry
 * in a formatted variable store.
 */
static const uint8_t FVH[] = {
	/* EFI_FIRMWARE_VOLUME_HEADER */
	/* UINT8 ZeroVector[16] */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	/* EFI_GUID FileSystemGuid */
	0x8d, 0x2b, 0xf1, 0xff, 0x96, 0x76, 0x8b, 0x4c, 0xa9, 0x85, 0x27, 0x47, 0x07, 0x5b,
	0x4f, 0x50,
	/* UINT64 FvLength */
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* UINT32 Signature */
	0x5f, 0x46, 0x56, 0x48,
	/* EFI_FVB_ATTRIBUTES_2 Attributes */
	0x36, 0x0e, 0x00, 0x00,
	/* UINT16 HeaderLength */
	0x48, 0x00,
	/* UINT16 Checksum */
	0x00, 0xfa,
	/* UINT16 ExtHeaderOffset */
	0x00, 0x00,
	/* UINT8 Reserved[1] */
	0x00,
	/* UINT8 Revision */
	0x02,
	/* EFI_FV_BLOCK_MAP_ENTRY BlockMap[2] */
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	/* Variable Info Header */
	/* EFI_GUID Signature */
	0x78, 0x2c, 0xf3, 0xaa, 0x7b, 0x94, 0x9a, 0x43, 0xa1, 0x80, 0x2e, 0x14, 0x4e, 0xc3,
	0x77, 0x92,
	/* UINT32 Size */
	0xb8, 0xff, 0x00, 0x00,
	/* UINT8 Format */
	0x5a,
	/* UINT8 State */
	0xfe,
	/* UINT16 Reserved */
	0x00, 0x00,
	/* UINT32 Reserved1 */
	0x00, 0x00, 0x00, 0x00,
	/* AUTHENTICATED_VARIABLE_HEADER */
	/* UINT16 StartId */
	0xaa, 0x55,
	/* UINT8 State */
	0x3f,
	/* UINT8 Reserved */
	0xff,
	/* UINT32 Attributes */
	0x07, 0x00, 0x00, 0x00,
	/* UINT64 MonotonicCount */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/* EFI_TIME TimeStamp */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	/* UINT32 PubKeyIndex */
	0xff, 0xff, 0xff, 0xff,
	/* UINT32 NameSize */
	0x12, 0x00, 0x00, 0x00,
	/* UINT32 DataSize */
	0x09, 0x00, 0x00, 0x00,
	/* EFI_GUID VendorGuid */
	0x1d, 0x4c, 0xae, 0xce, 0x5b, 0x33, 0x85, 0x46, 0xa4, 0xa0, 0xfc, 0x4a,
	0x94, 0xee, 0xa0, 0x85,
	/* L"coreboot" */
	0x63, 0x00, 0x6f, 0x00, 0x72, 0x00, 0x65, 0x00, 0x62, 0x00,
	0x6f, 0x00, 0x6f, 0x00, 0x74, 0x00, 0x00, 0x00,
	/* "is great" */
	0x69, 0x73, 0x20, 0x67, 0x72, 0x65, 0x61, 0x74, 0x00,
};

#define FVH_CHECKSUMMED_SIZE (sizeof(EFI_FIRMWARE_VOLUME_HEADER) + 8 + sizeof(EFI_GUID))

static struct region_device flash_rdev_rw;
static uint8_t flash_buffer[0x30000];

static const char *name = "coreboot";

static void mock_rdev(bool init)
{
	if (init) {
		/* Emulate NOR flash by setting all bits to 1 */
		memset(flash_buffer, 0xff, sizeof(flash_buffer));
		/* Place _FVH and VIH headers, as well as test data */
		memcpy(flash_buffer, FVH, sizeof(FVH));
	}

	rdev_chain_mem_rw(&flash_rdev_rw, flash_buffer, sizeof(flash_buffer));
}

static const EFI_GUID EficorebootNvDataGuid = {
	0xceae4c1d, 0x335b, 0x4685, { 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 } };

/* Test valid and corrupted FVH header */
static void efi_test_header(void **state)
{
	enum cb_err ret;
	uint8_t buf[16];
	uint32_t size;
	int i;

	mock_rdev(true);

	/* Test variable lookup with intact header */
	size = sizeof(buf);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, buf, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_int_equal(size, strlen("is great")+1);
	assert_string_equal((const char *)buf, "is great");

	for (i = 0; i < FVH_CHECKSUMMED_SIZE; i++) {
		mock_rdev(true);

		/* Flip some bits */
		flash_buffer[i] ^= 0xff;

		size = sizeof(buf);
		ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, buf,
					&size);
		assert_int_not_equal(ret, CB_SUCCESS);
	}
}

/* Write with the same key and value should not modify the store */
static void efi_test_noop_existing_write(void **state)
{
	enum cb_err ret;
	int i;

	mock_rdev(true);

	ret = efi_fv_set_option(&flash_rdev_rw,
				&EficorebootNvDataGuid,
				name,
				"is great",
				strlen("is great") + 1);

	assert_int_equal(ret, CB_SUCCESS);

	for (i = sizeof(FVH); i < sizeof(flash_buffer); i++)
		assert_int_equal(flash_buffer[i], 0xff);
}

static void efi_test_new_write(void **state)
{
	enum cb_err ret;
	uint8_t buf[16];
	uint32_t size;
	int i;

	mock_rdev(true);

	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
				name, "is awesome", strlen("is awesome") + 1);
	assert_int_equal(ret, CB_SUCCESS);

	/* New variable has been written */
	assert_int_equal(flash_buffer[ALIGN_UP(sizeof(FVH), 4)], 0xaa);
	assert_int_equal(flash_buffer[ALIGN_UP(sizeof(FVH), 4) + 1], 0x55);

	/* Remaining space is blank */
	for (i = ALIGN_UP(sizeof(FVH), 4) + 89; i < sizeof(flash_buffer); i++)
		assert_int_equal(flash_buffer[i], 0xff);

	mock_rdev(false);

	memset(buf, 0, sizeof(buf));
	size = sizeof(buf);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, buf,
				&size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_int_equal(size, strlen("is awesome")+1);

	assert_int_equal(flash_buffer[ALIGN_UP(sizeof(FVH), 4) + 1], 0x55);
	assert_string_equal((const char *)buf, "is awesome");
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(efi_test_header),
		cmocka_unit_test(efi_test_noop_existing_write),
		cmocka_unit_test(efi_test_new_write)
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
