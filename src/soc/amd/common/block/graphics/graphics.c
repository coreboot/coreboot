/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <amdblocks/graphics.h>
#include <amdblocks/vbios_cache.h>
#include <amdblocks/vbt.h>
#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fmap.h>
#include <security/vboot/vbios_cache_hash_tpm.h>
#include <timestamp.h>

static bool vbios_loaded_from_cache = false;
static uint8_t vbios_data[VBIOS_CACHE_FMAP_SIZE];

#define ATIF_FUNCTION_VERIFY_INTERFACE 0x0
struct atif_verify_interface_output {
	uint16_t size; /* Size of this object, including size field */
	uint16_t version;
	uint32_t supported_notifications;
	uint32_t supported_functions; /* Bit n set if function n+1 supported. */
};

#define ATIF_FUNCTION_QUERY_BRIGHTNESS_TRANSFER_CHARACTERISTICS		0x10
# define ATIF_QBTC_REQUEST_LCD1					0
/* error codes */
# define ATIF_QBTC_ERROR_CODE_SUCCESS				0
# define ATIF_QBTC_ERROR_CODE_FAILURE				1
# define ATIF_QBTC_ERROR_CODE_DEVICE_NOT_SUPPORTED		2
struct atif_brightness_input {
	uint16_t size;
	/* ATIF doc indicates this field is a word, but the kernel drivers uses a byte. */
	uint8_t requested_display;
};
struct atif_brightness_output {
	uint16_t size; /* Size of this object, including size field. */
	uint16_t flags; /* Currently all reserved. */
	uint8_t error_code;
	/* default brightness fields currently ignored by Linux driver. */
	uint8_t default_brightness_ac; /* Percentage brightness when connected to AC. */
	uint8_t default_brightness_dc; /* Percentage brightness when connected to DC. */
	/* The following 2 fields are the only ones honored by Linux driver currently. */
	uint8_t min_input_signal_level; /* 0-255 corresponding to 0% */
	uint8_t max_input_signal_level; /* 0-255 corresponding to 100% */
	/* Array of data points consisting of:
	 *  { uint8_t luminance_level; (percent)
	 *    uint8_t input_signal_level; (0-255 in value) }
	 * Linux ignores these fields so no support currently. */
	uint8_t count_data_points; /* Count of data points. */
};

static void generate_atif(const struct device *dev)
{
	struct atif_verify_interface_output verify_output = {
		.size = sizeof(verify_output),
		.version = 1,
		.supported_functions =
			BIT(ATIF_FUNCTION_QUERY_BRIGHTNESS_TRANSFER_CHARACTERISTICS - 1),
	};
	struct atif_brightness_output brightness_error = {
		.size = sizeof(brightness_error),
		.error_code = ATIF_QBTC_ERROR_CODE_DEVICE_NOT_SUPPORTED,
	};
	struct atif_brightness_output brightness_out = {
		.size = sizeof(brightness_out),
		.error_code = ATIF_QBTC_ERROR_CODE_SUCCESS,
		.min_input_signal_level = 0,
		.max_input_signal_level = 255,
	};

	/* Scope (\_SB.PCI0.PBRA.IGFX) */
	acpigen_write_scope(acpi_device_path(dev));
	/* Method (ATIF, 2, NotSerialized) */
	acpigen_write_method("ATIF", 2);
	/* ToInteger (Arg0, Local0) */
	acpigen_write_to_integer(ARG0_OP, LOCAL0_OP);

	/* If ((Local0 == Zero)) */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, ATIF_FUNCTION_VERIFY_INTERFACE);
	/* Return (Buffer (0x0C) { ... } */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&verify_output,
		sizeof(verify_output));

	/* ElseIf ((Local0 == 0x10)) */
	acpigen_write_else();
	acpigen_write_if_lequal_op_int(LOCAL0_OP,
		ATIF_FUNCTION_QUERY_BRIGHTNESS_TRANSFER_CHARACTERISTICS);
	/* CreateByteField (Arg1, 0x02, DISP) */
	acpigen_write_create_byte_field(ARG1_OP,
		offsetof(struct atif_brightness_input, requested_display), "DISP");
	/* ToInteger (DISP, Local1) */
	acpigen_write_to_integer_from_namestring("DISP", LOCAL1_OP);
	/* If ((Local1 == Zero)) */
	acpigen_write_if_lequal_op_int(LOCAL1_OP, ATIF_QBTC_REQUEST_LCD1);
	/* Return (Buffer (0x0A) { ... } */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&brightness_out,
		sizeof(brightness_out));
	/* Else */
	acpigen_write_else();
	/* Return (Buffer (0x0A) */
	acpigen_write_return_byte_buffer((uint8_t *)(void *)&brightness_error,
		sizeof(brightness_error));
	acpigen_pop_len(); /* else */

	acpigen_pop_len(); /* if (LEqual(Local0, 0x10) */
	acpigen_pop_len(); /* else */

	acpigen_pop_len(); /* Method */
	acpigen_pop_len(); /* Scope */
}

