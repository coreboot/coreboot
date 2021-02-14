/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>
#include <intelblocks/pmclib.h>
#include <types.h>
#include "variants/baseboard/include/eeprom.h"
#include "gpio.h"

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

static void mainboard_init(void *chip_info)
{
	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	/* Set Deep Sx */
	config_t *config = config_of_soc();
	config->deep_s5_enable_ac = board_cfg->deep_sx_enabled;
	config->deep_s5_enable_dc = board_cfg->deep_sx_enabled;

	/* Enable internal speaker amplifier */
	if (board_cfg->internal_audio_connection == 2)
		mb_hda_amp_enable(1);
	else
		mb_hda_amp_enable(0);
}

static void mainboard_final(struct device *dev)
{
	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	/* Encoding: 0 -> S0, 1 -> S5 */
	const bool on = !board_cfg->power_state_after_g3;

	pmc_soc_set_afterg3_en(on);
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

	acpigen_write_scope("\\_SB");
	{
		acpigen_write_method("MPTS", 1);
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

#if CONFIG(HAVE_ACPI_TABLES)
	dev->ops->acpi_fill_ssdt = mainboard_acpi_fill_ssdt;
#endif
}

struct chip_operations mainboard_ops = {
	.init       = mainboard_init,
	.enable_dev = mainboard_enable,
};
