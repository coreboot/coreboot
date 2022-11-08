ifeq ($(CONFIG_NORTHBRIDGE_INTEL_E7505),y)

ramstage-y += northbridge.c
ramstage-y += memmap.c

romstage-y += romstage.c
romstage-y += raminit.c
romstage-y += memmap.c

endif