static void generate_acp(const struct device *dev)
{
	/* Scope (\_SB.PCI0.IGFX) */
	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_device("ACP");

	acpigen_write_name_string("_HID", "BOOT0003");

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static void graphics_fill_ssdt(const struct device *dev)
{
	acpi_device_write_pci_dev(dev);

	/* Use the VFCT copy when using GOP */
	if (!CONFIG(RUN_FSP_GOP))
		pci_rom_ssdt(dev);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_GRAPHICS_ATIF))
		generate_atif(dev);

	if (CONFIG(SOC_AMD_COMMON_BLOCK_GRAPHICS_ACP))
		generate_acp(dev);
}

static const char *graphics_acpi_name(const struct device *dev)
{
	return "IGFX";
}

void *vbt_get(void)
{
	if (CONFIG(RUN_FSP_GOP))
		return (void *)(uintptr_t)PCI_VGA_RAM_IMAGE_START;

	return NULL;
}

static void graphics_set_resources(struct device *const dev)
{
	struct rom_header *rom, *ram;

	pci_dev_set_resources(dev);

	if (!CONFIG(RUN_FSP_GOP))
		return;

	/* Load the VBIOS before FSP AFTER_PCI_ENUM notify is called. */
	timestamp_add_now(TS_OPROM_INITIALIZE);
	if (CONFIG(USE_SELECTIVE_GOP_INIT) && vbios_cache_is_valid() &&
			!display_init_required()) {
		vbios_load_from_cache();
		timestamp_add_now(TS_OPROM_COPY_END);
		return;
	}

	/*
	 * VBIOS cache was not used, so load it from CBFS and let FSP GOP
	 * initialize the ATOMBIOS tables.
	 */
	rom = pci_rom_probe(dev);
	if (rom == NULL) {
		printk(BIOS_ERR, "%s: Unable to find ROM for %s\n",
		       __func__, dev_path(dev));
		timestamp_add_now(TS_OPROM_COPY_END);
		return;
	}

	ram = pci_rom_load(dev, rom);
	if (ram == NULL) {
		printk(BIOS_ERR, "%s: Unable to load ROM for %s\n",
		       __func__, dev_path(dev));
	}

	pci_rom_free(rom);

	timestamp_add_now(TS_OPROM_COPY_END);
}

static void graphics_dev_init(struct device *const dev)
{
	if (CONFIG(RUN_FSP_GOP))
		fsp_graphics_init(dev);

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);
}

