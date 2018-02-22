/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 - 2018 Online SAS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arch/early_variables.h>

#include <console/console.h>
#include <console/uart.h>

#include "bmcinfo.h"

typedef struct {
	u32 magic0; // "BMCI"
	u32 magic1; // "nfo0"
	u16 length;
	u16 chksum;
	u8 uuid[16];
	u8 bmcSerial[9]; // as null terminated string
	u8 slot;
	u8 corebootVerbosityLevel;
	u8 relaxSecurity;
	u32 baudrate;
	u8 bootOption;
	u8 hwRev; // Note: Initial implementation ended here
	u8 disableNic1;
	u8 endMarker; // Insert new fields before
} biosBmcInfo_t;

#define BIOSBMCINFO_MAGIC0 0x49434d42
#define BIOSBMCINFO_MAGIC1 0x306f666e


#define BMC_INFO ((biosBmcInfo_t *)CONFIG_BMC_INFO_LOC)

enum biosBmcInfoValidFlag_e {
	BMCINFO_UNTESTED = 0,
	BMCINFO_INVALID,
	BMCINFO_INVALID_WARNED,
	BMCINFO_VALID_NEED_WARN,
	BMCINFO_VALID,
};

static enum biosBmcInfoValidFlag_e CAR_GLOBAL biosBmcInfoValidFlag;

static bool bmcinfo_is_valid(size_t minsize)
{
	const biosBmcInfo_t *bmc_info = BMC_INFO;
	int flag = car_get_var(biosBmcInfoValidFlag);
	if (flag == BMCINFO_UNTESTED) {
		flag = BMCINFO_INVALID;
		if ((bmc_info->magic0 == BIOSBMCINFO_MAGIC0)
		 && (bmc_info->magic1 == BIOSBMCINFO_MAGIC1)
		 && (bmc_info->length >= offsetof(biosBmcInfo_t, hwRev))
		 && (bmc_info->length <= 0x1000)) {
			u16 chksum = 0 - (bmc_info->chksum & 0xff)
				- (bmc_info->chksum >> 8);
			int i;
			for (i = 0; i < bmc_info->length ; i++)
				chksum += ((u8 *)bmc_info)[i];
			if (bmc_info->chksum == chksum) {
				if (bmc_info->length >= offsetof(biosBmcInfo_t,
							endMarker))
					flag = BMCINFO_VALID;
				else
					flag = BMCINFO_VALID_NEED_WARN;
			}
		}
		car_set_var(biosBmcInfoValidFlag, flag);
	}
#if !defined(__PRE_RAM__)
	if (flag == BMCINFO_INVALID) {
		int length = offsetof(biosBmcInfo_t, endMarker);
		printk(BIOS_CRIT, "WARNING bmcInfo struct"
				"is not available please update your BMC.\n");
		flag = BMCINFO_INVALID_WARNED;
		car_set_var(biosBmcInfoValidFlag, flag);
		printk(BIOS_CRIT, "bmcInfo magic = \"%x-%x\"\n",
				bmc_info->magic0, bmc_info->magic1);
		printk(BIOS_CRIT, "bmcInfo length = %d expected = %d\"\n",
				bmc_info->length, length);
		u16 chksum = 0 - (bmc_info->chksum & 0xff)
			- (bmc_info->chksum >> 8);
		int i;
		for (i = 0; i < bmc_info->length; i++)
			chksum += ((u8 *)bmc_info)[i];
		printk(BIOS_CRIT, "bmcInfo chksum = 0x%x expected = 0x%x\"\n",
				bmc_info->chksum, chksum);
	}
	if (flag == BMCINFO_VALID_NEED_WARN) {
		printk(BIOS_CRIT, "WARNING bmcInfo struct"
				" is incomplete please update your BMC.\n");

		flag = BMCINFO_VALID;
		car_set_var(biosBmcInfoValidFlag, flag);
	}
#endif
	if (flag < BMCINFO_VALID_NEED_WARN)
		return false;
	return (bmc_info->length >= minsize);
}

#define IS_BMC_INFO_FIELD_VALID(field) \
	(bmcinfo_is_valid(offsetof(biosBmcInfo_t, field) \
		+ sizeof(((biosBmcInfo_t *)0)->field)))

char *bmcinfo_serial(void)
{
	if (IS_BMC_INFO_FIELD_VALID(bmcSerial))
		return (char *) BMC_INFO->bmcSerial;
	return NULL;
}

u8 *bmcinfo_uuid(void)
{
	if (IS_BMC_INFO_FIELD_VALID(uuid))
		return BMC_INFO->uuid;
	return NULL;
}


int bmcinfo_slot(void)
{
	if (IS_BMC_INFO_FIELD_VALID(slot))
		return BMC_INFO->slot;
	return -1;
}

int bmcinfo_hwrev(void)
{
	if (IS_BMC_INFO_FIELD_VALID(hwRev))
		return BMC_INFO->hwRev;
	return -1;
}

u32 bmcinfo_baudrate(void)
{
	if (IS_BMC_INFO_FIELD_VALID(baudrate))
		return BMC_INFO->baudrate;
	return 0;
}

int bmcinfo_coreboot_verbosity_level(void)
{
	if (IS_BMC_INFO_FIELD_VALID(corebootVerbosityLevel))
		return BMC_INFO->corebootVerbosityLevel & 0xf;
	return BIOS_CRIT;
}

int bmcinfo_fsp_verbosity_level(void)
{
	if (IS_BMC_INFO_FIELD_VALID(corebootVerbosityLevel))
		return BMC_INFO->corebootVerbosityLevel >> 4;
	return 0;
}

int bmcinfo_relax_security(void)
{
	if (IS_BMC_INFO_FIELD_VALID(relaxSecurity))
		return BMC_INFO->relaxSecurity;
	return 0;
}

int bmcinfo_boot_option(void)
{
	if (IS_BMC_INFO_FIELD_VALID(bootOption))
		return BMC_INFO->bootOption;
	return 0;
}

int bmcinfo_disable_nic1(void)
{
	if (IS_BMC_INFO_FIELD_VALID(disableNic1))
		return BMC_INFO->disableNic1;
	return 0;
}

/* Add override functions below */

/* Override default uart baudrate */
unsigned int get_uart_baudrate(void)
{
	int baudrate = bmcinfo_baudrate();
	if (baudrate)
		return baudrate;
	return 115200;
}

#if __CONSOLE_ENABLE__
/* Override default console loglevel */
int get_console_loglevel(void)
{
	return bmcinfo_coreboot_verbosity_level();
}
#endif
