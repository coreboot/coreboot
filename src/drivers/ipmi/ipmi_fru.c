/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <delay.h>
#include <stdlib.h>

#include "ipmi_if.h"
#include "ipmi_ops.h"

#define MAX_FRU_BUSY_RETRY 5
#define READ_FRU_DATA_RETRY_INTERVAL_MS 30 /* From IPMI spec v2.0 rev 1.1 */
#define OFFSET_LENGTH_MULTIPLIER 8 /* offsets/lengths are multiples of 8 */
#define NUM_DATA_BYTES(t) (t & 0x3f) /* Encoded in type/length byte */
#define FRU_END_OF_FIELDS 0xc1 /* type/length byte encoded to indicate no more info fields */

static enum cb_err ipmi_read_fru(const int port, struct ipmi_read_fru_data_req *req,
			uint8_t *fru_data)
{
	int ret;
	uint8_t total_size;
	uint16_t offset = 0;
	struct ipmi_read_fru_data_rsp rsp;
	int retry_count = 0;

	if (req == NULL || fru_data == NULL) {
		printk(BIOS_ERR, "%s failed, null pointer parameter\n",
			 __func__);
		return CB_ERR;
	}

	total_size = req->count;
	do {
		if (req->count > CONFIG_IPMI_FRU_SINGLE_RW_SZ)
			req->count = CONFIG_IPMI_FRU_SINGLE_RW_SZ;

		while (retry_count <= MAX_FRU_BUSY_RETRY) {
			ret = ipmi_message(port, IPMI_NETFN_STORAGE, 0x0,
					IPMI_READ_FRU_DATA, (const unsigned char *) req,
					sizeof(*req), (unsigned char *) &rsp, sizeof(rsp));
			if (rsp.resp.completion_code == 0x81) {
				/* Device is busy */
				if (retry_count == MAX_FRU_BUSY_RETRY) {
					printk(BIOS_ERR, "IPMI: %s command failed, "
						"device busy timeout\n", __func__);
					return CB_ERR;
				}
				printk(BIOS_ERR, "IPMI: FRU device is busy, "
					"retry count:%d\n", retry_count);
				retry_count++;
				mdelay(READ_FRU_DATA_RETRY_INTERVAL_MS);
			} else if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
				printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
					__func__, ret, rsp.resp.completion_code);
				return CB_ERR;
			}
			break;
		}
		retry_count = 0;
		memcpy(fru_data + offset, rsp.data, rsp.count);
		offset += rsp.count;
		total_size -= rsp.count;
		req->fru_offset += rsp.count;
		req->count = total_size;
	} while (total_size > 0);

	return CB_SUCCESS;
}

/* data: data to check, offset: offset to checksum. */
static uint8_t checksum(uint8_t *data, int offset)
{
	uint8_t c = 0;
	for (; offset > 0; offset--, data++)
		c += *data;
	return -c;
}

static uint8_t data2str(const uint8_t *frudata, char *stringdata, uint8_t length)
{
	uint8_t type;

	/* bit[7:6] is the type code. */
	type = ((frudata[0] & 0xc0) >> 6);
	if (type != ASCII_8BIT) {
		printk(BIOS_ERR, "%s typecode %d is unsupported, FRU string only "
			"supports 8-bit ASCII + Latin 1 for now.\n", __func__, type);
		return 0;
	}
	/* In the spec the string data is always the next byte to the type/length byte. */
	memcpy(stringdata, frudata + 1, length);
	stringdata[length] = '\0';
	return length;
}

/*
 * Read data string from data_ptr and store it to string, return the
 * length of the string or 0 when it's failed.
 */
static int read_data_string(const uint8_t *data_ptr, char **string)
{
	uint8_t length;

	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length == 0) {
		printk(BIOS_DEBUG, "%s:%d - failed due to length is zero\n", __func__,
			__LINE__);
		return 0;
	}

	*string = malloc(length + 1);
	if (!*string) {
		printk(BIOS_ERR, "%s failed to malloc %d bytes for string data.\n", __func__,
			length + 1);
		return 0;
	}
	if (!data2str((const uint8_t *)data_ptr, *string, length)) {
		printk(BIOS_ERR, "%s:%d - data2str failed\n", __func__, __LINE__);
		free(*string);
		return 0;
	}

	return length;
}