static void read_vbios_cache_from_fmap(void *unused)
{
	struct region_device rw_vbios_cache;
	int32_t region_size;

	if (!CONFIG(SOC_AMD_GFX_CACHE_VBIOS_IN_FMAP))
		return;

	if (fmap_locate_area_as_rdev(VBIOS_CACHE_FMAP_NAME, &rw_vbios_cache)) {
		printk(BIOS_ERR, "%s: No %s FMAP section.\n", __func__, VBIOS_CACHE_FMAP_NAME);
		return;
	}

	region_size = region_device_sz(&rw_vbios_cache);

	if (region_size != VBIOS_CACHE_FMAP_SIZE) {
		printk(BIOS_ERR, "%s: %s FMAP size mismatch for VBIOS cache (%d vs %d).\n",
		       __func__, VBIOS_CACHE_FMAP_NAME, VBIOS_CACHE_FMAP_SIZE, region_size);
		return;
	}

	/* Read cached VBIOS data into buffer */
	if (rdev_readat(&rw_vbios_cache, &vbios_data, 0, VBIOS_CACHE_FMAP_SIZE) != VBIOS_CACHE_FMAP_SIZE) {
		printk(BIOS_ERR, "Failed to read vbios data from flash; rdev_readat() failed.\n");
		return;
	}

	printk(BIOS_SPEW, "VBIOS cache successfully read from FMAP.\n");
}

static void write_vbios_cache_to_fmap(void *unused)
{
	if (!CONFIG(SOC_AMD_GFX_CACHE_VBIOS_IN_FMAP))
		return;

	/* Don't save if VBIOS loaded from cache / data unchanged */
	if (vbios_loaded_from_cache == true) {
		printk(BIOS_SPEW, "VBIOS data loaded from cache; not saving\n");
		return;
	}

	struct region_device rw_vbios_cache;

	if (fmap_locate_area_as_rdev_rw(VBIOS_CACHE_FMAP_NAME, &rw_vbios_cache)) {
		printk(BIOS_ERR, "%s: No %s FMAP section.\n", __func__, VBIOS_CACHE_FMAP_NAME);
		return;
	}

	/* copy from PCI_VGA_RAM_IMAGE_START to rdev */
	if (rdev_writeat(&rw_vbios_cache, vbt_get(), 0,
						VBIOS_CACHE_FMAP_SIZE) != VBIOS_CACHE_FMAP_SIZE)
		printk(BIOS_ERR, "Failed to save vbios data to flash; rdev_writeat() failed.\n");

	/* copy modified vbios data to buffer before hashing */
	memcpy(vbios_data, vbt_get(), VBIOS_CACHE_FMAP_SIZE);

	/* save data hash to TPM NVRAM for validation on subsequent boots */
	vbios_cache_update_hash(vbios_data, VBIOS_CACHE_FMAP_SIZE);

	printk(BIOS_SPEW, "VBIOS cache successfully written to FMAP.\n");
}

/*
 * Loads cached VBIOS data into legacy oprom location.
 *
 * Assumes user has called vbios_cache_is_valid() and checked for success
 */
void vbios_load_from_cache(void)
{
	/* copy cached vbios data from buffer to address used by FSP */
	memcpy(vbt_get(), vbios_data, VBIOS_CACHE_FMAP_SIZE);

	/* mark cache as used so we know not to write it later */
	vbios_loaded_from_cache = true;
}

/*
 * Return true if VBIOS cache data is valid
 *
 * Compare first 2 bytes of data with known signature
 * and hash of data with hash stored in TPM NVRAM
 */
bool vbios_cache_is_valid(void)
{
	bool sig_valid = vbios_data[0] == 0x55 && vbios_data[1] == 0xaa;
	return sig_valid && vbios_cache_verify_hash(vbios_data, VBIOS_CACHE_FMAP_SIZE) == CB_SUCCESS;
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, read_vbios_cache_from_fmap, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_ENTRY, write_vbios_cache_to_fmap, NULL);

const struct device_operations amd_graphics_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= graphics_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= graphics_dev_init,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_dev_ops_pci,
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= graphics_fill_ssdt,
	.acpi_name		= graphics_acpi_name,
};
