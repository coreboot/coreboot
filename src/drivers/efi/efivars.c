/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <console/console.h>

#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiFirmwareVolume.h>
#include <Guid/VariableFormat.h>

#include "efivars.h"

#define PREFIX "EFIVARS: "

static const EFI_GUID EfiVariableGuid = {
	0xddcf3616, 0x3275, 0x4164, { 0x98, 0xb6, 0xfe, 0x85, 0x70, 0x7f, 0xfe, 0x7d } };
static const EFI_GUID EfiAuthenticatedVariableGuid = {
	0xaaf32c78, 0x947b, 0x439a, { 0xa1, 0x80, 0x2e, 0x14, 0x4e, 0xc3, 0x77, 0x92 } };
static const EFI_GUID EfiSystemNvDataFvGuid = {
	0xfff12b8d, 0x7696, 0x4c8b, { 0xa9, 0x85, 0x27, 0x47, 0x07, 0x5b, 0x4f, 0x50 } };

static void print_guid(int log_level, const EFI_GUID *g)
{
	printk(log_level, "GUID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x",
		g->Data1, g->Data2, g->Data3, g->Data4[0], g->Data4[1], g->Data4[2],
		g->Data4[3], g->Data4[4], g->Data4[5], g->Data4[6], g->Data4[7]);
}

static bool compare_guid(const EFI_GUID *a, const EFI_GUID *b)
{
	return memcmp(a, b, sizeof(*a)) == 0;
}

struct efi_fv_initial_image {
	struct {
		EFI_FIRMWARE_VOLUME_HEADER fv;
		EFI_FV_BLOCK_MAP_ENTRY terminator;
	} volume;
	VARIABLE_STORE_HEADER store;
};

static enum cb_err region_can_complete_initialization(
	const struct region_device *rdev, const struct efi_fv_initial_image *image,
	size_t image_size)
{
	uint8_t buf[64];
	const uint8_t *expected = (const uint8_t *)image;
	const uint8_t *mapping;
	size_t offset = 0;
	enum cb_err ret = CB_SUCCESS;

	mapping = rdev_mmap_full(rdev);
	if (mapping) {
		for (size_t i = 0; i < region_device_sz(rdev); i++) {
			const uint8_t target = i < image_size ? expected[i] : UINT8_MAX;

			/* SPI programming may only clear bits from the erased state. */
			if ((mapping[i] & target) != target) {
				ret = CB_EFI_FVH_INVALID;
				break;
			}
		}
		rdev_munmap(rdev, (void *)mapping);
		return ret;
	}

	while (offset < region_device_sz(rdev)) {
		size_t size = MIN(sizeof(buf), region_device_sz(rdev) - offset);

		if (rdev_readat(rdev, buf, offset, size) != size)
			return CB_EFI_ACCESS_ERROR;
		for (size_t i = 0; i < size; i++) {
			const size_t position = offset + i;
			const uint8_t target = position < image_size ?
				expected[position] : UINT8_MAX;

			/* SPI programming may only clear bits from the erased state. */
			if ((buf[i] & target) != target)
				return CB_EFI_FVH_INVALID;
		}
		offset += size;
	}

	return CB_SUCCESS;
}

static bool region_starts_with(const struct region_device *rdev,
			       const void *expected, size_t expected_size)
{
	uint8_t buf[64];
	const uint8_t *bytes = expected;
	size_t offset = 0;

	while (offset < expected_size) {
		const size_t size = MIN(sizeof(buf), expected_size - offset);

		if (rdev_readat(rdev, buf, offset, size) != size ||
		    memcmp(buf, bytes + offset, size))
			return false;
		offset += size;
	}

	return true;
}

static enum cb_err region_is_erased(const struct region_device *rdev, bool *erased)
{
	uint8_t buf[64];
	const uint8_t *mapping;
	size_t offset = 0;

	*erased = false;
	mapping = rdev_mmap_full(rdev);
	if (mapping) {
		for (size_t i = 0; i < region_device_sz(rdev); i++) {
			if (mapping[i] != UINT8_MAX) {
				rdev_munmap(rdev, (void *)mapping);
				return CB_SUCCESS;
			}
		}
		rdev_munmap(rdev, (void *)mapping);
		*erased = true;
		return CB_SUCCESS;
	}

	while (offset < region_device_sz(rdev)) {
		const size_t size = MIN(sizeof(buf), region_device_sz(rdev) - offset);

		if (rdev_readat(rdev, buf, offset, size) != size)
			return CB_EFI_ACCESS_ERROR;
		for (size_t i = 0; i < size; i++)
			if (buf[i] != UINT8_MAX)
				return CB_SUCCESS;
		offset += size;
	}

