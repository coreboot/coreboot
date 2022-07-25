/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Place in devicetree.cb:
 *
 * chip drivers/ipmi
 *   device pnp ca2.0 on end         # IPMI KCS
 * end
 */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/gpio.h>
#include <device/pnp.h>
#if CONFIG(HAVE_ACPI_TABLES)
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES)
#include <smbios.h>
#endif
#include <version.h>
#include <delay.h>
#include <timer.h>
#include "ipmi_if.h"
#include "ipmi_supermicro_oem.h"
#include "chip.h"

#define IPMI_GET_DID_RETRY_MS 10000

/* 4 bit encoding */
static u8 ipmi_revision_major = 0x1;
static u8 ipmi_revision_minor = 0x0;

static u8 bmc_revision_major = 0x0;
static u8 bmc_revision_minor = 0x0;

static struct boot_state_callback bscb_post_complete;

static void bmc_set_post_complete_gpio_callback(void *arg)
{
	struct drivers_ipmi_config *conf = arg;
	const struct gpio_operations *gpio_ops;

	if (!conf || !conf->post_complete_gpio)
		return;

	gpio_ops = dev_get_gpio_ops(conf->gpio_dev);
	if (!gpio_ops) {
		printk(BIOS_WARNING, "IPMI: specified gpio device is missing gpio ops!\n");
		return;
	}

	/* Set POST Complete pin. The `invert` field controls the polarity. */
	gpio_ops->output(conf->post_complete_gpio, conf->post_complete_invert ^ 1);

	printk(BIOS_DEBUG, "BMC: POST complete gpio set\n");
}

static void ipmi_kcs_init(struct device *dev)
{
	struct ipmi_devid_rsp rsp;
	uint32_t man_id = 0, prod_id = 0;
	struct drivers_ipmi_config *conf = dev->chip_info;
	const struct gpio_operations *gpio_ops;

	if (!conf) {
		printk(BIOS_WARNING, "IPMI: chip_info is missing! Skip init.\n");
		return;
	}

	if (conf->bmc_jumper_gpio) {
		gpio_ops = dev_get_gpio_ops(conf->gpio_dev);
		if (!gpio_ops) {
			printk(BIOS_WARNING, "IPMI: gpio device is missing gpio ops!\n");
		} else {
			/* Get jumper value and set device state accordingly */
			dev->enabled = gpio_ops->get(conf->bmc_jumper_gpio);
			if (!dev->enabled)
				printk(BIOS_INFO, "IPMI: Disabled by jumper\n");
		}
	}

	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "IPMI: PNP KCS 0x%x\n", dev->path.pnp.port);

	/* Set up boot state callback for POST_COMPLETE# */
	if (conf->post_complete_gpio) {
		bscb_post_complete.callback = bmc_set_post_complete_gpio_callback;
		bscb_post_complete.arg = conf;
		boot_state_sched_on_entry(&bscb_post_complete, BS_PAYLOAD_BOOT);
	}

	/* Get IPMI version for ACPI and SMBIOS */
	if (conf->wait_for_bmc && conf->bmc_boot_timeout) {
		struct stopwatch sw;
		stopwatch_init_msecs_expire(&sw, conf->bmc_boot_timeout * 1000);
		printk(BIOS_INFO, "IPMI: Waiting for BMC...\n");

		while (!stopwatch_expired(&sw)) {
			if (inb(dev->path.pnp.port) != 0xff)
				break;
			mdelay(100);
		}
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "IPMI: Waiting for BMC timed out\n");
			/* Don't write tables if communication failed */
			dev->enabled = 0;
			return;
		}
	}

	if (ipmi_process_self_test_result(dev)) {
		/* Don't write tables if communication failed */
		dev->enabled = 0;
		return;
	}

	if (!wait_ms(IPMI_GET_DID_RETRY_MS, !ipmi_get_device_id(dev, &rsp))) {
		printk(BIOS_ERR, "IPMI: BMC does not respond to get device id even "
			"after %d ms.\n", IPMI_GET_DID_RETRY_MS);
		dev->enabled = 0;
		return;
	}

	/* Queried the IPMI revision from BMC */
	ipmi_revision_minor = IPMI_IPMI_VERSION_MINOR(rsp.ipmi_version);
	ipmi_revision_major = IPMI_IPMI_VERSION_MAJOR(rsp.ipmi_version);

	bmc_revision_major = rsp.fw_rev1;
	bmc_revision_minor = rsp.fw_rev2;

	memcpy(&man_id, rsp.manufacturer_id, sizeof(rsp.manufacturer_id));

	memcpy(&prod_id, rsp.product_id, sizeof(rsp.product_id));

	printk(BIOS_INFO, "IPMI: Found man_id 0x%06x, prod_id 0x%04x\n", man_id, prod_id);

	printk(BIOS_INFO, "IPMI: Version %01x.%01x\n", ipmi_revision_major,
	       ipmi_revision_minor);

	if (CONFIG(DRIVERS_IPMI_SUPERMICRO_OEM))
		supermicro_ipmi_oem(dev->path.pnp.port);
}

#if CONFIG(HAVE_ACPI_TABLES)
static uint32_t uid_cnt = 0;

static unsigned long
ipmi_write_acpi_tables(const struct device *dev, unsigned long current,
		       struct acpi_rsdp *rsdp)
{
	struct drivers_ipmi_config *conf = dev->chip_info;
	struct acpi_spmi *spmi;
	s8 gpe_interrupt = -1;
	u32 apic_interrupt = 0;
	acpi_addr_t addr = {
		.space_id = ACPI_ADDRESS_SPACE_IO,
		.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS,
		.addrl = dev->path.pnp.port,
		.bit_width = 8,
	};

