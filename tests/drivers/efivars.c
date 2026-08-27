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
	0x01, 0xfa,
	/* UINT16 ExtHeaderOffset */
	0x00, 0x00,
	/* UINT8 Reserved[1] */
	0x00,
	/* UINT8 Revision */
	0x02,
	/* EFI_FV_BLOCK_MAP_ENTRY BlockMap[2] */
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
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
static struct region_device fault_root_rdev;
static uint8_t flash_buffer[0x40000];
static size_t read_failure_offset;
static size_t write_failure_offset = SIZE_MAX;

static ssize_t fault_readat(const struct region_device *rdev, void *buffer, size_t offset,
			    size_t size)
{
	if (read_failure_offset >= offset && read_failure_offset - offset < size)
		return -1;
	memcpy(buffer, flash_buffer + offset, size);
	return size;
}

static ssize_t fault_writeat(const struct region_device *rdev, const void *buffer,
			     size_t offset, size_t size)
{
	if (offset == write_failure_offset)
		return -1;
	memcpy(flash_buffer + offset, buffer, size);
	return size;
}

static const struct region_device_ops fault_rdev_ops = {
	.readat = fault_readat,
	.writeat = fault_writeat,
};

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

static void mock_fault_rdev(size_t offset)
{
	mock_rdev(true);
	read_failure_offset = offset;
	write_failure_offset = SIZE_MAX;
	region_device_init(&fault_root_rdev, &fault_rdev_ops, 0, sizeof(flash_buffer));
	assert_int_equal(rdev_chain(&flash_rdev_rw, &fault_root_rdev, 0,
				    sizeof(flash_buffer)), 0);
}

static const EFI_GUID EficorebootNvDataGuid = {
	0xceae4c1d, 0x335b, 0x4685, { 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 } };

static void update_fv_checksum(EFI_FIRMWARE_VOLUME_HEADER *header)
{
	uint16_t *words = (uint16_t *)header;
	uint16_t checksum = 0;

	header->Checksum = 0;
	for (size_t i = 0; i < header->HeaderLength / sizeof(*words); i++)
		checksum += words[i];
	header->Checksum = -checksum;
}

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

static void efi_test_initialize_erased_store(void **state)
{
	enum cb_err ret;
	uint8_t buf[16];
	uint32_t size;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);

	ret = efi_fv_initialize(&flash_rdev_rw, 0x10000);
	assert_int_equal(ret, CB_SUCCESS);

	size = sizeof(buf);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, buf, &size);
	assert_int_equal(ret, CB_EFI_OPTION_NOT_FOUND);

	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
				name, "persists", strlen("persists") + 1);
	assert_int_equal(ret, CB_SUCCESS);

	size = sizeof(buf);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, buf, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_string_equal((const char *)buf, "persists");
}

static void efi_test_initialize_odd_block_store(void **state)
{
	struct region_device rdev;
	EFI_FIRMWARE_VOLUME_HEADER volume;
	VARIABLE_STORE_HEADER store;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);
	rdev_chain(&rdev, &flash_rdev_rw, 0, 7 * 0x8000);
	assert_int_equal(efi_fv_initialize(&rdev, 0x8000), CB_SUCCESS);

	memcpy(&volume, flash_buffer, sizeof(volume));
	memcpy(&store, flash_buffer + volume.HeaderLength, sizeof(store));
	assert_int_equal(volume.FvLength, 7 * 0x8000);
	/* Three spare blocks and one working block leave three for variables. */
	assert_int_equal(store.Size, 3 * 0x8000 - volume.HeaderLength);
}

static void efi_test_refuse_non_erased_store(void **state)
{
	enum cb_err ret;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	flash_buffer[sizeof(flash_buffer) - 1] = 0xfe;
	mock_rdev(false);

	ret = efi_fv_initialize(&flash_rdev_rw, 0x10000);
	assert_int_equal(ret, CB_EFI_FVH_INVALID);
	for (size_t i = 0; i < sizeof(flash_buffer) - 1; i++)
		assert_int_equal(flash_buffer[i], 0xff);
	assert_int_equal(flash_buffer[sizeof(flash_buffer) - 1], 0xfe);
}

static void efi_test_complete_interrupted_initialization(void **state)
{
	const size_t initial_header_size = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	uint8_t expected[128];
	enum cb_err ret;

	assert_true(initial_header_size <= sizeof(expected));
	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);
	assert_int_equal(efi_fv_initialize(&flash_rdev_rw, 0x10000), CB_SUCCESS);
	memcpy(expected, flash_buffer, initial_header_size);

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	memcpy(flash_buffer, expected, initial_header_size / 2);
	ret = efi_fv_initialize(&flash_rdev_rw, 0x10000);
	assert_int_equal(ret, CB_SUCCESS);
	assert_memory_equal(flash_buffer, expected, initial_header_size);
}

