## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_CHROMEOS_NVS) += gnvs.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
ramstage-y += vpd_mac.c vpd_serialno.c vpd_calibration.c
ramstage-$(CONFIG_CHROMEOS_DISABLE_PLATFORM_HIERARCHY_ON_RESUME) += tpm2.c
ramstage-$(CONFIG_HAVE_REGULATORY_DOMAIN) += wrdd.c
ramstage-$(CONFIG_USE_SAR) += sar.c
ramstage-$(CONFIG_TPM_GOOGLE) += cr50_enable_update.c
ramstage-$(CONFIG_TPM_GOOGLE) += tpm_factory_config.c

romstage-$(CONFIG_CHROMEOS_CSE_BOARD_RESET_OVERRIDE) += cse_board_reset.c
ramstage-$(CONFIG_CHROMEOS_CSE_BOARD_RESET_OVERRIDE) += cse_board_reset.c

bootblock-y += watchdog.c
verstage-y += watchdog.c
romstage-y += watchdog.c
ramstage-y += watchdog.c

romstage-$(CONFIG_CHROMEOS_DRAM_PART_NUMBER_IN_CBI) += dram_part_num_override.c
ramstage-$(CONFIG_CHROMEOS_FW_SPLASH_SCREEN) += splash.c

# Add logo to the cbfs image
BMP_LOGO_COMPRESS_FLAG := $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_BMP_LOGO_COMPRESS_LZMA),y)
	BMP_LOGO_COMPRESS_FLAG := LZMA
else ifeq ($(CONFIG_BMP_LOGO_COMPRESS_LZ4),y)
	BMP_LOGO_COMPRESS_FLAG := LZ4
endif

cbfs-files-$(CONFIG_CHROMEOS_FW_SPLASH_SCREEN) += cb_logo.bmp
cb_logo.bmp-file := $(call strip_quotes,$(CONFIG_CHROMEOS_LOGO_PATH))
cb_logo.bmp-type := raw
cb_logo.bmp-compression := $(BMP_LOGO_COMPRESS_FLAG)

cbfs-files-$(CONFIG_CHROMEOS_FW_SPLASH_SCREEN) += cb_plus_logo.bmp
cb_plus_logo.bmp-file := $(call strip_quotes,$(CONFIG_CHROMEBOOK_PLUS_LOGO_PATH))
cb_plus_logo.bmp-type := raw
cb_plus_logo.bmp-compression := $(BMP_LOGO_COMPRESS_FLAG)
