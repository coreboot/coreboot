ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_P2SB),y)
bootblock-y += p2sb.c
romstage-y += p2sb.c
ramstage-y += p2sb.c
ramstage-y += p2sblib.c
smm-y += p2sb.c
smm-y += p2sblib.c
endif
