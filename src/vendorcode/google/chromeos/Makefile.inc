## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_CHROMEOS_NVS) += gnvs.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
ramstage-y += vpd_mac.c vpd_serialno.c vpd_calibration.c
ramstage-$(CONFIG_CHROMEOS_DISABLE_PLATFORM_HIERARCHY_ON_RESUME) += tpm2.c
ramstage-$(CONFIG_HAVE_REGULATORY_DOMAIN) += wrdd.c
ramstage-$(CONFIG_USE_SAR) += sar.c
ramstage-$(CONFIG_CHROMEOS_DSM_CALIB) += dsm_calib.c
ramstage-$(CONFIG_TPM_GOOGLE) += cr50_enable_update.c

romstage-$(CONFIG_CHROMEOS_CSE_BOARD_RESET_OVERRIDE) += cse_board_reset.c
ramstage-$(CONFIG_CHROMEOS_CSE_BOARD_RESET_OVERRIDE) += cse_board_reset.c

bootblock-y += watchdog.c
verstage-y += watchdog.c
romstage-y += watchdog.c
ramstage-y += watchdog.c

romstage-$(CONFIG_CHROMEOS_DRAM_PART_NUMBER_IN_CBI) += dram_part_num_override.c