	*erased = true;
	return CB_SUCCESS;
}

enum cb_err efi_fv_initialize(const struct region_device *rdev, size_t block_size)
{
	struct efi_fv_initial_image image;
	size_t region_size = region_device_sz(rdev);
	size_t variable_store_size;
	size_t number_of_blocks;
	const size_t image_size = sizeof(image.volume) + sizeof(image.store);
	uint16_t checksum = 0;
	enum cb_err ret;

	if (!block_size || block_size > UINT32_MAX || region_size % block_size)
		return CB_ERR_ARG;

	number_of_blocks = region_size / block_size;
	if (number_of_blocks < 4 || number_of_blocks > UINT32_MAX)
		return CB_ERR_ARG;

	variable_store_size = (number_of_blocks - number_of_blocks / 2 - 1) * block_size;
	if (variable_store_size <= sizeof(image.volume) + sizeof(image.store))
		return CB_ERR_ARG;
	variable_store_size -= sizeof(image.volume);
	if (variable_store_size > UINT32_MAX)
		return CB_ERR_ARG;

	memset(&image, 0, sizeof(image));
	image.volume.fv.FileSystemGuid = EfiSystemNvDataFvGuid;
	image.volume.fv.FvLength = region_size;
	image.volume.fv.Signature = EFI_FVH_SIGNATURE;
	image.volume.fv.Attributes = EFI_FVB2_READ_ENABLED_CAP | EFI_FVB2_READ_STATUS |
		EFI_FVB2_WRITE_ENABLED_CAP | EFI_FVB2_WRITE_STATUS |
		EFI_FVB2_STICKY_WRITE | EFI_FVB2_MEMORY_MAPPED | EFI_FVB2_ERASE_POLARITY;
	image.volume.fv.HeaderLength = sizeof(image.volume);
	image.volume.fv.Revision = EFI_FVH_REVISION;
	image.volume.fv.BlockMap[0].NumBlocks = number_of_blocks;
	image.volume.fv.BlockMap[0].Length = block_size;

	for (size_t i = 0; i < sizeof(image.volume) / sizeof(uint16_t); i++)
		checksum += ((const uint16_t *)&image.volume)[i];
	image.volume.fv.Checksum = -checksum;

	image.store.Signature = EfiAuthenticatedVariableGuid;
	image.store.Size = variable_store_size;
	image.store.Format = VARIABLE_STORE_FORMATTED;
	image.store.State = VARIABLE_STORE_HEALTHY;

	ret = region_can_complete_initialization(rdev, &image, image_size);
	if (ret != CB_SUCCESS)
		return ret;

	if (rdev_writeat(rdev, &image, 0, image_size) != image_size ||
	    !region_starts_with(rdev, &image, image_size))
		return CB_EFI_ACCESS_ERROR;

	return CB_SUCCESS;
}

/* Reads the CHAR16 string from rdev at offset and prints it */
static enum cb_err rdev_print_wchar(int log_level, struct region_device *rdev, size_t offset)
{
	CHAR16 c;
	int i = 0;

	/* Convert ASCII to UTF-16 */
	do {
		if (rdev_readat(rdev, &c, offset + i * sizeof(c), sizeof(c)) != sizeof(c))
			return CB_EFI_ACCESS_ERROR;
		if (c < 0x80)
			printk(log_level, "%c", (char)c);
		else
			printk(log_level, "\\u%04x", c);

		i++;
	} while (c);
	return CB_SUCCESS;
}

/* Convert an ASCII string to UTF-16 and write it to the rdev starting at offset. */
static enum cb_err rdev_write_wchar(struct region_device *rdev, size_t offset, const char *msg)
{
	size_t i;
	CHAR16 c;

	/* Convert ASCII to UTF-16 */
	for (i = 0; i < strlen(msg) + 1; i++) {
		c = msg[i];

		if (rdev_writeat(rdev, &c, offset + i * sizeof(c), sizeof(c)) != sizeof(c))
			return CB_EFI_ACCESS_ERROR;
	}
	return CB_SUCCESS;
}

/* Read a bounded UTF-16 string from rdev and compare it to an ASCII string. */
static int rdev_strcmp_wchar_ascii(struct region_device *rdev, size_t offset,
				   size_t size, const char *msg)
{
	size_t i;
	CHAR16 c;
	int r;

	if (!size || size % sizeof(c))
		return CB_EFI_VS_CORRUPTED_INVALID;

	/* Compare UTF-16 and ASCII */
	for (i = 0; i < size / sizeof(c); i++) {
		if (rdev_readat(rdev, &c, offset + i * sizeof(c), sizeof(c)) != sizeof(c))
			return CB_EFI_ACCESS_ERROR;
		if ((r = (c - msg[i])) != 0 || !c)
			return r;
	}

	return CB_EFI_VS_CORRUPTED_INVALID;
}

