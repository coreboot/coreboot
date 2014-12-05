ramstage-y += model_206ax_init.c
subdirs-y += ../../x86/name

ramstage-y += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c
CPPFLAGS_romstage += -I$(src)/cpu/intel/fsp_model_206ax

ifneq ($(CONFIG_MICROCODE_INCLUDE_PATH),)
ifneq ($(wildcard $(shell readlink -f "$(top)/$(CONFIG_MICROCODE_INCLUDE_PATH)")),)
CPPFLAGS_common += -I$(CONFIG_MICROCODE_INCLUDE_PATH)
endif
endif
