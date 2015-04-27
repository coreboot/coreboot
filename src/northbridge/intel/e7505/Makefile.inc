ifeq ($(CONFIG_NORTHBRIDGE_INTEL_E7505),y)

ramstage-y += northbridge.c
romstage-y += raminit.c
romstage-y += debug.c

endif
