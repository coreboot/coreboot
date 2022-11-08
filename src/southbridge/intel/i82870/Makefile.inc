ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82870),y)

ramstage-y += ioapic.c
ramstage-y += pcibridge.c

endif
