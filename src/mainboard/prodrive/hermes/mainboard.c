/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <crc_byte.h>
#include <device/device.h>
#include <device/dram/spd.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/opregion.h>
#include <gpio.h>
#include <intelblocks/gpio.h>
#include <intelblocks/pmclib.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <soc/pm.h>
#include <string.h>
#include <types.h>

#include "eeprom.h"
#include "gpio.h"

const char *mainboard_vbt_filename(void)
{
	const struct eeprom_bmc_settings *bmc_cfg = get_bmc_settings();

	if (bmc_cfg && bmc_cfg->efp3_displayport)
		return "vbt-avalanche.bin";
	else
		return "vbt.bin"; /* Poseidon */
}

/* FIXME: Example code below */

static void mb_configure_dp1_pwr(bool enable)
{
	gpio_output(GPP_K3, enable);
}

static void mb_configure_dp2_pwr(bool enable)
{
	gpio_output(GPP_K4, enable);
}

static void mb_configure_dp3_pwr(bool enable)
{
	gpio_output(GPP_K5, enable);
}

static void mb_hda_amp_enable(bool enable)
{
	gpio_output(GPP_C19, enable);
}

static void mb_usb31_rp1_pwr_enable(bool enable)
{
	gpio_output(GPP_G0, enable);
}

static void mb_usb31_rp2_pwr_enable(bool enable)
{
	gpio_output(GPP_G1, enable);
}

static void mb_usb31_fp_pwr_enable(bool enable)
{
	gpio_output(GPP_G2, enable);
}

static void mb_usb2_fp1_pwr_enable(bool enable)
{
	gpio_output(GPP_G3, enable);
}

static void mb_usb2_fp2_pwr_enable(bool enable)
{
	gpio_output(GPP_G4, enable);
}

static void copy_meminfo(const struct dimm_info *dimm, union eeprom_dimm_layout *l)
{
	memset(l, 0, sizeof(*l));
	if (dimm->dimm_size == 0)
		return;

	strncpy(l->name, (char *)dimm->module_part_number, sizeof(l->name) - 1);
	l->capacity_mib = dimm->dimm_size;
	l->data_width_bits = 8 * (1 << (dimm->bus_width & 0x7));
	l->bus_width_bits = l->data_width_bits + 8 * ((dimm->bus_width >> 3) & 0x3);
	l->ranks = dimm->rank_per_dimm;
	l->controller_id = 0;
	strncpy(l->manufacturer, spd_manufacturer_name(dimm->mod_id),
		sizeof(l->manufacturer) - 1);
}

/*
 * Collect board specific settings and update the CFG EEPROM if necessary.
 * This allows the BMC webui to display the current hardware configuration.
 */
static void update_board_layout(void)
{
	struct eeprom_board_layout layout = {0};

	printk(BIOS_INFO, "MB: Collecting Board Layout information\n");

	/* Update CPU fields */
	for (struct device *cpu = all_devices; cpu; cpu = cpu->next) {
		if (!is_enabled_cpu(cpu))
			continue;
		layout.cpu_count++;
		if (!layout.cpu_name[0])
			strcpy(layout.cpu_name, cpu->name);
	}

	if (cpuid_get_max_func() >= 0x16)
		layout.cpu_max_non_turbo_frequency = cpuid_eax(0x16);

	/* PCH */
	strcpy(layout.pch_name, "Cannonlake-H C246");

	/* DRAM */
	struct memory_info *meminfo = cbmem_find(CBMEM_ID_MEMINFO);
	if (meminfo) {
		const size_t meminfo_max = MIN(meminfo->dimm_cnt, ARRAY_SIZE(meminfo->dimm));
		for (size_t i = 0; i < MIN(meminfo_max, ARRAY_SIZE(layout.dimm)); i++)
			copy_meminfo(&meminfo->dimm[i], &layout.dimm[i]);
	}

	/* Update CRC */
	layout.signature = CRC(layout.raw_layout, sizeof(layout.raw_layout), crc32_byte);

	printk(BIOS_DEBUG, "BOARD LAYOUT:\n");
	printk(BIOS_DEBUG, " Signature : 0x%x\n", layout.signature);
	printk(BIOS_DEBUG, " CPU name  : %s\n", layout.cpu_name);
	printk(BIOS_DEBUG, " CPU count : %u\n", layout.cpu_count);
	printk(BIOS_DEBUG, " CPU freq  : %u\n", layout.cpu_max_non_turbo_frequency);
	printk(BIOS_DEBUG, " PCH name  : %s\n", layout.pch_name);
	for (size_t i = 0; i < ARRAY_SIZE(layout.dimm); i++)
		printk(BIOS_DEBUG, " DRAM SIZE : %u\n", layout.dimm[i].capacity_mib);

	if (write_board_settings(&layout))
		printk(BIOS_ERR, "MB: Failed to update Board Layout\n");
}

static void mainboard_init(void *chip_info)
{
	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	/* Enable internal speaker amplifier */
	if (board_cfg->front_panel_audio == 2)
		mb_hda_amp_enable(1);
	else
		mb_hda_amp_enable(0);
}

static void mainboard_final(struct device *dev)
{
	update_board_layout();

	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	/* Encoding: 0 -> S0, 1 -> S5 */
	const bool on = !board_cfg->power_state_after_g3;

	pmc_soc_set_afterg3_en(on);
}

