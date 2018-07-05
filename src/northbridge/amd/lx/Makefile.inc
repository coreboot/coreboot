ifeq ($(CONFIG_NORTHBRIDGE_AMD_LX),y)

ramstage-y += northbridge.c
ramstage-y += northbridgeinit.c
ramstage-y += grphinit.c

romstage-y += raminit.c
romstage-y += generic_sdram.c
romstage-y += pll_reset.c

endif