/* Compare an rdev region and a data buffer */
static int rdev_memcmp(struct region_device *rdev, size_t offset, uint8_t *data, size_t size)
{
	uint8_t buf[16];
	size_t i;
	int r;

	i = 0;
	while (size >= sizeof(buf)) {
		if (rdev_readat(rdev, buf, offset + i, sizeof(buf)) != sizeof(buf))
			return CB_EFI_ACCESS_ERROR;
		r = memcmp(buf, data + i, sizeof(buf));
		if (r != 0)
			return r;
		i += sizeof(buf);
		size -= sizeof(buf);
	}
	while (size > 0) {
		if (rdev_readat(rdev, buf, offset + i, 1) != 1)
			return CB_EFI_ACCESS_ERROR;
		r = buf[0] - data[i];
		if (r != 0)
			return r;
		i++;
		size--;
	}
	return 0;
}


static enum cb_err validate_fv_header(const struct region_device *rdev,
				      EFI_FIRMWARE_VOLUME_HEADER *fw_vol_hdr)
{
	const size_t minimum_header_size = sizeof(*fw_vol_hdr) +
		sizeof(EFI_FV_BLOCK_MAP_ENTRY);
	const size_t block_map_offset = offsetof(EFI_FIRMWARE_VOLUME_HEADER, BlockMap);
	EFI_FV_BLOCK_MAP_ENTRY block;
	uint64_t mapped_length = 0;
	bool block_map_terminated = false;
	uint16_t checksum, data;
	size_t i, offset;

	if (rdev_readat(rdev, fw_vol_hdr, 0, sizeof(*fw_vol_hdr)) != sizeof(*fw_vol_hdr))
		return CB_EFI_ACCESS_ERROR;

	/*
	 * Verify the header revision, header signature, length
	 * Length of FvBlock cannot be 2**64-1
	 * HeaderLength cannot be an odd number
	 */
	if ((fw_vol_hdr->Revision != EFI_FVH_REVISION)
	    || (fw_vol_hdr->Signature != EFI_FVH_SIGNATURE)
	    || (fw_vol_hdr->FvLength > region_device_sz(rdev))
	    || (fw_vol_hdr->HeaderLength < minimum_header_size)
	    || (fw_vol_hdr->HeaderLength > fw_vol_hdr->FvLength)
	    || (fw_vol_hdr->FvLength - fw_vol_hdr->HeaderLength <
		sizeof(VARIABLE_STORE_HEADER))
	    || (fw_vol_hdr->HeaderLength & 1)) {
		printk(BIOS_WARNING, PREFIX "No Firmware Volume header present\n");
		return CB_EFI_FVH_INVALID;
	}

	/* Check the Firmware Volume Guid */
	if (!compare_guid(&fw_vol_hdr->FileSystemGuid, &EfiSystemNvDataFvGuid)) {
		printk(BIOS_WARNING, PREFIX "Firmware Volume Guid non-compatible\n");
		return CB_EFI_FVH_INVALID;
	}

	/* Verify the header checksum */
	checksum = 0;
	for (i = 0; i < fw_vol_hdr->HeaderLength; i += 2) {
		if (rdev_readat(rdev, &data, i, sizeof(data)) != sizeof(data))
			return CB_EFI_ACCESS_ERROR;
		checksum = (uint16_t)(checksum + data); /* intentionally overflows */
	}
	if (checksum != 0) {
		printk(BIOS_WARNING, PREFIX "FV checksum is invalid: 0x%X\n", checksum);
		return CB_EFI_CHECKSUM_INVALID;
	}

	for (offset = block_map_offset;
	     offset <= fw_vol_hdr->HeaderLength - sizeof(block); offset += sizeof(block)) {
		if (rdev_readat(rdev, &block, offset, sizeof(block)) != sizeof(block))
			return CB_EFI_ACCESS_ERROR;

		if (!block.NumBlocks && !block.Length) {
			block_map_terminated = true;
			break;
		}
		if (!block.NumBlocks || !block.Length ||
		    block.NumBlocks > (UINT64_MAX - mapped_length) / block.Length)
			return CB_EFI_FVH_INVALID;

		mapped_length += (uint64_t)block.NumBlocks * block.Length;
	}

	if (!block_map_terminated || mapped_length != fw_vol_hdr->FvLength) {
		printk(BIOS_WARNING, PREFIX "Firmware Volume block map is invalid\n");
		return CB_EFI_FVH_INVALID;
	}

	printk(BIOS_SPEW, PREFIX "UEFI FV with size %lld found\n", fw_vol_hdr->FvLength);

	return CB_SUCCESS;
}

