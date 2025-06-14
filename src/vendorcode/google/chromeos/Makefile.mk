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

romstage-$(CONFIG_PLATFORM_HAS_EARLY_LOW_BATTERY_INDICATOR) += battery.c
romstage-$(CONFIG_CHROMEOS_DRAM_PART_NUMBER_IN_CBI) += dram_part_num_override.c

ramstage-$(CONFIG_PLATFORM_HAS_LOW_BATTERY_INDICATOR) += battery.c
ramstage-$(CONFIG_CHROMEOS_FW_SPLASH_SCREEN) += splash.c
ramstage-$(CONFIG_CHROMEOS_PVMFW_CBMEM) += pvmfw_cbmem.c

# Add logo to the cbfs image
BMP_LOGO_COMPRESS_FLAG := $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_BMP_LOGO_COMPRESS_LZMA),y)
	BMP_LOGO_COMPRESS_FLAG := LZMA
else ifeq ($(CONFIG_BMP_LOGO_COMPRESS_LZ4),y)
	BMP_LOGO_COMPRESS_FLAG := LZ4
endif

define add_bmp_logo_file_to_cbfs
cbfs-files-$$($(1)) += $(2)
$(2)-file := $$(call strip_quotes,$$($(3)))
$(2)-type := raw
$(2)-compression := $$(BMP_LOGO_COMPRESS_FLAG)
endef

$(eval $(call add_bmp_logo_file_to_cbfs,CONFIG_CHROMEOS_FW_SPLASH_SCREEN, \
	      cb_logo.bmp,CONFIG_CHROMEOS_LOGO_PATH))
ifneq ($(CONFIG_SPLASH_SCREEN_FOOTER),y)
$(eval $(call add_bmp_logo_file_to_cbfs,CONFIG_CHROMEOS_FW_SPLASH_SCREEN, \
	      cb_plus_logo.bmp,CONFIG_CHROMEBOOK_PLUS_LOGO_PATH))
endif
