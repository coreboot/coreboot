ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_STB),y)

bootblock-y += stb.c
romstage-y += stb.c
ramstage-y += stb.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_STB
