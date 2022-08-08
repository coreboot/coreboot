ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)
romstage-y += fsp_reset.c
romstage-y += fsp_validate.c
ramstage-y += fsp_reset.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fsp-acpi.c
ramstage-$(CONFIG_SOC_AMD_COMMON_FSP_CCX_CPPC_HOB) += fsp_ccx_cppc_hob.c
ramstage-$(CONFIG_SOC_AMD_COMMON_FSP_DMI_TABLES) += dmi.c

subdirs-y += ./*

ifeq ($(CONFIG_FSP_M_FILE),)
$(info There's no FSP-M binary file specified. The resulting image won't boot successfully.)
else
ifeq ($(call int-gt,\
	$(call file-size,$(CONFIG_FSP_M_FILE))\
	$(shell printf "%d" $(CONFIG_FSP_M_SIZE))),\
	1)
$(error FSP-M binary larger than FSP_M_FILE.)
endif
endif # CONFIG_FSP_M_FILE

endif # CONFIG_PLATFORM_USES_FSP2_0
