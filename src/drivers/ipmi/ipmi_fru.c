/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include <delay.h>
#include <stdlib.h>

#include "ipmi_ops.h"

#define MAX_FRU_BUSY_RETRY 5
#define READ_FRU_DATA_RETRY_INTERVAL_MS 30 /* From IPMI spec v2.0 rev 1.1 */
#define OFFSET_LENGTH_MULTIPLIER 8 /* offsets/lengths are multiples of 8 */
#define NUM_DATA_BYTES(t) (t & 0x3f) /* Encoded in type/length byte */

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
			ret = ipmi_kcs_message(port, IPMI_NETFN_STORAGE, 0x0,
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

static void read_fru_board_info_area(const int port, const uint8_t id,
				uint8_t offset, struct fru_board_info *info)
{
	uint8_t length;
	struct ipmi_read_fru_data_req req;
	uint8_t *data_ptr;

	offset = offset * OFFSET_LENGTH_MULTIPLIER;
	if (!offset)
		return;
	req.fru_device_id = id;
	/* Read Board Info Area length first. */
	req.fru_offset = offset + 1;
	req.count = sizeof(length);
	if (ipmi_read_fru(port, &req, &length) != CB_SUCCESS || !length) {
		printk(BIOS_ERR, "%s failed, length: %d\n", __func__, length);
		return;
	}
	length = length * OFFSET_LENGTH_MULTIPLIER;
	data_ptr = (uint8_t *)malloc(length);
	if (!data_ptr) {
		printk(BIOS_ERR, "malloc %d bytes for board info failed\n", length);
		return;
	}

	/* Read Board Info Area data. */
	req.fru_offset = offset;
	req.count = length;
	if (ipmi_read_fru(port, &req, data_ptr) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s failed to read fru\n", __func__);
		goto out;
	}
	if (checksum(data_ptr, length)) {
		printk(BIOS_ERR, "Bad FRU board info checksum.\n");
		goto out;
	}
	/* Read manufacturer string, bit[5:0] is the string length. */
	length = NUM_DATA_BYTES(data_ptr[BOARD_MAN_TYPE_LEN_OFFSET]);
	data_ptr += BOARD_MAN_TYPE_LEN_OFFSET;
	if (length > 0) {
		info->manufacturer = malloc(length + 1);
		if (!info->manufacturer) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"manufacturer.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->manufacturer, length))
			free(info->manufacturer);
	}

	/* Read product name string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->product_name = malloc(length+1);
		if (!info->product_name) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"product_name.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->product_name, length))
			free(info->product_name);
	}

	/* Read serial number string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->serial_number = malloc(length + 1);
		if (!info->serial_number) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"serial_number.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->serial_number, length))
			free(info->serial_number);
	}

	/* Read part number string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->part_number = malloc(length + 1);
		if (!info->part_number) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"part_number.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->part_number, length))
			free(info->part_number);
	}

out:
	free(data_ptr);
}

static void read_fru_product_info_area(const int port, const uint8_t id,
				uint8_t offset, struct fru_product_info *info)
{
	uint8_t length;
	struct ipmi_read_fru_data_req req;
	uint8_t *data_ptr;

	offset = offset * OFFSET_LENGTH_MULTIPLIER;
	if (!offset)
		return;

	req.fru_device_id = id;
	/* Read Product Info Area length first. */
	req.fru_offset = offset + 1;
	req.count = sizeof(length);
	if (ipmi_read_fru(port, &req, &length) != CB_SUCCESS || !length) {
		printk(BIOS_ERR, "%s failed, length: %d\n", __func__, length);
		return;
	}
	length = length * OFFSET_LENGTH_MULTIPLIER;
	data_ptr = (uint8_t *)malloc(length);
	if (!data_ptr) {
		printk(BIOS_ERR, "malloc %d bytes for product info failed\n", length);
		return;
	}

	/* Read Product Info Area data. */
	req.fru_offset = offset;
	req.count = length;
	if (ipmi_read_fru(port, &req, data_ptr) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s failed to read fru\n", __func__);
		goto out;
	}
	if (checksum(data_ptr, length)) {
		printk(BIOS_ERR, "Bad FRU product info checksum.\n");
		goto out;
	}
	/* Read manufacturer string, bit[5:0] is the string length. */
	length = NUM_DATA_BYTES(data_ptr[PRODUCT_MAN_TYPE_LEN_OFFSET]);
	data_ptr += PRODUCT_MAN_TYPE_LEN_OFFSET;
	if (length > 0) {
		info->manufacturer = malloc(length + 1);
		if (!info->manufacturer) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"manufacturer.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->manufacturer, length))
			free(info->manufacturer);
	}

	/* Read product_name string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->product_name = malloc(length + 1);
		if (!info->product_name) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"product_name.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->product_name, length))
			free(info->product_name);
	}

	/* Read product part/model number. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->product_partnumber = malloc(length + 1);
		if (!info->product_partnumber) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"product_partnumber.\n",	__func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->product_partnumber, length))
			free(info->product_partnumber);
	}

	/* Read product version string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->product_version = malloc(length + 1);
		if (!info->product_version) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"product_version.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->product_version, length))
			free(info->product_version);
	}

	/* Read serial number string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->serial_number = malloc(length + 1);
		if (!info->serial_number) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"serial_number.\n", __func__, length + 1);
			goto out;
		}
		if (!data2str((const uint8_t *)data_ptr, info->serial_number, length))
			free(info->serial_number);
	}

	/* Read asset tag string. */
	data_ptr += length + 1;
	length = NUM_DATA_BYTES(data_ptr[0]);
	if (length > 0) {
		info->asset_tag = malloc(length + 1);
		if (!info->asset_tag) {
			printk(BIOS_ERR, "%s failed to malloc %d bytes for "
				"asset_tag.\n", __func__, length + 1);
				goto out;
			}
		if (!data2str((const uint8_t *)data_ptr, info->asset_tag, length))
			free(info->asset_tag);
	}

out:
	free(data_ptr);
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
	/* ToDo: Add read_fru_chassis_info_area(). */
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
	/* ToDo: Add case for CHASSIS_INFO_AREA. */
	default:
		printk(BIOS_ERR, "Invalid fru_area: %d\n", fru_area);
		break;
	}
}
