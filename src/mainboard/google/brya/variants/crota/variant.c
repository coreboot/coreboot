/* SPDX-License-Identifier: GPL-2.0-only */

#include <sar.h>
#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <drivers/vpd/vpd.h>
#include <stdio.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

static const char *get_dock_mac_from_vpd(char *buf, int size)
{
	/* Support MAC address pass-through */
	/* Read value of 'dock_mac' from RO VPD */
	return vpd_gets("dock_mac", buf, size, VPD_RO);
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(DB_LTE, LTE_USB))) {
		config->ext_fivr_settings.configure_ext_fivr = 1;
		config->ext_fivr_settings.v1p05_enable_bitmap = FIVR_ENABLE_ALL_SX;
		config->ext_fivr_settings.vnn_enable_bitmap = FIVR_ENABLE_ALL_SX;
		config->ext_fivr_settings.v1p05_supported_voltage_bitmap = FIVR_VOLTAGE_NORMAL |
								FIVR_VOLTAGE_MIN_ACTIVE |
								FIVR_VOLTAGE_MIN_RETENTION;
		config->ext_fivr_settings.vnn_supported_voltage_bitmap = FIVR_VOLTAGE_NORMAL |
								FIVR_VOLTAGE_MIN_ACTIVE |
								FIVR_VOLTAGE_MIN_RETENTION;
		config->ext_fivr_settings.v1p05_icc_max_ma = 500;
		config->ext_fivr_settings.vnn_sx_voltage_mv = 1250;
	}
}

void variant_fill_ssdt(const struct device *unused)
{
	/* Write MAC address to SSDT for Linux kernel r8152 driver */
	/* enable pass-through support */
	/* and read ACPI object name: "\_SB.AMAC" */

	/* ASL code like this */
	//     Scope (\_SB)
	// {
	//     Method (AMAC, 0, Serialized)
	//     {
	//         Return (ToBuffer (STRING))
	//     }
	// }

	char buf[32], acpi_buf[32];
	if (get_dock_mac_from_vpd(buf, sizeof(buf)) != NULL) {
		printk(BIOS_INFO, "RO_VPD, dock_mac=%s\n", buf);

		/* remove ':' from mac address string */
		size_t len = strlen(buf);
		int i, j;
		for (i = 0; i < len; i++) {
			if (buf[i] == ':') {
				for (j = i; j < len; j++)
					buf[j] = buf[j+1];
				len--;
				i--;
			}
		}
		buf[len] = '\0';

		/* Format expected by the Linux kernel r8152 driver */
		/* "_AUXMAC_#XXXXXXXXXXXX#" */
		int acpi_buf_len = snprintf(acpi_buf, sizeof(acpi_buf), "_AUXMAC_#%s#", buf);

		acpigen_write_scope("\\_SB");
		acpigen_write_method_serialized("AMAC", 0);

		acpigen_write_return_byte_buffer((uint8_t *)acpi_buf, acpi_buf_len);

		acpigen_write_method_end();
		acpigen_write_scope_end();
	}
}