static enum cb_err
validate_variable_store_header(const EFI_FIRMWARE_VOLUME_HEADER  *fv_hdr,
			       struct region_device *rdev,
			       bool *auth_format)
{
	VARIABLE_STORE_HEADER hdr;
	size_t length;

	if (rdev_readat(rdev, &hdr, fv_hdr->HeaderLength, sizeof(hdr)) != sizeof(hdr))
		return CB_EFI_ACCESS_ERROR;

	/* Check the Variable Store Guid */
	if (!compare_guid(&hdr.Signature, &EfiVariableGuid) &&
	    !compare_guid(&hdr.Signature, &EfiAuthenticatedVariableGuid)) {
		printk(BIOS_WARNING, PREFIX "Variable Store Guid non-compatible\n");
		return CB_EFI_VS_CORRUPTED_INVALID;
	}

	*auth_format = compare_guid(&hdr.Signature, &EfiAuthenticatedVariableGuid);

	length = fv_hdr->FvLength - fv_hdr->HeaderLength;
	if (hdr.Size <= sizeof(hdr) || hdr.Size > length) {
		printk(BIOS_WARNING, PREFIX "Variable Store Length does not match\n");
		return CB_EFI_VS_CORRUPTED_INVALID;
	}

	if (hdr.Format != VARIABLE_STORE_FORMATTED)
		return CB_EFI_VS_NOT_FORMATTED_INVALID;

	if (hdr.State != VARIABLE_STORE_HEALTHY)
		return CB_EFI_VS_CORRUPTED_INVALID;

	if (rdev_chain(rdev, rdev, fv_hdr->HeaderLength + sizeof(hdr),
		       hdr.Size - sizeof(hdr))) {
		printk(BIOS_WARNING, PREFIX "rdev_chain failed\n");
		return CB_EFI_ACCESS_ERROR;
	}

	printk(BIOS_SPEW, PREFIX "UEFI variable store with size %zu found\n",
		region_device_sz(rdev));

	return CB_SUCCESS;
}

struct efi_find_args {
	const EFI_GUID *guid;
	const char *name;
	uint32_t *size;
	uint32_t capacity;
	void *data;
};

static enum cb_err match(struct region_device *rdev, VARIABLE_HEADER *hdr, size_t hdr_size,
			 const char *name, const EFI_GUID *guid, bool *matched)
{
	size_t expected_name_size;
	int ret;

	*matched = false;

	/* Only search for valid or in transition to be deleted variables */
	if ((hdr->State != VAR_ADDED) &&
	    (hdr->State != (VAR_IN_DELETED_TRANSITION & VAR_ADDED)))
		return CB_SUCCESS;

	if (!compare_guid(&hdr->VendorGuid, guid))
		return CB_SUCCESS;

	if (strlen(name) > (SIZE_MAX / sizeof(CHAR16)) - 1)
		return CB_SUCCESS;
	expected_name_size = (strlen(name) + 1) * sizeof(CHAR16);
	if (hdr->NameSize != expected_name_size)
		return CB_SUCCESS;

	ret = rdev_strcmp_wchar_ascii(rdev, hdr_size, hdr->NameSize, name);
	if (ret == CB_EFI_ACCESS_ERROR || ret == CB_EFI_VS_CORRUPTED_INVALID)
		return ret;
	if (ret)
		return CB_SUCCESS;

	*matched = true;
	return CB_SUCCESS;
}

static
enum cb_err find_and_copy(struct region_device *rdev, VARIABLE_HEADER *hdr, size_t hdr_size,
			  void *arg, bool *stop)
{
	struct efi_find_args *fa = (struct efi_find_args *)arg;
	bool matched;
	enum cb_err ret;

	ret = match(rdev, hdr, hdr_size, fa->name, fa->guid, &matched);
	if (ret != CB_SUCCESS || !matched)
		return ret;

	*stop = true;
	if (fa->capacity < hdr->DataSize)
		return CB_EFI_BUFFER_TOO_SMALL;

	if (hdr->DataSize &&
	    rdev_readat(rdev, fa->data, hdr_size + hdr->NameSize, hdr->DataSize) !=
			hdr->DataSize)
		return CB_EFI_ACCESS_ERROR;

	*(fa->size) = hdr->DataSize;
	return CB_SUCCESS;
}

struct efi_find_compare_args {
	const EFI_GUID *guid;
	const char *name;
	uint32_t size;
	void *data;
	bool match;
};