static enum cb_err read_fru_chassis_info_area(const int port, const uint8_t id,
				uint8_t offset, struct fru_chassis_info *info)
{
	uint8_t length;
	struct ipmi_read_fru_data_req req;
	uint8_t *data_ptr, *end, *custom_data_ptr;
	int ret = CB_SUCCESS;

	if (!offset)
		return CB_ERR;

	offset = offset * OFFSET_LENGTH_MULTIPLIER;
	req.fru_device_id = id;
	/* Read Chassis Info Area length first. */
	req.fru_offset = offset + 1;
	req.count = sizeof(length);
	if (ipmi_read_fru(port, &req, &length) != CB_SUCCESS || !length) {
		printk(BIOS_ERR, "%s failed, length: %d\n", __func__, length);
		return CB_ERR;
	}
	length = length * OFFSET_LENGTH_MULTIPLIER;
	data_ptr = (uint8_t *)malloc(length);
	if (!data_ptr) {
		printk(BIOS_ERR, "malloc %d bytes for chassis info failed\n", length);
		return CB_ERR;
	}
	end = data_ptr + length;
	/* Read Chassis Info Area data. */
	req.fru_offset = offset;
	req.count = length;
	if (ipmi_read_fru(port, &req, data_ptr) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s failed to read fru\n", __func__);
		ret = CB_ERR;
		goto out;
	}
	if (checksum(data_ptr, length)) {
		printk(BIOS_ERR, "Bad FRU chassis info checksum.\n");
		ret = CB_ERR;
		goto out;
	}
	/* Read chassis type. */
	info->chassis_type = data_ptr[CHASSIS_TYPE_OFFSET];

	printk(BIOS_DEBUG, "Read chassis part number string.\n");
	length = read_data_string(data_ptr + CHASSIS_TYPE_OFFSET + 1,
		&info->chassis_partnumber);

	printk(BIOS_DEBUG, "Read chassis serial number string.\n");
	data_ptr += CHASSIS_TYPE_OFFSET + 1 + length + 1;
	length = read_data_string(data_ptr, &info->serial_number);

	printk(BIOS_DEBUG, "Read custom chassis info fields.\n");
	data_ptr += length + 1;
	/* Check how many valid custom fields first. */
	info->custom_count = 0;
	custom_data_ptr = data_ptr;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0)
			info->custom_count++;
		data_ptr += length + 1;
	}
	if (!info->custom_count)
		goto out;

	info->chassis_custom = malloc(info->custom_count * sizeof(char *));
	if (!info->chassis_custom) {
		printk(BIOS_ERR, "%s failed to malloc %zu bytes for "
			"chassis custom data array.\n", __func__,
			info->custom_count * sizeof(char *));
		ret = CB_ERR;
		goto out;
	}

	/* Start reading custom chassis data. */
	data_ptr = custom_data_ptr;
	int count = 0;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0) {
			length = read_data_string(data_ptr, info->chassis_custom + count);
			count++;
		}
		data_ptr += length + 1;
	}

out:
	free(data_ptr);
	return ret;
}

