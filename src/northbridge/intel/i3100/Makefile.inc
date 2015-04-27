ifeq ($(CONFIG_NORTHBRIDGE_INTEL_I3100),y)

ramstage-y += northbridge.c
ramstage-y += pciexp_porta.c
ramstage-y += pciexp_porta_ep80579.c

endif
