/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tis.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <drivers/intel/ptt/ptt.h>
#include <drivers/tpm/tpm_ppi.h>
#include <security/tpm/tss.h>
#include <endian.h>
#include <smbios.h>
#include <string.h>

#include "tpm.h"
#include "chip.h"

static unsigned int tpm_is_open;

static const struct {
	uint16_t vid;
	uint16_t did;
	const char *device_name;
} dev_map[] = {
	{0x1ae0, 0x0028, "CR50"},
	{0xa13a, 0x8086, "Intel iTPM"}
};

static const char *tis_get_dev_name(struct tpm2_info *info)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dev_map); i++)
		if ((dev_map[i].vid == info->vendor_id) && (dev_map[i].did == info->device_id))
			return dev_map[i].device_name;
	return "Unknown";
}

int tis_open(void)
{
	if (tpm_is_open) {
		printk(BIOS_ERR, "%s called twice.\n", __func__);
		return -1;
	}

	if (CONFIG(HAVE_INTEL_PTT)) {
		if (!ptt_active()) {
			printk(BIOS_ERR, "%s: Intel PTT is not active.\n", __func__);
			return -1;
		}
		printk(BIOS_DEBUG, "%s: Intel PTT is active.\n", __func__);
	}

	return 0;
}

int tis_close(void)
{
	if (tpm_is_open) {
		/*
		 * Do we need to do something here, like waiting for a
		 * transaction to stop?
		 */
		tpm_is_open = 0;
	}

	return 0;
}

int tis_init(void)
{
	struct tpm2_info info;

	// Wake TPM up (if necessary)
	if (tpm2_init() != 0)
		return -1;

	tpm2_get_info(&info);

	printk(BIOS_INFO, "Initialized TPM device %s revision %d\n", tis_get_dev_name(&info),
	       info.revision);

	return 0;
}

int tis_sendrecv(const uint8_t *sendbuf, size_t sbuf_size, uint8_t *recvbuf, size_t *rbuf_len)
{
	int len = tpm2_process_command(sendbuf, sbuf_size, recvbuf, *rbuf_len);

	if (len == 0)
		return -1;

	*rbuf_len = len;

	return 0;
}

static void crb_tpm_fill_ssdt(const struct device *dev)
{
	const char *path = acpi_device_path(dev);
	if (!path) {
		path = "\\_SB_.TPM";
		printk(BIOS_DEBUG, "Using default TPM2 ACPI path: '%s'\n", path);
	}

	/* Device */
	acpigen_write_device(path);

	acpigen_write_name_string("_HID", "MSFT0101");
	acpigen_write_name_string("_CID", "MSFT0101");

	acpi_device_write_uid(dev);

	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, TPM_CRB_BASE_ADDRESS, 0x5000);

	acpigen_write_resourcetemplate_footer();

	if (!CONFIG(CHROMEOS) && CONFIG(TPM_PPI))
		tpm_ppi_acpi_fill_ssdt(dev);

	acpigen_pop_len(); /* Device */
}

static const char *crb_tpm_acpi_name(const struct device *dev)
{
	return "TPM";
}

#if CONFIG(GENERATE_SMBIOS_TABLES) && CONFIG(TPM2)
static int tpm_get_cap(uint32_t property, uint32_t *value)
{
	TPMS_CAPABILITY_DATA cap_data;
	int i;
	uint32_t status;

	if (!value)
		return -1;

	status = tlcl_get_capability(TPM_CAP_TPM_PROPERTIES, property, 1, &cap_data);

	if (status)
		return -1;

	for (i = 0 ; i < cap_data.data.tpmProperties.count; i++) {
		if (cap_data.data.tpmProperties.tpmProperty[i].property == property) {
			*value = cap_data.data.tpmProperties.tpmProperty[i].value;
			return 0;
		}
	}

	return -1;
}

static int smbios_write_type43_tpm(struct device *dev, int *handle, unsigned long *current)
{
	struct tpm2_info info;
	uint32_t tpm_manuf, tpm_family;
	uint32_t fw_ver1, fw_ver2;
	uint8_t major_spec_ver, minor_spec_ver;

	tpm2_get_info(&info);

	/* If any of these have invalid values, assume TPM not present or disabled */
	if (info.vendor_id == 0 || info.vendor_id == 0xFFFF ||
	    info.device_id == 0 || info.device_id == 0xFFFF) {
		printk(BIOS_DEBUG, "%s: Invalid Vendor ID/Device ID\n", __func__);
		return 0;
	}

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
				   fw_ver1, fw_ver2, tis_get_dev_name(&info),
				   SMBIOS_TPM_DEVICE_CHARACTERISTICS_NOT_SUPPORTED, 0);
}
#endif

static struct device_operations __maybe_unused crb_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = crb_tpm_acpi_name,
	.acpi_fill_ssdt = crb_tpm_fill_ssdt,
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES) && CONFIG(TPM2)
	.get_smbios_data	= smbios_write_type43_tpm,
#endif
};

static void enable_dev(struct device *dev)
{
#if !DEVTREE_EARLY
	dev->ops = &crb_ops;
#endif
}

struct chip_operations drivers_crb_ops = {
	CHIP_NAME("CRB TPM")
	.enable_dev = enable_dev
};
