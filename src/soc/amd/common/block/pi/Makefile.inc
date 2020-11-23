ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PI),y)

romstage-y += agesawrapper.c
romstage-y += def_callouts.c
romstage-y += heapmanager.c
romstage-y += image.c
romstage-y += refcode_loader.c

ramstage-y += agesawrapper.c
ramstage-y += amd_late_init.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += amd_resume_final.c
ramstage-y += def_callouts.c
ramstage-y += heapmanager.c
ramstage-y += image.c
ramstage-y += refcode_loader.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_PI