static
enum cb_err find_and_compare(struct region_device *rdev, VARIABLE_HEADER *hdr, size_t hdr_size,
			     void *arg, bool *stop)
{
	struct efi_find_compare_args *fa = (struct efi_find_compare_args *)arg;
	bool matched;
	int compare;
	enum cb_err ret;

	ret = match(rdev, hdr, hdr_size, fa->name, fa->guid, &matched);
	if (ret != CB_SUCCESS || !matched)
		return ret;

	*stop = true;
	if (fa->size != hdr->DataSize) {
		fa->match = false;
		return CB_SUCCESS;
	}

	compare = rdev_memcmp(rdev, hdr_size + hdr->NameSize, fa->data, hdr->DataSize);
	if (compare == CB_EFI_ACCESS_ERROR)
		return CB_EFI_ACCESS_ERROR;
	fa->match = compare == 0;

	return CB_SUCCESS;
}

static enum cb_err noop(struct region_device *rdev, VARIABLE_HEADER *hdr, size_t hdr_size,
			void *arg, bool *stop)
{
	/* Does nothing. */
	return CB_SUCCESS;
}

static enum cb_err print_var(struct region_device *rdev, VARIABLE_HEADER *hdr, size_t hdr_size,
			     void *arg, bool *stop)
{
	uint8_t buf[16];
	size_t len, i;

	printk(BIOS_DEBUG, "%08zx: Var ", region_device_offset(rdev));
	print_guid(BIOS_DEBUG, &hdr->VendorGuid);

	printk(BIOS_DEBUG, "-");

	if (rdev_print_wchar(BIOS_DEBUG, rdev, hdr_size) != CB_SUCCESS)
		return CB_EFI_ACCESS_ERROR;

	printk(BIOS_DEBUG, ", State %02x, Size %02x\n", hdr->State, hdr->DataSize);

	if (hdr->DataSize && hdr->NameSize) {
		len = sizeof(buf) < hdr->DataSize ? sizeof(buf) : hdr->DataSize;
		if (rdev_readat(rdev, buf, hdr_size + hdr->NameSize, len) != len)
			return CB_EFI_ACCESS_ERROR;
		printk(BIOS_DEBUG, "  Data: ");

		for (i = 0; i < len; i++)
			printk(BIOS_DEBUG, "0x%02x ", buf[i]);

		if (hdr->DataSize > len)
			printk(BIOS_DEBUG, "...");

		printk(BIOS_DEBUG, "\n");
	}

	return CB_SUCCESS;
}

static bool variable_state_is_valid(uint8_t state)
{
	return state == UINT8_MAX || state == VAR_HEADER_VALID_ONLY || state == VAR_ADDED ||
		state == (VAR_IN_DELETED_TRANSITION & VAR_ADDED) ||
		state == (VAR_DELETED & VAR_ADDED) ||
		state == (VAR_DELETED & VAR_IN_DELETED_TRANSITION & VAR_ADDED);
}

