/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <intelblocks/cpulib.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <stdint.h>

#include "commands.h"
#include "ec.h"
#include "chip.h"

/*
 * Setting minimum length of UCSI_ACPI will ensure this region is placed out of IMD Small.
 * Having this region out of IMD Small will prevent any memory mapping conflicts.
 */
#define UCSI_MIN_ALLOC_REGION_LEN	CBMEM_SM_ROOT_SIZE
/*
 * The UCSI fields are defined in the UCSI specification at
 * https://www.intel.com/content/www/us/en/io/universal-serial-bus/usb-type-c-ucsi-spec.html
 * https://www.intel.com/content/www/us/en/io/universal-serial-bus/bios-implementation-of-ucsi.html
 */

static struct fieldlist ucsi_region_fields[] = {
	FIELDLIST_NAMESTR("VER0", 8),
	FIELDLIST_NAMESTR("VER1", 8),
	FIELDLIST_NAMESTR("RSV0", 8),
	FIELDLIST_NAMESTR("RSV1", 8),
	FIELDLIST_NAMESTR("CCI0", 8),
	FIELDLIST_NAMESTR("CCI1", 8),
	FIELDLIST_NAMESTR("CCI2", 8),
	FIELDLIST_NAMESTR("CCI3", 8),
	FIELDLIST_NAMESTR("CTL0", 8),
	FIELDLIST_NAMESTR("CTL1", 8),
	FIELDLIST_NAMESTR("CTL2", 8),
	FIELDLIST_NAMESTR("CTL3", 8),
	FIELDLIST_NAMESTR("CTL4", 8),
	FIELDLIST_NAMESTR("CTL5", 8),
	FIELDLIST_NAMESTR("CTL6", 8),
	FIELDLIST_NAMESTR("CTL7", 8),
	FIELDLIST_NAMESTR("MGI0", 8),
	FIELDLIST_NAMESTR("MGI1", 8),
	FIELDLIST_NAMESTR("MGI2", 8),
	FIELDLIST_NAMESTR("MGI3", 8),
	FIELDLIST_NAMESTR("MGI4", 8),
	FIELDLIST_NAMESTR("MGI5", 8),
	FIELDLIST_NAMESTR("MGI6", 8),
	FIELDLIST_NAMESTR("MGI7", 8),
	FIELDLIST_NAMESTR("MGI8", 8),
	FIELDLIST_NAMESTR("MGI9", 8),
	FIELDLIST_NAMESTR("MGIA", 8),
	FIELDLIST_NAMESTR("MGIB", 8),
	FIELDLIST_NAMESTR("MGIC", 8),
	FIELDLIST_NAMESTR("MGID", 8),
	FIELDLIST_NAMESTR("MGIE", 8),
	FIELDLIST_NAMESTR("MGIF", 8),
	FIELDLIST_NAMESTR("MGO0", 8),
	FIELDLIST_NAMESTR("MGO1", 8),
	FIELDLIST_NAMESTR("MGO2", 8),
	FIELDLIST_NAMESTR("MGO3", 8),
	FIELDLIST_NAMESTR("MGO4", 8),
	FIELDLIST_NAMESTR("MGO5", 8),
	FIELDLIST_NAMESTR("MGO6", 8),
	FIELDLIST_NAMESTR("MGO7", 8),
	FIELDLIST_NAMESTR("MGO8", 8),
	FIELDLIST_NAMESTR("MGO9", 8),
	FIELDLIST_NAMESTR("MGOA", 8),
	FIELDLIST_NAMESTR("MGOB", 8),
	FIELDLIST_NAMESTR("MGOC", 8),
	FIELDLIST_NAMESTR("MGOD", 8),
	FIELDLIST_NAMESTR("MGOE", 8),
	FIELDLIST_NAMESTR("MGOF", 8),
};
static const size_t ucsi_region_len = ARRAY_SIZE(ucsi_region_fields);

static void wilco_ec_post_complete(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_POST_COMPLETE);
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,
		      wilco_ec_post_complete, NULL);