static void efi_test_reject_invalid_initializer_geometry(void **state)
{
	const size_t exact_block_size = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	struct region_device exact_rdev;
	struct region_device tiny_rdev;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);
	assert_int_equal(rdev_chain(&tiny_rdev, &flash_rdev_rw, 0, 128), 0);
	assert_int_equal(efi_fv_initialize(&tiny_rdev, 32), CB_ERR_ARG);
	assert_int_equal(rdev_chain(&exact_rdev, &flash_rdev_rw, 0,
		4 * exact_block_size), 0);
	assert_int_equal(efi_fv_initialize(&exact_rdev, exact_block_size), CB_ERR_ARG);
#if __SIZEOF_SIZE_T__ > 4
	assert_int_equal(efi_fv_initialize(&flash_rdev_rw,
					   (size_t)UINT32_MAX + 1), CB_ERR_ARG);
#endif
}

static void efi_test_reject_wrapped_variable_size(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[4];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);
	assert_int_equal(efi_fv_initialize(&flash_rdev_rw, 0x10000), CB_SUCCESS);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer +
		sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) +
		sizeof(VARIABLE_STORE_HEADER));
	header->StartId = VARIABLE_DATA;
	header->State = VAR_ADDED;
	header->NameSize = UINT32_MAX - sizeof(*header) + 1;
	header->DataSize = UINT32_MAX;

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
				name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);
}

static void efi_test_reject_invalid_fv_geometry(void **state)
{
	EFI_FIRMWARE_VOLUME_HEADER *header;
	uint8_t value[4];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	header->FvLength = header->HeaderLength - 1;
	update_fv_checksum(header);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_FVH_INVALID);

	mock_rdev(true);
	header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	header->BlockMap[0].NumBlocks--;
	update_fv_checksum(header);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_FVH_INVALID);

	mock_rdev(true);
	header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	header->BlockMap[1].NumBlocks = 1;
	header->BlockMap[1].Length = 1;
	update_fv_checksum(header);
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_FVH_INVALID);
}

static void efi_test_reject_malformed_variable_name(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer + sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER));
	header->NameSize--;
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);

	mock_rdev(true);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer + sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER));
	flash_buffer[(uint8_t *)header - flash_buffer + sizeof(*header) +
		header->NameSize - 2] = 'x';
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);
}

static void efi_test_reject_non_erased_terminator(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer +
		sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) +
		sizeof(VARIABLE_STORE_HEADER));
	header->StartId = 0;
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);
}

static void efi_test_ignore_uncommitted_variable(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer +
		sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) +
		sizeof(VARIABLE_STORE_HEADER));
	header->State = VAR_HEADER_VALID_ONLY;
	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_OPTION_NOT_FOUND);
}

static void efi_test_validate_tail_after_match(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	size_t variable_offset, variable_size;
	enum cb_err ret;

	mock_rdev(true);
	variable_offset = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer + variable_offset);
	variable_size = ALIGN_UP(sizeof(*header) + header->NameSize + header->DataSize,
		HEADER_ALIGNMENT);
	flash_buffer[variable_offset + variable_size] = 0;

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);
}

static void efi_test_find_zero_length_variable(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	size_t data_offset, old_variable_size;
	enum cb_err ret;

	mock_rdev(true);
	header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer +
		sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) +
		sizeof(VARIABLE_STORE_HEADER));
	data_offset = (uint8_t *)header - flash_buffer + sizeof(*header) + header->NameSize;
	old_variable_size = ALIGN_UP(sizeof(*header) + header->NameSize + header->DataSize,
		HEADER_ALIGNMENT);
	memset(flash_buffer + data_offset, 0xff,
		old_variable_size - sizeof(*header) - header->NameSize);
	header->DataSize = 0;

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_int_equal(size, 0);
}

static void efi_test_skip_incomplete_variable(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *old_header, *incomplete;
	uint8_t value[16], expected[16];
	uint32_t size = sizeof(expected);
	size_t variable_size;

	mock_rdev(true);
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, expected, &size), CB_SUCCESS);
	old_header = (void *)(flash_buffer + sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER));
	variable_size = ALIGN_UP(sizeof(*old_header) + old_header->NameSize +
		old_header->DataSize, HEADER_ALIGNMENT);
	incomplete = (void *)((uint8_t *)old_header + variable_size);
	memcpy(incomplete, old_header, sizeof(*incomplete));
	incomplete->State = VAR_HEADER_VALID_ONLY;
	old_header->State = VAR_IN_DELETED_TRANSITION & VAR_ADDED;

	uint32_t read_size = sizeof(value);
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, value, &read_size), CB_SUCCESS);
	assert_int_equal(read_size, size);
	assert_memory_equal(value, expected, size);
	assert_int_equal(efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, "recovered", sizeof("recovered")), CB_SUCCESS);
	read_size = sizeof(value);
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, value, &read_size), CB_SUCCESS);
	assert_string_equal((const char *)value, "recovered");
}