static const char *format_pn(const char *prefix, size_t offset)
{
	static char buffer[32 + HERMES_SN_PN_LENGTH] = { 0 };

	const char *part_num = eeprom_read_serial(offset, "N/A");

	snprintf(buffer, sizeof(buffer), "%s%s", prefix, part_num);

	return buffer;
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	const size_t board_offset = offsetof(struct eeprom_layout, board_part_number);
	const size_t product_offset = offsetof(struct eeprom_layout, product_part_number);
	t->count = smbios_add_string(t->eos, format_pn("Board P/N: ", board_offset));
	t->count = smbios_add_string(t->eos, format_pn("Product P/N: ", product_offset));
}

#if CONFIG(HAVE_ACPI_TABLES)
static void mainboard_acpi_fill_ssdt(const struct device *dev)
{
	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	const unsigned int usb_power_gpios[] = { GPP_G0, GPP_G1, GPP_G2, GPP_G3, GPP_G4 };

	/* Function pointer to write STXS or CTXS according to EEPROM board setting */
	int (*acpigen_write_soc_gpio_op)(unsigned int gpio_num);

	if (board_cfg->usb_powered_in_s5)
		acpigen_write_soc_gpio_op = acpigen_soc_set_tx_gpio;
	else
		acpigen_write_soc_gpio_op = acpigen_soc_clear_tx_gpio;

	acpigen_write_method("\\_SB.MPTS", 1);
	{
		acpigen_write_if_lequal_op_int(ARG0_OP, 5);
		{
			for (size_t i = 0; i < ARRAY_SIZE(usb_power_gpios); i++)
				acpigen_write_soc_gpio_op(usb_power_gpios[i]);
		}
		acpigen_pop_len();
	}
	acpigen_pop_len();
}
#endif

static void mainboard_enable(struct device *dev)
{
	/* FIXME: Do runtime configuration once the board is production ready */
	mb_configure_dp1_pwr(1);
	mb_configure_dp2_pwr(1);
	mb_configure_dp3_pwr(1);

	mb_usb31_rp1_pwr_enable(1);
	mb_usb31_rp2_pwr_enable(1);
	mb_usb31_fp_pwr_enable(1);
	mb_usb2_fp1_pwr_enable(1);
	mb_usb2_fp2_pwr_enable(1);

	dev->ops->final = mainboard_final;
	dev->ops->get_smbios_strings = mainboard_smbios_strings;

#if CONFIG(HAVE_ACPI_TABLES)
	dev->ops->acpi_fill_ssdt = mainboard_acpi_fill_ssdt;
#endif
}

struct chip_operations mainboard_ops = {
	.init       = mainboard_init,
	.enable_dev = mainboard_enable,
};

static void log_reset_causes(void)
{
	struct chipset_power_state *ps = pmc_get_power_state();

	if (!ps) {
		printk(BIOS_ERR, "chipset_power_state not found!\n");
		return;
	}

	union {
		struct eeprom_reset_cause_regs regs;
		uint8_t raw[sizeof(struct eeprom_reset_cause_regs)];
	} reset_cause = {
		.regs = {
			.gblrst_cause0 = ps->gblrst_cause[0],
			.gblrst_cause1 = ps->gblrst_cause[1],
			.hpr_cause0 = ps->hpr_cause0,
		},
	};

	const size_t base = offsetof(struct eeprom_layout, reset_cause_regs);
	for (size_t i = 0; i < ARRAY_SIZE(reset_cause.raw); i++)
		eeprom_write_byte(reset_cause.raw[i], base + i);
}

/* Must happen before MPinit */
static void mainboard_early(void *unused)
{
	const struct eeprom_board_settings *const board_cfg = get_board_settings();
	config_t *config = config_of_soc();

	if (board_cfg) {
		/* Set Deep Sx */
		config->deep_s5_enable_ac = board_cfg->deep_sx_enabled;
		config->deep_s5_enable_dc = board_cfg->deep_sx_enabled;

		config->disable_vmx = board_cfg->vtx_disabled;
	}

	if (check_signature(offsetof(struct eeprom_layout, supd), FSPS_UPD_SIGNATURE)) {
		struct {
			struct {
				u8 TurboMode;
			} FspsConfig;
		} supd = {0};

		READ_EEPROM_FSP_S((&supd), FspsConfig.TurboMode);
		config->cpu_turbo_disable = !supd.FspsConfig.TurboMode;
	}

	log_reset_causes();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, mainboard_early, NULL);

/*
 * coreboot only exposes the last framebuffer that is set up.
 * The ASPEED BMC will always be initialized after the IGD due to its higher
 * bus number. To have coreboot only expose the IGD framebuffer skip the init
 * function on the ASPEED.
 */
static void mainboard_configure_internal_gfx(void *unused)
{
	struct device *dev;
	const struct eeprom_board_settings *board_cfg = get_board_settings();
	if (!board_cfg)
		return;

	if (board_cfg->primary_video == PRIMARY_VIDEO_INTEL) {
		dev = dev_find_device(PCI_VID_ASPEED, PCI_DID_ASPEED_AST2050_VGA, NULL);
		dev->on_mainboard = false;
		dev->enabled = false;
		dev->ops->init = NULL;
	}
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, mainboard_configure_internal_gfx, NULL)
