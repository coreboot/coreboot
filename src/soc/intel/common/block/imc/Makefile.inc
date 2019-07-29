ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_IMC),y)

bootblock-y += imc.c
romstage-y += imc.c
verstage-y += imc.c
postcar-y += imc.c
ramstage-y += imc.c

endif