static enum cb_err walk_variables(struct region_device *rdev,
				  bool auth_format,
				  enum cb_err (*walker)(struct region_device *rdev,
						   VARIABLE_HEADER *hdr,
						   size_t hdr_size,
						   void *arg,
						   bool *stop),
				  void *walker_arg, bool *full)
{
	AUTHENTICATED_VARIABLE_HEADER auth_hdr;
	size_t header_size, var_size;
	VARIABLE_HEADER hdr;
	CHAR16 terminator;
	bool erased;
	bool stop;
	bool walker_stopped = false;
	bool walker_done = false;
	enum cb_err ret;
	enum cb_err walker_ret = CB_EFI_OPTION_NOT_FOUND;
	struct region_device stopped_rdev;

	if (full)
		*full = false;

	if (auth_format)
		header_size = sizeof(AUTHENTICATED_VARIABLE_HEADER);
	else
		header_size = sizeof(VARIABLE_HEADER);

	do {
		if (region_device_sz(rdev) < header_size) {
			ret = region_is_erased(rdev, &erased);
			if (ret != CB_SUCCESS)
				return ret;
			if (!erased)
				return CB_EFI_VS_CORRUPTED_INVALID;
			if (walker_stopped)
				*rdev = stopped_rdev;
			return walker_ret;
		}
		if (auth_format) {
			if (rdev_readat(rdev, &auth_hdr, 0, sizeof(auth_hdr))
					!= sizeof(auth_hdr))
				return CB_EFI_ACCESS_ERROR;
			hdr.Reserved = auth_hdr.Reserved;
			hdr.StartId = auth_hdr.StartId;
			hdr.State = auth_hdr.State;
			hdr.Attributes = auth_hdr.Attributes;
			hdr.NameSize = auth_hdr.NameSize;
			hdr.DataSize = auth_hdr.DataSize;
			memcpy(&hdr.VendorGuid, &auth_hdr.VendorGuid, sizeof(hdr.VendorGuid));
		} else if (rdev_readat(rdev, &hdr, 0, sizeof(hdr)) != sizeof(hdr)) {
			return CB_EFI_ACCESS_ERROR;
		}
		if (hdr.StartId != VARIABLE_DATA) {
			ret = region_is_erased(rdev, &erased);
			if (ret != CB_SUCCESS)
				return ret;
			if (!erased)
				return CB_EFI_VS_CORRUPTED_INVALID;
			if (walker_stopped)
				*rdev = stopped_rdev;
			return walker_ret;
		}
		if (!variable_state_is_valid(hdr.State))
			return CB_EFI_VS_CORRUPTED_INVALID;

		printk(BIOS_SPEW, "Found variable with state %02x and ", hdr.State);
		print_guid(BIOS_SPEW, &hdr.VendorGuid);
		printk(BIOS_SPEW, "\n");

		if (hdr.NameSize > SIZE_MAX - header_size)
			return CB_EFI_VS_CORRUPTED_INVALID;
		var_size = header_size + hdr.NameSize;
		if (hdr.DataSize > SIZE_MAX - var_size)
			return CB_EFI_VS_CORRUPTED_INVALID;
		var_size += hdr.DataSize;
		if (var_size > SIZE_MAX - (HEADER_ALIGNMENT - 1))
			return CB_EFI_VS_CORRUPTED_INVALID;
		var_size = ALIGN_UP(var_size, HEADER_ALIGNMENT);
		if (!var_size || var_size > region_device_sz(rdev))
			return CB_EFI_VS_CORRUPTED_INVALID;
		if (hdr.NameSize < sizeof(terminator) || hdr.NameSize % sizeof(terminator))
			return CB_EFI_VS_CORRUPTED_INVALID;
		/* The header is written before its state, name and data. */
		if (hdr.State != UINT8_MAX && hdr.State != VAR_HEADER_VALID_ONLY) {
			if (rdev_readat(rdev, &terminator, header_size + hdr.NameSize -
					 sizeof(terminator), sizeof(terminator)) != sizeof(terminator))
				return CB_EFI_ACCESS_ERROR;
			if (terminator)
				return CB_EFI_VS_CORRUPTED_INVALID;

			if (!walker_done) {
				stop = false;
				ret = walker(rdev, &hdr, header_size, walker_arg, &stop);
				if (ret != CB_SUCCESS && !stop)
					return ret;
				if (stop) {
					stopped_rdev = *rdev;
					walker_stopped = true;
					walker_ret = ret;
					walker_done = hdr.State == VAR_ADDED;
				}
			}
		}
		if (var_size == region_device_sz(rdev)) {
			if (full)
				*full = true;
			if (walker_stopped)
				*rdev = stopped_rdev;
			return walker_ret;
		}
		if (rdev_chain(rdev, rdev, var_size, region_device_sz(rdev) - var_size))
			return CB_EFI_ACCESS_ERROR;
	} while (true);
}

static enum cb_err efi_fv_init(struct region_device *rdev, bool *auth_format)
{
	EFI_FIRMWARE_VOLUME_HEADER fv_hdr;
	enum cb_err ret;

	ret = validate_fv_header(rdev, &fv_hdr);
	if (ret != CB_SUCCESS) {
		printk(BIOS_WARNING, PREFIX "Failed to validate firmware header\n");

		return ret;
	}
	ret = validate_variable_store_header(&fv_hdr, rdev, auth_format);
	if (ret != CB_SUCCESS)
		printk(BIOS_WARNING, PREFIX "Failed to validate variable store header\n");

	return ret;
}

enum cb_err efi_fv_print_options(const struct region_device *rdev)
{
	enum cb_err ret;
	bool auth_format;
	struct region_device store_rdev = *rdev;

	ret = efi_fv_init(&store_rdev, &auth_format);
	if (ret != CB_SUCCESS)
		return ret;

	return walk_variables(&store_rdev, auth_format, print_var, NULL, NULL);
}

/*
 * efi_fv_get_option
 * - writes up to *size bytes into a buffer pointed to by *dest
 * - on success, updates *size to the actual number of bytes written
 * - rdev is the spi flash region to operate on
 * - the FVH and variable store header must have been initialized by a third party
 */
enum cb_err efi_fv_get_option(const struct region_device *rdev,
			      const EFI_GUID *guid,
			      const char *name,
			      void *dest,
			      uint32_t *size)
{
	struct efi_find_args args;
	bool auth_format;
	enum cb_err ret;
	struct region_device store_rdev = *rdev;

	ret = efi_fv_init(&store_rdev, &auth_format);
	if (ret != CB_SUCCESS)
		return ret;

	args.guid = guid;
	args.name = name;
	args.size = size;
	args.capacity = *size;
	args.data = dest;

	return walk_variables(&store_rdev, auth_format, find_and_copy, &args, NULL);
}