static enum cb_err read_fru_board_info_area(const int port, const uint8_t id,
				uint8_t offset, struct fru_board_info *info)
{
	uint8_t length;
	struct ipmi_read_fru_data_req req;
	uint8_t *data_ptr, *end, *custom_data_ptr;
	int ret = CB_SUCCESS;

	if (!offset)
		return CB_ERR;

	offset = offset * OFFSET_LENGTH_MULTIPLIER;
	req.fru_device_id = id;
	/* Read Board Info Area length first. */
	req.fru_offset = offset + 1;
	req.count = sizeof(length);
	if (ipmi_read_fru(port, &req, &length) != CB_SUCCESS || !length) {
		printk(BIOS_ERR, "%s failed, length: %d\n", __func__, length);
		return CB_ERR;
	}
	length = length * OFFSET_LENGTH_MULTIPLIER;
	data_ptr = (uint8_t *)malloc(length);
	if (!data_ptr) {
		printk(BIOS_ERR, "malloc %d bytes for board info failed\n", length);
		return CB_ERR;
	}
	end = data_ptr + length;
	/* Read Board Info Area data. */
	req.fru_offset = offset;
	req.count = length;
	if (ipmi_read_fru(port, &req, data_ptr) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s failed to read fru\n", __func__);
		ret = CB_ERR;
		goto out;
	}
	if (checksum(data_ptr, length)) {
		printk(BIOS_ERR, "Bad FRU board info checksum.\n");
		ret = CB_ERR;
		goto out;
	}
	printk(BIOS_DEBUG, "Read board manufacturer string\n");
	length = read_data_string(data_ptr + BOARD_MAN_TYPE_LEN_OFFSET,
		&info->manufacturer);

	printk(BIOS_DEBUG, "Read board product name string.\n");
	data_ptr += BOARD_MAN_TYPE_LEN_OFFSET + length + 1;
	length = read_data_string(data_ptr, &info->product_name);

	printk(BIOS_DEBUG, "Read board serial number string.\n");
	data_ptr += length + 1;
	length = read_data_string(data_ptr, &info->serial_number);

	printk(BIOS_DEBUG, "Read board part number string.\n");
	data_ptr += length + 1;
	length = read_data_string(data_ptr, &info->part_number);

	printk(BIOS_DEBUG, "Read board FRU file ID string.\n");
	data_ptr += length + 1;
	length = read_data_string(data_ptr, &info->fru_file_id);

	/* Check how many valid custom fields first. */
	data_ptr += length + 1;
	info->custom_count = 0;
	custom_data_ptr = data_ptr;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0)
			info->custom_count++;
		data_ptr += length + 1;
	}
	if (!info->custom_count)
		goto out;

	info->board_custom = malloc(info->custom_count * sizeof(char *));
	if (!info->board_custom) {
		printk(BIOS_ERR, "%s failed to malloc %zu bytes for "
			"board custom data array.\n", __func__,
			info->custom_count * sizeof(char *));
		ret = CB_ERR;
		goto out;
	}

	/* Start reading custom board data. */
	data_ptr = custom_data_ptr;
	int count = 0;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0) {
			length = read_data_string(data_ptr, info->board_custom + count);
			count++;
		}
		data_ptr += length + 1;
	}

out:
	free(data_ptr);
	return ret;
}

static enum cb_err read_fru_product_info_area(const int port, const uint8_t id,
				uint8_t offset, struct fru_product_info *info)
{
	uint8_t length;
	struct ipmi_read_fru_data_req req;
	uint8_t *data_ptr, *end, *custom_data_ptr;
	int ret = CB_SUCCESS;

	if (!offset)
		return CB_ERR;

	offset = offset * OFFSET_LENGTH_MULTIPLIER;
	req.fru_device_id = id;
	/* Read Product Info Area length first. */
	req.fru_offset = offset + 1;
	req.count = sizeof(length);
	if (ipmi_read_fru(port, &req, &length) != CB_SUCCESS || !length) {
		printk(BIOS_ERR, "%s failed, length: %d\n", __func__, length);
		return CB_ERR;
	}
	length = length * OFFSET_LENGTH_MULTIPLIER;
	data_ptr = (uint8_t *)malloc(length);
	if (!data_ptr) {
		printk(BIOS_ERR, "malloc %d bytes for product info failed\n", length);
		return CB_ERR;
	}
	end = data_ptr + length;
	/* Read Product Info Area data. */
	req.fru_offset = offset;
	req.count = length;
	if (ipmi_read_fru(port, &req, data_ptr) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s failed to read fru\n", __func__);
		ret = CB_ERR;
		goto out;
	}
	if (checksum(data_ptr, length)) {
		printk(BIOS_ERR, "Bad FRU product info checksum.\n");
		ret = CB_ERR;
		goto out;
	}
	printk(BIOS_DEBUG, "Read product manufacturer string.\n");
	length = read_data_string(data_ptr + PRODUCT_MAN_TYPE_LEN_OFFSET,
		&info->manufacturer);

	data_ptr += PRODUCT_MAN_TYPE_LEN_OFFSET + length + 1;
	printk(BIOS_DEBUG, "Read product_name string.\n");
	length = read_data_string(data_ptr, &info->product_name);

	data_ptr += length + 1;
	printk(BIOS_DEBUG, "Read product part/model number.\n");
	length = read_data_string(data_ptr, &info->product_partnumber);

	data_ptr += length + 1;
	printk(BIOS_DEBUG, "Read product version string.\n");
	length = read_data_string(data_ptr, &info->product_version);

	data_ptr += length + 1;
	printk(BIOS_DEBUG, "Read serial number string.\n");
	length = read_data_string(data_ptr, &info->serial_number);

	data_ptr += length + 1;
	printk(BIOS_DEBUG, "Read asset tag string.\n");
	length = read_data_string(data_ptr, &info->asset_tag);

	printk(BIOS_DEBUG, "Read product FRU file ID string.\n");
	data_ptr += length + 1;
	length = read_data_string(data_ptr, &info->fru_file_id);

	/* Check how many valid custom fields first. */
	data_ptr += length + 1;
	info->custom_count = 0;
	custom_data_ptr = data_ptr;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0)
			info->custom_count++;
		data_ptr += length + 1;
	}
	if (!info->custom_count)
		goto out;

	info->product_custom = malloc(info->custom_count * sizeof(char *));
	if (!info->product_custom) {
		printk(BIOS_ERR, "%s failed to malloc %zu bytes for "
			"product custom data array.\n", __func__,
			info->custom_count * sizeof(char *));
		ret = CB_ERR;
		goto out;
	}

	/* Start reading custom product data. */
	data_ptr = custom_data_ptr;
	int count = 0;
	while ((data_ptr < end) && ((data_ptr[0] != FRU_END_OF_FIELDS))) {
		length = NUM_DATA_BYTES(data_ptr[0]);
		if (length > 0) {
			length = read_data_string(data_ptr, info->product_custom + count);
			count++;
		}
		data_ptr += length + 1;
	}