static void efi_test_interrupted_header_state_write(void **state)
{
	uint8_t value[16];
	uint32_t size = sizeof(value);
	AUTHENTICATED_VARIABLE_HEADER *incomplete;

	mock_fault_rdev(SIZE_MAX);
	/* The replacement starts after the existing variable and its padding. */
	write_failure_offset = ALIGN_UP(sizeof(FVH), HEADER_ALIGNMENT) +
		offsetof(AUTHENTICATED_VARIABLE_HEADER, State);
	assert_int_equal(efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, "recovered", sizeof("recovered")), CB_EFI_ACCESS_ERROR);
	incomplete = (void *)(flash_buffer + ALIGN_UP(sizeof(FVH), HEADER_ALIGNMENT));
	assert_int_equal(incomplete->StartId, VARIABLE_DATA);
	assert_int_equal(incomplete->State, UINT8_MAX);

	write_failure_offset = SIZE_MAX;
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, value, &size), CB_SUCCESS);
	assert_string_equal((const char *)value, "is great");
	assert_int_equal(efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, "recovered", sizeof("recovered")), CB_SUCCESS);
	size = sizeof(value);
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, value, &size), CB_SUCCESS);
	assert_string_equal((const char *)value, "recovered");
}

static void efi_test_prefer_added_over_transition(void **state)
{
	AUTHENTICATED_VARIABLE_HEADER *old_header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid, name,
		"is awesome", strlen("is awesome") + 1);
	assert_int_equal(ret, CB_SUCCESS);

	old_header = (AUTHENTICATED_VARIABLE_HEADER *)(flash_buffer +
		sizeof(EFI_FIRMWARE_VOLUME_HEADER) + sizeof(EFI_FV_BLOCK_MAP_ENTRY) +
		sizeof(VARIABLE_STORE_HEADER));
	old_header->State = VAR_IN_DELETED_TRANSITION & VAR_ADDED;
	mock_rdev(false);

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_string_equal((const char *)value, "is awesome");
}

static void efi_test_accept_full_store(void **state)
{
	EFI_FIRMWARE_VOLUME_HEADER *fv_header;
	VARIABLE_STORE_HEADER *store_header;
	AUTHENTICATED_VARIABLE_HEADER *variable_header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	size_t variable_size;
	enum cb_err ret;

	mock_rdev(true);
	fv_header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	store_header = (VARIABLE_STORE_HEADER *)(flash_buffer + fv_header->HeaderLength);
	variable_header = (AUTHENTICATED_VARIABLE_HEADER *)(store_header + 1);
	variable_size = ALIGN_UP(sizeof(*variable_header) + variable_header->NameSize +
		variable_header->DataSize, HEADER_ALIGNMENT);
	store_header->Size = sizeof(*store_header) + variable_size;

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_string_equal((const char *)value, "is great");
	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid, "new",
		value, size);
	assert_int_equal(ret, CB_EFI_STORE_FULL);
}

static void efi_test_reject_empty_variable_region(void **state)
{
	EFI_FIRMWARE_VOLUME_HEADER *fv_header;
	VARIABLE_STORE_HEADER *store_header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	enum cb_err ret;

	mock_rdev(true);
	fv_header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	store_header = (VARIABLE_STORE_HEADER *)(flash_buffer + fv_header->HeaderLength);
	store_header->Size = sizeof(*store_header);

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_VS_CORRUPTED_INVALID);
}

static void efi_test_accept_short_erased_tail(void **state)
{
	EFI_FIRMWARE_VOLUME_HEADER *fv_header;
	VARIABLE_STORE_HEADER *store_header;
	AUTHENTICATED_VARIABLE_HEADER *variable_header;
	uint8_t value[16];
	uint32_t size = sizeof(value);
	size_t variable_size;
	enum cb_err ret;

	mock_rdev(true);
	fv_header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	store_header = (VARIABLE_STORE_HEADER *)(flash_buffer + fv_header->HeaderLength);
	variable_header = (AUTHENTICATED_VARIABLE_HEADER *)(store_header + 1);
	variable_size = ALIGN_UP(sizeof(*variable_header) + variable_header->NameSize +
		variable_header->DataSize, HEADER_ALIGNMENT);
	store_header->Size = sizeof(*store_header) + variable_size + 1;

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_SUCCESS);
	assert_string_equal((const char *)value, "is great");
}