static enum cb_err variable_write_sizes(const struct region_device *rdev, const char *name,
					size_t header_size, size_t data_size,
					size_t *name_size)
{
	size_t total_size;

	if (strlen(name) > SIZE_MAX / sizeof(CHAR16) - 1)
		return CB_ERR_ARG;
	*name_size = (strlen(name) + 1) * sizeof(CHAR16);
	if (header_size > SIZE_MAX - *name_size)
		return CB_EFI_STORE_FULL;
	total_size = header_size + *name_size;
	if (data_size > SIZE_MAX - total_size ||
	    total_size + data_size > SIZE_MAX - (HEADER_ALIGNMENT - 1))
		return CB_EFI_STORE_FULL;
	total_size = ALIGN_UP(total_size + data_size, HEADER_ALIGNMENT);

	return total_size <= region_device_sz(rdev) ? CB_SUCCESS : CB_EFI_STORE_FULL;
}

static enum cb_err write_auth_hdr(struct region_device *rdev, const EFI_GUID *guid,
				  const char *name, void *data, size_t size)
{
	AUTHENTICATED_VARIABLE_HEADER auth_hdr;
	size_t name_size;
	enum cb_err ret;

	ret = variable_write_sizes(rdev, name, sizeof(auth_hdr), size, &name_size);
	if (ret != CB_SUCCESS)
		return ret;

	/* Sanity check. flash must be blank */
	if (rdev_readat(rdev, &auth_hdr, 0, sizeof(auth_hdr)) != sizeof(auth_hdr))
		return CB_EFI_ACCESS_ERROR;

	if (auth_hdr.StartId != UINT16_MAX ||
	    auth_hdr.State != UINT8_MAX ||
	    auth_hdr.DataSize != UINT32_MAX ||
	    auth_hdr.NameSize != UINT32_MAX ||
	    auth_hdr.Attributes != UINT32_MAX) {
		return CB_EFI_ACCESS_ERROR;
	}

	memset(&auth_hdr, 0xff, sizeof(auth_hdr));

	auth_hdr.StartId = VARIABLE_DATA;
	auth_hdr.Attributes = EFI_VARIABLE_NON_VOLATILE|
			      EFI_VARIABLE_BOOTSERVICE_ACCESS|
			      EFI_VARIABLE_RUNTIME_ACCESS;
	auth_hdr.NameSize = name_size;
	auth_hdr.DataSize = size;
	memcpy(&auth_hdr.VendorGuid, guid, sizeof(EFI_GUID));

	/* Write header with no State */
	if (rdev_writeat(rdev, &auth_hdr, 0, sizeof(auth_hdr)) != sizeof(auth_hdr))
		return CB_EFI_ACCESS_ERROR;

	/* Set header State to valid header */
	auth_hdr.State = VAR_HEADER_VALID_ONLY;
	if (rdev_writeat(rdev, &auth_hdr.State, offsetof(AUTHENTICATED_VARIABLE_HEADER, State),
			 sizeof(auth_hdr.State)) != sizeof(auth_hdr.State))
		return CB_EFI_ACCESS_ERROR;

	/* Write the name */
	ret = rdev_write_wchar(rdev, sizeof(auth_hdr), name);
	if (ret != CB_SUCCESS)
		return ret;

	/* Write the data */
	if (rdev_writeat(rdev, data, sizeof(auth_hdr) + name_size, size) != size)
		return CB_EFI_ACCESS_ERROR;

	/* Set header State to valid data */
	auth_hdr.State = VAR_ADDED;
	if (rdev_writeat(rdev, &auth_hdr.State, offsetof(AUTHENTICATED_VARIABLE_HEADER, State),
				sizeof(auth_hdr.State)) != sizeof(auth_hdr.State))
		return CB_EFI_ACCESS_ERROR;

	return CB_SUCCESS;
}