static void wilco_ec_post_memory_init(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_MEMORY_INIT);
}
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT,
		      wilco_ec_post_memory_init, NULL);

static void wilco_ec_post_video_init(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_VIDEO_INIT);
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT,
		      wilco_ec_post_video_init, NULL);

static void wilco_ec_post_logo_displayed(void *unused)
{
	wilco_ec_send(KB_BIOS_PROGRESS, BIOS_PROGRESS_LOGO_DISPLAYED);
}
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
		      wilco_ec_post_logo_displayed, NULL);

static void wilco_ec_resume(void *unused)
{
	wilco_ec_send_noargs(KB_RESTORE);
}
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, wilco_ec_resume, NULL);

static int wilco_set_cpu_id(void)
{
	uint32_t cpu_phy_cores, cpu_virtual_cores;

	cpu_read_topology(&cpu_phy_cores, &cpu_virtual_cores);
	return wilco_ec_set_cpuid(cpu_get_cpuid(), cpu_phy_cores, 0);
}

static int wilco_ec_set_charge_limits(uint8_t start, uint8_t stop, uint8_t cur_stop)
{
	/* Order the writes so the lower limit stays below the upper limit */
	if (start >= cur_stop) {
		if (wilco_ec_set_byte_property(PID_CHARGE_UPPER_LIMIT, stop))
			return -1;
		return wilco_ec_set_byte_property(PID_CHARGE_LOWER_LIMIT, start);
	}

	if (wilco_ec_set_byte_property(PID_CHARGE_LOWER_LIMIT, start))
		return -1;
	return wilco_ec_set_byte_property(PID_CHARGE_UPPER_LIMIT, stop);
}

/*
 * The charge mode and limits live in EC NVRAM and the OS may change them,
 * so only write them when they differ from the selected option values.
 */
static void wilco_ec_init_charge_mode(void)
{
	const unsigned int mode = get_uint_option_checked("ec_charge_mode",
		CHARGE_MODE_EC_DEFAULT,
		OPTION_ENUM(CHARGE_MODE_STANDARD, CHARGE_MODE_CUSTOM));
	const unsigned int start = get_uint_option_checked("ec_charge_start", 60,
		OPTION_RANGE(CHARGE_LOWER_LIMIT_MIN, CHARGE_LOWER_LIMIT_MAX));
	const unsigned int stop = get_uint_option_checked("ec_charge_stop", 90,
		OPTION_RANGE(CHARGE_UPPER_LIMIT_MIN, CHARGE_UPPER_LIMIT_MAX));
	uint8_t cur_mode, cur_start, cur_stop;
	bool changed = false;

	if (wilco_ec_get_byte_property(PID_CHARGE_MODE, &cur_mode) ||
	    wilco_ec_get_byte_property(PID_CHARGE_LOWER_LIMIT, &cur_start) ||
	    wilco_ec_get_byte_property(PID_CHARGE_UPPER_LIMIT, &cur_stop)) {
		printk(BIOS_ERR, "EC: failed to read battery charge settings\n");
		return;
	}

	printk(BIOS_INFO, "EC: battery charge mode %u, start %u%%, stop %u%%\n",
	       cur_mode, cur_start, cur_stop);

	if (mode == CHARGE_MODE_EC_DEFAULT) {
		printk(BIOS_INFO, "EC: battery charge mode override disabled\n");
		return;
	}

	if (cur_mode == CHARGE_MODE_LONG_LIFE) {
		printk(BIOS_WARNING, "EC: Long Life charge mode is locked\n");
		return;
	}

	if (mode == CHARGE_MODE_CUSTOM) {
		if (start + CHARGE_LIMIT_MIN_GAP > stop) {
			printk(BIOS_WARNING,
			       "EC: ignoring invalid charge limits start=%u stop=%u\n",
			       start, stop);
			return;
		}
		if (cur_start != start || cur_stop != stop) {
			if (wilco_ec_set_charge_limits(start, stop, cur_stop)) {
				printk(BIOS_ERR, "EC: failed to set battery charge limits\n");
				return;
			}
			changed = true;
		}
	}

	if (cur_mode != mode) {
		if (wilco_ec_set_byte_property(PID_CHARGE_MODE, mode)) {
			printk(BIOS_ERR, "EC: failed to set battery charge mode\n");
			return;
		}
		changed = true;
	}

	if (changed)
		printk(BIOS_INFO, "EC: battery charge mode set to %u, start %u%%, stop %u%%\n",
		       mode, start, stop);
}