static void efi_test_propagate_name_read_error(void **state)
{
	uint8_t value[16];
	uint32_t size = sizeof(value);
	size_t variable_offset;
	enum cb_err ret;

	variable_offset = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	mock_fault_rdev(variable_offset + sizeof(AUTHENTICATED_VARIABLE_HEADER));

	ret = efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid, name, value, &size);
	assert_int_equal(ret, CB_EFI_ACCESS_ERROR);
}

static void efi_test_propagate_print_name_read_error(void **state)
{
	size_t variable_offset;
	enum cb_err ret;

	variable_offset = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	mock_fault_rdev(variable_offset + sizeof(AUTHENTICATED_VARIABLE_HEADER));

	ret = efi_fv_print_options(&flash_rdev_rw);
	assert_int_equal(ret, CB_EFI_ACCESS_ERROR);
}

static void efi_test_propagate_data_read_error(void **state)
{
	size_t variable_offset;
	enum cb_err ret;

	variable_offset = sizeof(EFI_FIRMWARE_VOLUME_HEADER) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY) + sizeof(VARIABLE_STORE_HEADER);
	mock_fault_rdev(variable_offset + sizeof(AUTHENTICATED_VARIABLE_HEADER) +
		(strlen(name) + 1) * sizeof(CHAR16));

	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid, name,
		"is great", strlen("is great") + 1);
	assert_int_equal(ret, CB_EFI_ACCESS_ERROR);
}

static void efi_test_exact_fit_write(void **state)
{
	EFI_FIRMWARE_VOLUME_HEADER *fv_header;
	VARIABLE_STORE_HEADER *store_header;
	const size_t name_size = (strlen(name) + 1) * sizeof(CHAR16);
	const size_t variable_size = ALIGN_UP(sizeof(AUTHENTICATED_VARIABLE_HEADER) +
		name_size + sizeof(uint32_t), HEADER_ALIGNMENT);
	uint32_t value = 1;
	enum cb_err ret;

	memset(flash_buffer, 0xff, sizeof(flash_buffer));
	mock_rdev(false);
	assert_int_equal(efi_fv_initialize(&flash_rdev_rw, 0x10000), CB_SUCCESS);
	fv_header = (EFI_FIRMWARE_VOLUME_HEADER *)flash_buffer;
	store_header = (VARIABLE_STORE_HEADER *)(flash_buffer + fv_header->HeaderLength);
	store_header->Size = sizeof(*store_header) + variable_size;

	ret = efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid, name,
		&value, sizeof(value));
	assert_int_equal(ret, CB_SUCCESS);
	uint32_t read_value = 0;
	uint32_t read_size = sizeof(read_value);
	assert_int_equal(efi_fv_get_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		name, &read_value, &read_size), CB_SUCCESS);
	assert_int_equal(read_value, value);
	assert_int_equal(efi_fv_set_option(&flash_rdev_rw, &EficorebootNvDataGuid,
		"new", &value, sizeof(value)), CB_EFI_STORE_FULL);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(efi_test_header),
		cmocka_unit_test(efi_test_noop_existing_write),
		cmocka_unit_test(efi_test_new_write),
		cmocka_unit_test(efi_test_initialize_erased_store),
		cmocka_unit_test(efi_test_initialize_odd_block_store),
		cmocka_unit_test(efi_test_refuse_non_erased_store),
		cmocka_unit_test(efi_test_complete_interrupted_initialization),
		cmocka_unit_test(efi_test_reject_invalid_initializer_geometry),
		cmocka_unit_test(efi_test_reject_wrapped_variable_size),
		cmocka_unit_test(efi_test_reject_invalid_fv_geometry),
		cmocka_unit_test(efi_test_reject_malformed_variable_name),
		cmocka_unit_test(efi_test_reject_non_erased_terminator),
		cmocka_unit_test(efi_test_ignore_uncommitted_variable),
		cmocka_unit_test(efi_test_validate_tail_after_match),
		cmocka_unit_test(efi_test_find_zero_length_variable),
		cmocka_unit_test(efi_test_prefer_added_over_transition),
		cmocka_unit_test(efi_test_skip_incomplete_variable),
		cmocka_unit_test(efi_test_interrupted_header_state_write),
		cmocka_unit_test(efi_test_accept_full_store),
		cmocka_unit_test(efi_test_reject_empty_variable_region),
		cmocka_unit_test(efi_test_accept_short_erased_tail),
		cmocka_unit_test(efi_test_propagate_name_read_error),
		cmocka_unit_test(efi_test_propagate_print_name_read_error),
		cmocka_unit_test(efi_test_propagate_data_read_error),
		cmocka_unit_test(efi_test_exact_fit_write)
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
