/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <security/tpm/tis.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <security/tpm/tss.h>
#include <endian.h>
#include <smbios.h>
#include <string.h>

#include "tpm.h"
#include "chip.h"

static const char *tis_get_dev_name(void)
{
	return "fTPM";
}

static tpm_result_t crb_tpm_sendrecv(const uint8_t *sendbuf, size_t sbuf_size, uint8_t *recvbuf,
				     size_t *rbuf_len)
{
	int len = crb_tpm_process_command(sendbuf, sbuf_size, recvbuf, *rbuf_len);

	if (len <= 0)
		return TPM_CB_FAIL;

	*rbuf_len = len;

	return TPM_SUCCESS;
}

tis_sendrecv_fn crb_tis_probe(enum tpm_family *family)
{
	/* Wake TPM up (if necessary) */
	if (crb_tpm_init())
		return NULL;

	/* CRB interface exists only in TPM2 */
	if (family != NULL)
		*family = TPM_2;

	printk(BIOS_INFO, "Initialized TPM device %s\n", tis_get_dev_name());

	return &crb_tpm_sendrecv;
}

static void tpm_start_func0_cb(void *arg)
{
	/* Function 1. */
	acpigen_write_return_singleton_buffer(0x3);
}
static void tpm_start_func1_cb(void *arg)
{
	/* Just return success. fTPM is already operational. */
	acpigen_write_return_byte(0);
}

static void (*tpm_start_callbacks[])(void *) = {
	tpm_start_func0_cb,
	tpm_start_func1_cb,
};

#define TPM_START_UUID   "6bbf6cab-5463-4714-b7cd-f0203c0368d4"

static void crb_tpm_fill_ssdt(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_MMIO);
	assert(dev->path.mmio.addr == crb_tpm_base_address());

	/* Device */
	acpigen_write_scope("\\_SB");
	acpigen_write_device(acpi_device_name(dev));

	acpigen_write_name_string("_HID", "MSFT0101");
	acpigen_write_name_string("_CID", "MSFT0101");

	acpi_device_write_uid(dev);

	if (crb_tpm_is_active())
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);
	else
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_OFF);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, read32(CRB_REG(CRB_REG_CMD_ADDR)),
				 read32(CRB_REG(CRB_REG_CMD_SIZE)));
	acpigen_write_mem32fixed(1, read32(CRB_REG(CRB_REG_RESP_ADDR)),
				 read32(CRB_REG(CRB_REG_RESP_SIZE)));
	acpigen_write_resourcetemplate_footer();

	/*
	 * _DSM method
	 */
	struct dsm_uuid ids[] = {
		DSM_UUID(TPM_START_UUID, tpm_start_callbacks,
			ARRAY_SIZE(tpm_start_callbacks), NULL),
	};

	acpigen_write_dsm_uuid_arr(ids, ARRAY_SIZE(ids));

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static const char *crb_tpm_acpi_name(const struct device *dev)
{
	return "TPM2";
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static tpm_result_t tpm_get_cap(uint32_t property, uint32_t *value)
{
	TPMS_CAPABILITY_DATA cap_data;
	int i;
	tpm_result_t rc;

	if (!value)
		return TPM_CB_INVALID_ARG;

	rc = tlcl2_get_capability(TPM_CAP_TPM_PROPERTIES, property, 1, &cap_data);

	if (rc)
		return rc;

	for (i = 0; i < cap_data.data.tpmProperties.count; i++) {
		if (cap_data.data.tpmProperties.tpmProperty[i].property == property) {
			*value = cap_data.data.tpmProperties.tpmProperty[i].value;
			return TPM_SUCCESS;
		}
	}

	return TPM_CB_FAIL;
}

static int smbios_write_type43_tpm(struct device *dev, int *handle, unsigned long *current)
{
	uint32_t tpm_manuf, tpm_family;
	uint32_t fw_ver1, fw_ver2;
	uint8_t major_spec_ver, minor_spec_ver;

	/* Vendor ID is the value returned by TPM2_GetCapabiltiy TPM_PT_MANUFACTURER */
	if (tpm_get_cap(TPM_PT_MANUFACTURER, &tpm_manuf)) {
		printk(BIOS_DEBUG, "TPM2_GetCap TPM_PT_MANUFACTURER failed\n");
		return 0;
	}

	tpm_manuf = be32toh(tpm_manuf);

	if (tpm_get_cap(TPM_PT_FIRMWARE_VERSION_1, &fw_ver1)) {
		printk(BIOS_DEBUG, "TPM2_GetCap TPM_PT_FIRMWARE_VERSION_1 failed\n");
		return 0;
	}

	if (tpm_get_cap(TPM_PT_FIRMWARE_VERSION_2, &fw_ver2)) {
		printk(BIOS_DEBUG, "TPM2_GetCap TPM_PT_FIRMWARE_VERSION_2 failed\n");
		return 0;
	}

	if (tpm_get_cap(TPM_PT_FAMILY_INDICATOR, &tpm_family)) {
		printk(BIOS_DEBUG, "TPM2_GetCap TPM_PT_FAMILY_INDICATOR failed\n");
		return 0;
	}

	tpm_family = be32toh(tpm_family);

	if (!strncmp((char *)&tpm_family, "2.0", 4)) {
		major_spec_ver = 2;
		minor_spec_ver = 0;
	} else {
		printk(BIOS_ERR, "%s: Invalid TPM family\n", __func__);
		return 0;
	}

	return smbios_write_type43(current, handle, tpm_manuf, major_spec_ver, minor_spec_ver,
				   fw_ver1, fw_ver2, tis_get_dev_name(),
				   SMBIOS_TPM_DEVICE_CHARACTERISTICS_NOT_SUPPORTED, 0);
}
#endif

static struct device_operations __maybe_unused amd_crb_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = crb_tpm_acpi_name,
	.acpi_fill_ssdt = crb_tpm_fill_ssdt,
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data	= smbios_write_type43_tpm,
#endif
};

static void enable_dev(struct device *dev)
{
#if !DEVTREE_EARLY
	dev->ops = &amd_crb_ops;
#endif
}

struct chip_operations drivers_amd_ftpm_ops = {
	.name = "AMD CRB fTPM",
	.enable_dev = enable_dev
};
