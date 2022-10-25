/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <drivers/ipmi/ipmi_ops.h>
#include <delay.h>
#include <cpu/x86/mp.h>
#include <timer.h>
#include <stdio.h>
#include <string.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <smbios.h>
#include <types.h>

#include "ocp_dmi.h"

#define PPIN_STR_LEN 17

struct fru_info_str fru_strings = {0};
/* The spec defines only at most 2 sockets */
msr_t xeon_sp_ppin[2] = {0};
static bool remote_ppin_done = false;

/*
 * Update SMBIOS type 0 ec version.
 * For OCP platforms, BMC version is used to represent ec version.
 * Refer to IPMI v2.0 spec, minor revision is defined as BCD encoded,
 * format it accordingly.
 */
void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	uint8_t bmc_major_revision, bmc_minor_revision;

	ipmi_bmc_version(&bmc_major_revision, &bmc_minor_revision);
	*ec_major_revision = bmc_major_revision & 0x7f; /* bit[6:0] Major Firmware Revision */
	*ec_minor_revision = ((bmc_minor_revision / 16) * 10) + (bmc_minor_revision % 16);
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
	if (fru_strings.board_info.product_name != NULL)
		return (const char *)fru_strings.prod_info.product_name;
	else
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
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
		return CONFIG_MAINBOARD_VERSION;
}

/* Override SMBIOS type 1 uuid from the value from BMC. */
void smbios_system_set_uuid(u8 *uuid)
{
	ipmi_get_system_guid(CONFIG_BMC_KCS_BASE, uuid);
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

/* Override SMBIOS type 2 and 3 asset_tag data. */
const char *smbios_mainboard_asset_tag(void)
{
	if (fru_strings.prod_info.asset_tag != NULL)
		return (const char *)fru_strings.prod_info.asset_tag;
	else
		return "";
}

/* Override SMBIOS type 3 data. */
smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	/* SMBIOS System Enclosure or Chassis Types are values from 0 to 20h. */
	if (fru_strings.chassis_info.chassis_type <= 0x20)
		return fru_strings.chassis_info.chassis_type;
	else
		return SMBIOS_ENCLOSURE_RACK_MOUNT_CHASSIS;
}

const char *smbios_chassis_version(void)
{
	if (fru_strings.chassis_info.chassis_partnumber != NULL)
		return fru_strings.chassis_info.chassis_partnumber;
	else
		return "";
}

const char *smbios_chassis_serial_number(void)
{
	if (fru_strings.chassis_info.serial_number != NULL)
		return fru_strings.chassis_info.serial_number;
	else
		return "";
}

/* Override SMBIOS type 4 processor serial numbers from FRU Chassis custom data. */
const char *smbios_processor_serial_number(void)
{
	/* For now type 4 only creates for one CPU, so it can only write the serial number
	 * of CPU0.
	 */
	if (*fru_strings.chassis_info.chassis_custom != NULL)
		return *fru_strings.chassis_info.chassis_custom;
	else
		return "";
}

static void read_remote_ppin(void *data)
{
	*(msr_t *)data = read_msr_ppin();
	remote_ppin_done = true;
}

static void wait_for_remote_ppin(void)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, 500);
	while (!stopwatch_expired(&sw)) {
		if (remote_ppin_done)
			break;
		mdelay(100);
	}
	if (stopwatch_expired(&sw))
		printk(BIOS_ERR, "Wait for read_remote_ppin() timeout\n");
}

int smbios_add_oem_string(u8 *start, const char *str)
{
	int i = 1;
	char *p = (char *)start;

	if (*str == '\0')
		return 0;

	for (;;) {
		if (!*p) {
			strcpy(p, str);
			p += strlen(str);
			*p++ = '\0';
			*p++ = '\0';
			return i;
		}

		p += strlen(p)+1;
		i++;
	}
}

/* When the most significant 4 bits of PPIN hi/lo are 0, add '0' to the beginning */
static void ppin_string_fixup(int i, char *ppin)
{
	if ((xeon_sp_ppin[i].hi & 0xf0000000) == 0) {
		if ((xeon_sp_ppin[i].lo & 0xf0000000) == 0)
			snprintf(ppin, PPIN_STR_LEN, "0%x0%x", xeon_sp_ppin[i].hi,
				xeon_sp_ppin[i].lo);
		else
			snprintf(ppin, PPIN_STR_LEN, "0%x%x", xeon_sp_ppin[i].hi,
				xeon_sp_ppin[i].lo);
	} else if ((xeon_sp_ppin[i].lo & 0xf0000000) == 0) {
		snprintf(ppin, PPIN_STR_LEN, "%x0%x", xeon_sp_ppin[i].hi, xeon_sp_ppin[i].lo);
	} else {
		snprintf(ppin, PPIN_STR_LEN, "%x%x", xeon_sp_ppin[i].hi, xeon_sp_ppin[i].lo);
	}
}

/*
 * Override SMBIOS type 11 OEM string 1 to string 6, the rest are project dependent
 * and can be added in the mainboard code.
 */
void ocp_oem_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	char ppin[PPIN_STR_LEN];

	/* Add OEM string 1 to 4 */
	if (fru_strings.board_info.custom_count > 0 &&
			*fru_strings.board_info.board_custom != NULL)
		t->count = smbios_add_oem_string(t->eos, *fru_strings.board_info.board_custom);
	else
		t->count = smbios_add_oem_string(t->eos, TBF);

	if (fru_strings.prod_info.custom_count > 0 &&
			*fru_strings.prod_info.product_custom != NULL)
		t->count = smbios_add_oem_string(t->eos, *fru_strings.prod_info.product_custom);
	else
		t->count = smbios_add_oem_string(t->eos, TBF);

	if (fru_strings.prod_info.custom_count > 1 &&
			*(fru_strings.prod_info.product_custom + 1) != NULL)
		t->count = smbios_add_oem_string(t->eos,
			*(fru_strings.prod_info.product_custom + 1));
	else
		t->count = smbios_add_oem_string(t->eos, TBF);

	if (fru_strings.prod_info.custom_count > 2 &&
			*(fru_strings.prod_info.product_custom + 2) != NULL) {
		t->count = smbios_add_oem_string(t->eos,
			*(fru_strings.prod_info.product_custom + 2));
	} else {
		t->count = smbios_add_oem_string(t->eos, TBF);
	}

	/* Add CPU0 PPIN to OEM string 5 */
	xeon_sp_ppin[0] = read_msr_ppin();
	ppin_string_fixup(0, ppin);
	t->count = smbios_add_oem_string(t->eos, ppin);

	/* Add CPU1 PPIN to OEM string 6 */
	if (CONFIG_MAX_SOCKET == 2 && CONFIG(PARALLEL_MP_AP_WORK)) {
		/* Read the last CPU MSR */
		if (mp_run_on_aps(read_remote_ppin, (void *)&xeon_sp_ppin[1],
				get_platform_thread_count() - 1, 100 * USECS_PER_MSEC) !=
						CB_SUCCESS) {
			printk(BIOS_ERR, "Failed to read remote PPIN.\n");
			t->count = smbios_add_oem_string(t->eos, TBF);
		} else {
			/* Wait for read_remote_ppin() to finish because it's executed
			   in parallel */
			wait_for_remote_ppin();
			ppin_string_fixup(1, ppin);
			t->count = smbios_add_oem_string(t->eos, ppin);
		}
	} else {
		t->count = smbios_add_oem_string(t->eos, "0000");
	}
}