static enum cb_err write_hdr(struct region_device *rdev, const EFI_GUID *guid,
			     const char *name,
			     void *data,
			     size_t size)
{
	VARIABLE_HEADER hdr;
	size_t name_size;
	enum cb_err ret;

	ret = variable_write_sizes(rdev, name, sizeof(hdr), size, &name_size);
	if (ret != CB_SUCCESS)
		return ret;

	/* Sanity check. flash must be blank */
	if (rdev_readat(rdev, &hdr, 0, sizeof(hdr)) != sizeof(hdr))
		return CB_EFI_ACCESS_ERROR;

	if (hdr.StartId != UINT16_MAX ||
	    hdr.State != UINT8_MAX ||
	    hdr.DataSize != UINT32_MAX ||
	    hdr.NameSize != UINT32_MAX ||
	    hdr.Attributes != UINT32_MAX) {
		return CB_EFI_ACCESS_ERROR;
	}

	memset(&hdr, 0xff, sizeof(hdr));

	hdr.StartId = VARIABLE_DATA;
	hdr.Attributes = EFI_VARIABLE_NON_VOLATILE|
			 EFI_VARIABLE_BOOTSERVICE_ACCESS|
			 EFI_VARIABLE_RUNTIME_ACCESS;
	hdr.NameSize = name_size;
	hdr.DataSize = size;
	memcpy(&hdr.VendorGuid, guid, sizeof(EFI_GUID));

	/* Write header with no State */
	if (rdev_writeat(rdev, &hdr, 0, sizeof(hdr)) != sizeof(hdr))
		return CB_EFI_ACCESS_ERROR;

	/* Set header State to valid header */
	hdr.State = VAR_HEADER_VALID_ONLY;
	if (rdev_writeat(rdev, &hdr.State, offsetof(VARIABLE_HEADER, State),
			 sizeof(hdr.State)) != sizeof(hdr.State))
		return CB_EFI_ACCESS_ERROR;

	/* Write the name */
	ret = rdev_write_wchar(rdev, sizeof(hdr), name);
	if (ret != CB_SUCCESS)
		return ret;

	/* Write the data */
	if (rdev_writeat(rdev, data, sizeof(hdr) + name_size, size) != size)
		return CB_EFI_ACCESS_ERROR;

	/* Set header State to valid data */
	hdr.State = VAR_ADDED;
	if (rdev_writeat(rdev, &hdr.State, offsetof(VARIABLE_HEADER, State),
				sizeof(hdr.State)) != sizeof(hdr.State))
		return CB_EFI_ACCESS_ERROR;

	return CB_SUCCESS;
}

/*
 * efi_fv_set_option
 * - writes size bytes read from the buffer pointed to by *data
 * - rdev is the spi flash region to operate on
 * - the FVH and variable store header must have been initialized by a third party
 */
enum cb_err efi_fv_set_option(const struct region_device *rdev,
			      const EFI_GUID *guid,
			      const char *name,
			      void *data,
			      uint32_t size)
{
	struct region_device rdev_old;
	struct region_device store_rdev = *rdev;
	struct efi_find_compare_args args;
	bool found_existing;
	bool full;
	VARIABLE_HEADER hdr;
	bool auth_format;
	enum cb_err ret;

	ret = efi_fv_init(&store_rdev, &auth_format);
	if (ret != CB_SUCCESS)
		return ret;

	/* Find existing variable */
	args.guid = guid;
	args.name = name;
	args.size = size;
	args.match = false;
	args.data = data;

	ret = walk_variables(&store_rdev, auth_format, find_and_compare, &args, NULL);
	if (ret != CB_SUCCESS && ret != CB_EFI_OPTION_NOT_FOUND)
		return ret;
	found_existing = ret == CB_SUCCESS;

	if (found_existing) {
		printk(BIOS_DEBUG, "found existing variable %s, match = %d\n", name, args.match);

		if (args.match)
			return CB_SUCCESS;

		rdev_old = store_rdev;

		/* Mark as to be deleted */
		hdr.State = VAR_IN_DELETED_TRANSITION & VAR_ADDED;
		if (rdev_writeat(&store_rdev, &hdr.State, offsetof(VARIABLE_HEADER, State),
			sizeof(hdr.State)) != sizeof(hdr.State))
			return CB_EFI_ACCESS_ERROR;
	}

	/* Walk to end of variable store */
	ret = walk_variables(&store_rdev, auth_format, noop, NULL, &full);
	if (ret != CB_EFI_OPTION_NOT_FOUND)
		return ret;
	if (full)
		return CB_EFI_STORE_FULL;

	/* Now append new variable:
	 * 1. Write the header without State field.
	 * 2. Write the State field and set it to HEADER_VALID.
	 * 3. Write data
	 * 4. Write the State field and set it to VAR_ADDED
	 */

	if (auth_format)
		ret = write_auth_hdr(&store_rdev, guid, name, data, size);
	else
		ret = write_hdr(&store_rdev, guid, name, data, size);
	if (ret != CB_SUCCESS)
		return ret;

	if (found_existing) {
		/* Mark old variable as deleted */
		hdr.State = VAR_DELETED & VAR_IN_DELETED_TRANSITION & VAR_ADDED;
		if (rdev_writeat(&rdev_old, &hdr.State, offsetof(VARIABLE_HEADER, State),
			sizeof(hdr.State)) != sizeof(hdr.State))
			return CB_EFI_ACCESS_ERROR;
	}

	return CB_SUCCESS;
}