out:
	free(data_ptr);
	return ret;
}

void read_fru_areas(const int port, const uint8_t id, uint16_t offset,
			struct fru_info_str *fru_info_str)
{
	struct ipmi_read_fru_data_req req;
	struct ipmi_fru_common_hdr fru_common_hdr;

	/* Set all the char pointers to 0 first, to avoid mainboard
	 * overwriting SMBIOS string with any non-NULL char pointer
	 * by accident. */
	memset(fru_info_str, 0, sizeof(*fru_info_str));
	req.fru_device_id = id;
	req.fru_offset = offset;
	req.count = sizeof(fru_common_hdr);
	/* Read FRU common header first */
	if (ipmi_read_fru(port, &req, (uint8_t *)&fru_common_hdr) == CB_SUCCESS) {
		if (checksum((uint8_t *)&fru_common_hdr, sizeof(fru_common_hdr))) {
			printk(BIOS_ERR, "Bad FRU common header checksum.\n");
			return;
		}
		printk(BIOS_DEBUG, "FRU common header: format_version: %x\n"
			"product_area_offset: %x\n"
			"board_area_offset: %x\n"
			"chassis_area_offset: %x\n",
			fru_common_hdr.format_version,
			fru_common_hdr.product_area_offset,
			fru_common_hdr.board_area_offset,
			fru_common_hdr.chassis_area_offset);
	} else {
		printk(BIOS_ERR, "Read FRU common header failed\n");
		return;
	}

	read_fru_product_info_area(port, id, fru_common_hdr.product_area_offset,
		&fru_info_str->prod_info);
	read_fru_board_info_area(port, id, fru_common_hdr.board_area_offset,
		&fru_info_str->board_info);
	read_fru_chassis_info_area(port, id, fru_common_hdr.chassis_area_offset,
		&fru_info_str->chassis_info);
}

void read_fru_one_area(const int port, const uint8_t id, uint16_t offset,
			struct fru_info_str *fru_info_str, enum fru_area fru_area)
{
	struct ipmi_read_fru_data_req req;
	struct ipmi_fru_common_hdr fru_common_hdr;

	req.fru_device_id = id;
	req.fru_offset = offset;
	req.count = sizeof(fru_common_hdr);
	if (ipmi_read_fru(port, &req, (uint8_t *)&fru_common_hdr) == CB_SUCCESS) {
		if (checksum((uint8_t *)&fru_common_hdr, sizeof(fru_common_hdr))) {
			printk(BIOS_ERR, "Bad FRU common header checksum.\n");
			return;
		}
		printk(BIOS_DEBUG, "FRU common header: format_version: %x\n"
			"product_area_offset: %x\n"
			"board_area_offset: %x\n"
			"chassis_area_offset: %x\n",
			fru_common_hdr.format_version,
			fru_common_hdr.product_area_offset,
			fru_common_hdr.board_area_offset,
			fru_common_hdr.chassis_area_offset);
	} else {
		printk(BIOS_ERR, "Read FRU common header failed\n");
		return;
	}

