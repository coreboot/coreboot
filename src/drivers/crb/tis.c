/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tis.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <drivers/intel/ptt/ptt.h>
#include <drivers/tpm/tpm_ppi.h>

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

static struct device_operations __maybe_unused crb_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = crb_tpm_acpi_name,
	.acpi_fill_ssdt = crb_tpm_fill_ssdt,
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