static void wilco_ec_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	/* Disable S0ix support in EC RAM with ACPI EC interface */
	if (!acpi_is_wakeup_s3()) {
		ec_set_ports(CONFIG_EC_BASE_ACPI_COMMAND,
			     CONFIG_EC_BASE_ACPI_DATA);
		ec_write(EC_RAM_S0IX_SUPPORT, 0);
	}

	/* Print EC firmware information */
	wilco_ec_print_all_info();

	/* Initialize keyboard, ignore emulated PS/2 mouse */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* Direct power button to the host for processing */
	wilco_ec_send(KB_POWER_BUTTON_TO_HOST, 1);

	/* Unmute speakers */
	wilco_ec_send(KB_HW_MUTE_CONTROL, AUDIO_UNMUTE_125MS);

	/* Enable WiFi radio */
	wilco_ec_radio_control(RADIO_WIFI, 1);

	/* Turn on camera power */
	wilco_ec_send(KB_CAMERA, CAMERA_ON);

	/* Set cpu id and phy cores */
	if (wilco_set_cpu_id())
		printk(BIOS_ERR, "EC: use default cpu power table\n");

	wilco_ec_init_charge_mode();
}

static void wilco_ec_resource(struct device *dev, int index,
			      size_t base, size_t size)
{
	struct resource *res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void wilco_ec_read_resources(struct device *dev)
{
	/* ACPI command and data regions */
	wilco_ec_resource(dev, 0, CONFIG_EC_BASE_ACPI_DATA, 8);

	/* Host command and data regions */
	wilco_ec_resource(dev, 1, CONFIG_EC_BASE_HOST_DATA, 8);

	/* Packet region */
	wilco_ec_resource(dev, 2, CONFIG_EC_BASE_PACKET, 16);
}

static void wilco_ec_fill_ssdt_generator(const struct device *dev)
{
	struct opregion opreg;
	void *region_ptr;
	size_t ucsi_alloc_region_len;

	ucsi_alloc_region_len = ucsi_region_len < UCSI_MIN_ALLOC_REGION_LEN ?
		UCSI_MIN_ALLOC_REGION_LEN : ucsi_region_len;
	region_ptr = cbmem_add(CBMEM_ID_ACPI_UCSI, ucsi_alloc_region_len);
	if (!region_ptr)
		return;
	memset(region_ptr, 0, ucsi_alloc_region_len);

	opreg.name = "UCSM";
	opreg.regionspace = SYSTEMMEMORY;
	opreg.regionoffset = (uintptr_t)region_ptr;
	opreg.regionlen = ucsi_alloc_region_len;

	acpigen_write_scope("\\_SB.UCSI");
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, (uintptr_t)region_ptr, ucsi_region_len);
	acpigen_write_resourcetemplate_footer();
	acpigen_write_opregion(&opreg);
	acpigen_write_field(opreg.name, ucsi_region_fields, ucsi_region_len,
			    FIELD_ANYACC | FIELD_LOCK | FIELD_PRESERVE);
	acpigen_pop_len(); /* Scope */
}

static const char *wilco_ec_acpi_name(const struct device *dev)
{
	return "EC0";
}

static struct device_operations ops = {
	.init			= wilco_ec_init,
	.read_resources		= wilco_ec_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_fill_ssdt		= wilco_ec_fill_ssdt_generator,
	.acpi_name		= wilco_ec_acpi_name,
};

static struct pnp_info info[] = {
	{ NULL, 0, 0, 0, }
};

static void wilco_ec_enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(info), info);
}

struct chip_operations ec_google_wilco_ops = {
	.name = "Google Wilco EC",
	.enable_dev = wilco_ec_enable_dev,
};
