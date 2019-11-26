/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <cf9_reset.h>
#include <smbios.h>
#include <string.h>
#include <drivers/vpd/vpd.h>
#include <console/console.h>
#include <drivers/ipmi/ipmi_ops.h>
#include "ipmi.h"
/* VPD variable for enabling/disabling FRB2 timer. */
#define FRB2_TIMER "FRB2_TIMER"
/* VPD variable for setting FRB2 timer countdown value. */
#define FRB2_COUNTDOWN "FRB2_COUNTDOWN"
#define VPD_LEN 10
/* Default countdown is 15 minutes. */
#define DEFAULT_COUNTDOWN 9000
#define FRU_DEVICE_ID 0
static struct fru_info_str fru_strings;
#define MAX_IMC 1
#define MAX_DIMM_SIZE_GB (32 * MiB)

static void init_frb2_wdt(void)
{

	char val[VPD_LEN];
	/* Enable FRB2 timer by default. */
	u8 enable = 1;
	uint16_t countdown;

	if (vpd_get_bool(FRB2_TIMER, VPD_RW, &enable)) {
		if (!enable) {
			printk(BIOS_DEBUG, "Disable FRB2 timer\n");
			ipmi_stop_bmc_wdt(BMC_KCS_BASE);
		}
	}
	if (enable) {
		if (vpd_gets(FRB2_COUNTDOWN, val, VPD_LEN, VPD_RW)) {
			countdown = (uint16_t)atol(val);
			printk(BIOS_DEBUG, "FRB2 timer countdown set to: %d\n",
				countdown);
		} else {
			printk(BIOS_DEBUG, "FRB2 timer use default value: %d\n",
				DEFAULT_COUNTDOWN);
			countdown = DEFAULT_COUNTDOWN;
		}
		ipmi_init_and_start_bmc_wdt(BMC_KCS_BASE, countdown,
			TIMEOUT_HARD_RESET);
	}
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int write_smbios_type16(struct device *dev, int *handle, unsigned long *current)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	u32 maximum_capacity;
	int len = sizeof(struct smbios_type16);

	printk(BIOS_INFO, "Creating SMBIOS tables type 16 (note, ECC information is hard-coded) ...");

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	/* The ECC setting can`t be confirmed in FSP, so hardcode it. */
	t->memory_error_correction = MEMORY_ARRAY_ECC_SINGLE_BIT;
	t->memory_error_information_handle = 0xFFFE;
	t->number_of_memory_devices = CONFIG_DIMM_MAX / MAX_IMC;

	maximum_capacity = (u32)(CONFIG_DIMM_MAX * MAX_DIMM_SIZE_GB);
	if (maximum_capacity >= 0x80000000) {
		t->maximum_capacity = 0x80000000;
		t->extended_maximum_capacity = maximum_capacity << 10;
	} else {
		t->maximum_capacity = (u32)maximum_capacity;
		t->extended_maximum_capacity = 0;
	}

	*current += len;
	t->handle = *handle;
	*handle += 1;
	t->length = len - 2;

	printk(BIOS_INFO, "done\n");

	return len;
}
#endif

/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(struct device *dev)
{
	ipmi_oem_rsp_t rsp;

	init_frb2_wdt();
	if (is_ipmi_clear_cmos_set(&rsp)) {
		/* TODO: Should also try to restore CMOS to cmos.default
		 * if USE_OPTION_TABLE is set */
		cmos_init(1);
		clear_ipmi_flags(&rsp);
		system_reset();
	}

	read_fru_areas(BMC_KCS_BASE, FRU_DEVICE_ID, 0, &fru_strings);

#if (CONFIG(GENERATE_SMBIOS_TABLES))
	dev->ops->get_smbios_data = write_smbios_type16;
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{

	char locator[64] = {0};

	snprintf(locator, sizeof(locator), "DIMM_%c%u", 'A' + dimm->channel_num,
		 dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "_Node0_Channel%d_Dimm%d", dimm->channel_num,
		 dimm->dimm_num);
	t->bank_locator = smbios_add_string(t->eos, locator);
}

/* Override SMBIOS uuid from the value from BMC. */
void smbios_system_set_uuid(u8 *uuid)
{
	ipmi_get_system_guid(BMC_KCS_BASE, uuid);
}
/* Override SMBIOS type 1 data. */
const char *smbios_system_manufacturer(void)
{
	if (fru_strings.prod_info.manufacturer != NULL)
		return (const char *)fru_strings.prod_info.manufacturer;
	else
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

const char *smbios_system_product_name(void)
{
	char *prod_name_partnumber;
	/* Concatenates IPMI FRU Product Info product name
	 * and product part number. */
	if (fru_strings.prod_info.product_name != NULL) {
		if (fru_strings.prod_info.product_partnumber != NULL) {
			/* Append a space after product_name. */
			prod_name_partnumber = strconcat(fru_strings.prod_info.product_name,
				" ");
			if (!prod_name_partnumber)
				return (const char *)fru_strings.prod_info.product_name;

			prod_name_partnumber = strconcat(prod_name_partnumber,
				fru_strings.prod_info.product_partnumber);
			if (!prod_name_partnumber)
				return (const char *)fru_strings.prod_info.product_name;

			return (const char *)prod_name_partnumber;
		} else {
			return (const char *)fru_strings.prod_info.product_name;
		}
	} else {
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
	}
}

const char *smbios_system_serial_number(void)
{
	if (fru_strings.prod_info.serial_number != NULL)
		return (const char *)fru_strings.prod_info.serial_number;
	else
		return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

const char *smbios_system_version(void)
{
	if (fru_strings.prod_info.product_version != NULL)
		return (const char *)fru_strings.prod_info.product_version;
	else
		return CONFIG_MAINBOARD_SERIAL_NUMBER;
}
/* Override SMBIOS type 2 data. */
const char *smbios_mainboard_version(void)
{
	if (fru_strings.board_info.part_number != NULL)
		return (const char *)fru_strings.board_info.part_number;
	else
		return CONFIG_MAINBOARD_VERSION;
}

const char *smbios_mainboard_manufacturer(void)
{
	if (fru_strings.board_info.manufacturer != NULL)
		return (const char *)fru_strings.board_info.manufacturer;
	else
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

const char *smbios_mainboard_product_name(void)
{
	if (fru_strings.board_info.product_name != NULL)
		return (const char *)fru_strings.board_info.product_name;
	else
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

const char *smbios_mainboard_serial_number(void)
{
	if (fru_strings.board_info.serial_number != NULL)
		return (const char *)fru_strings.board_info.serial_number;
	else
		return CONFIG_MAINBOARD_SERIAL_NUMBER;
}
