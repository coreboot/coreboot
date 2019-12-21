bootblock-$(CONFIG_MICROCODE_UPDATE_PRE_RAM) += microcode_asm.S

bootblock-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
ramstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
romstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