	switch (CONFIG_IPMI_KCS_REGISTER_SPACING) {
	case 4:
		addr.bit_offset = 32;
		break;
	case 16:
		addr.bit_offset = 128;
		break;
	default:
		printk(BIOS_ERR, "IPMI: Unsupported register spacing for SPMI\n");
		__fallthrough;
	case 1:
		addr.bit_offset = 8;
		break;
	}

	current = ALIGN_UP(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SPMI at %lx\n", current);
	spmi = (struct acpi_spmi *)current;

	if (conf) {
		if (conf->have_gpe)
			gpe_interrupt = conf->gpe_interrupt;
		if (conf->have_apic)
			apic_interrupt = conf->apic_interrupt;

		/* Use command to get UID from ipmi_ssdt */
		acpi_create_ipmi(dev, spmi, (ipmi_revision_major << 8) |
				 (ipmi_revision_minor << 4), &addr,
				 IPMI_INTERFACE_KCS, gpe_interrupt, apic_interrupt,
				 conf->uid);

		acpi_add_table(rsdp, spmi);

		current += spmi->header.length;
	} else {
		printk(BIOS_WARNING, "IPMI: chip_info is missing!\n");
	}

	return current;
}

static void ipmi_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	struct drivers_ipmi_config *conf = dev->chip_info;

	if (!scope) {
		printk(BIOS_ERR, "IPMI: Missing ACPI scope for %s\n",
		       dev_path(dev));
		return;
	}

	if (!conf) {
		printk(BIOS_WARNING, "IPMI: chip_info is missing!\n");
		return;
	}

	/* Use command to pass UID to ipmi_write_acpi_tables */
	conf->uid = uid_cnt++;

	/* write SPMI device */
	acpigen_write_scope(scope);
	acpigen_write_device("SPMI");
	acpigen_write_name_string("_HID", "IPI0001");
	acpigen_write_name_unicode("_STR", "IPMI_KCS");
	acpigen_write_name_byte("_UID", conf->uid);
	acpigen_write_STA(0xf);
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_io16(dev->path.pnp.port, dev->path.pnp.port, 1, 1, 1);
	acpigen_write_io16(dev->path.pnp.port + CONFIG_IPMI_KCS_REGISTER_SPACING,
			   dev->path.pnp.port + CONFIG_IPMI_KCS_REGISTER_SPACING, 1, 1, 1);

	// FIXME: is that correct?
	if (conf->have_apic)
		acpigen_write_irq(1 << conf->apic_interrupt);

	acpigen_write_resourcetemplate_footer();

	acpigen_write_method("_IFT", 0);
	acpigen_write_return_byte(1);	// KCS
	acpigen_pop_len();

	acpigen_write_method("_SRV", 0);
	acpigen_write_return_integer((ipmi_revision_major << 8) |
				     (ipmi_revision_minor << 4));
	acpigen_pop_len();

	acpigen_pop_len(); /* pop device */
	acpigen_pop_len(); /* pop scope */
}
#endif

void ipmi_bmc_version(uint8_t *ipmi_bmc_major_revision, uint8_t *ipmi_bmc_minor_revision)
{
	*ipmi_bmc_major_revision = bmc_revision_major;
	*ipmi_bmc_minor_revision = bmc_revision_minor;
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int ipmi_smbios_data(struct device *dev, int *handle,
			    unsigned long *current)
{
	struct drivers_ipmi_config *conf = dev->chip_info;
	u8 nv_storage = 0xff;
	u8 i2c_address = 0;
	u8 register_spacing;

	int len = 0;

	if (conf) {
		if (conf->have_nv_storage)
			nv_storage = conf->nv_storage_device_address;
		i2c_address = conf->bmc_i2c_address;
	}

	switch (CONFIG_IPMI_KCS_REGISTER_SPACING) {
	case 4:
		register_spacing = 1 << 6;
		break;
	case 16:
		register_spacing = 2 << 6;
		break;
	default:
		printk(BIOS_ERR, "IPMI: Unsupported register spacing for SMBIOS\n");
		__fallthrough;
	case 1:
		register_spacing = 0 << 6;
		break;
	}

	// add IPMI Device Information
	len += smbios_write_type38(
		current, handle,
		SMBIOS_BMC_INTERFACE_KCS,
		ipmi_revision_minor | (ipmi_revision_major << 4),
		i2c_address, // I2C address
		nv_storage, // NV storage
		dev->path.pnp.port | 1, // IO interface
		register_spacing,
		0); // no IRQ

	len += get_smbios_data(dev, handle, current);

	return len;
}
#endif

static void ipmi_set_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED))
			continue;

		res->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, res, "");
	}
}

static void ipmi_read_resources(struct device *dev)
{
	struct resource *res = new_resource(dev, 0);
	res->base = dev->path.pnp.port;
	res->size = 2;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations ops = {
	.read_resources   = ipmi_read_resources,
	.set_resources    = ipmi_set_resources,
	.init             = ipmi_kcs_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = ipmi_write_acpi_tables,
	.acpi_fill_ssdt    = ipmi_ssdt,
#endif
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data = ipmi_smbios_data,
#endif
};

static void enable_dev(struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PNP)
		printk(BIOS_ERR, "%s: Unsupported device type\n",
		       dev_path(dev));
	else if (dev->path.pnp.port & 1)
		printk(BIOS_ERR, "%s: Base address needs to be aligned to 2\n",
		       dev_path(dev));
	else
		dev->ops = &ops;
}

struct chip_operations drivers_ipmi_ops = {
	CHIP_NAME("IPMI KCS")
	.enable_dev = enable_dev,
};