	switch (fru_area) {
	case PRODUCT_INFO_AREA:
		memset(&fru_info_str->prod_info, 0, sizeof(fru_info_str->prod_info));
		read_fru_product_info_area(port, id, fru_common_hdr.product_area_offset,
			&fru_info_str->prod_info);
		break;
	case BOARD_INFO_AREA:
		memset(&fru_info_str->board_info, 0, sizeof(fru_info_str->board_info));
		read_fru_board_info_area(port, id, fru_common_hdr.board_area_offset,
			&fru_info_str->board_info);
		break;
	case CHASSIS_INFO_AREA:
		memset(&fru_info_str->chassis_info, 0, sizeof(fru_info_str->chassis_info));
		read_fru_chassis_info_area(port, id, fru_common_hdr.chassis_area_offset,
			&fru_info_str->chassis_info);
		break;
	default:
		printk(BIOS_ERR, "Invalid fru_area: %d\n", fru_area);
		break;
	}
}

void print_fru_areas(struct fru_info_str *fru_info_str)
{
	int count = 0;
	if (fru_info_str == NULL) {
		printk(BIOS_ERR, "FRU data is null pointer\n");
		return;
	}
	struct fru_product_info prod_info = fru_info_str->prod_info;
	struct fru_board_info board_info = fru_info_str->board_info;
	struct fru_chassis_info chassis_info = fru_info_str->chassis_info;

	printk(BIOS_DEBUG, "Printing Product Info Area...\n");
	if (prod_info.manufacturer != NULL)
		printk(BIOS_DEBUG, "manufacturer: %s\n", prod_info.manufacturer);
	if (prod_info.product_name != NULL)
		printk(BIOS_DEBUG, "product name: %s\n", prod_info.product_name);
	if (prod_info.product_partnumber != NULL)
		printk(BIOS_DEBUG, "product part number: %s\n", prod_info.product_partnumber);
	if (prod_info.product_version != NULL)
		printk(BIOS_DEBUG, "product version: %s\n", prod_info.product_version);
	if (prod_info.serial_number != NULL)
		printk(BIOS_DEBUG, "serial number: %s\n", prod_info.serial_number);
	if (prod_info.asset_tag != NULL)
		printk(BIOS_DEBUG, "asset tag: %s\n", prod_info.asset_tag);
	if (prod_info.fru_file_id != NULL)
		printk(BIOS_DEBUG, "FRU file ID: %s\n", prod_info.fru_file_id);

	for (count = 0; count < prod_info.custom_count; count++) {
		if (*(prod_info.product_custom + count) != NULL)
			printk(BIOS_DEBUG, "product custom data %i: %s\n", count,
				*(prod_info.product_custom + count));
	}

	printk(BIOS_DEBUG, "Printing Board Info Area...\n");
	if (board_info.manufacturer != NULL)
		printk(BIOS_DEBUG, "manufacturer: %s\n", board_info.manufacturer);
	if (board_info.product_name != NULL)
		printk(BIOS_DEBUG, "product name: %s\n", board_info.product_name);
	if (board_info.serial_number != NULL)
		printk(BIOS_DEBUG, "serial number: %s\n", board_info.serial_number);
	if (board_info.part_number != NULL)
		printk(BIOS_DEBUG, "part number: %s\n", board_info.part_number);
	if (board_info.fru_file_id != NULL)
		printk(BIOS_DEBUG, "FRU file ID: %s\n", board_info.fru_file_id);

	for (count = 0; count < board_info.custom_count; count++) {
		if (*(board_info.board_custom + count) != NULL)
			printk(BIOS_DEBUG, "board custom data %i: %s\n", count,
				*(board_info.board_custom + count));
	}

	printk(BIOS_DEBUG, "Printing Chassis Info Area...\n");
	printk(BIOS_DEBUG, "chassis type: 0x%x\n", chassis_info.chassis_type);
	if (chassis_info.chassis_partnumber != NULL)
		printk(BIOS_DEBUG, "part number: %s\n", chassis_info.chassis_partnumber);
	if (chassis_info.serial_number != NULL)
		printk(BIOS_DEBUG, "serial number: %s\n", chassis_info.serial_number);

	for (count = 0; count < chassis_info.custom_count; count++) {
		if (*(chassis_info.chassis_custom + count) != NULL)
			printk(BIOS_DEBUG, "chassis custom data %i: %s\n", count,
				*(chassis_info.chassis_custom + count));
	}
}
