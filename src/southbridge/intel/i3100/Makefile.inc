ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I3100),y)

ramstage-y += i3100.c
ramstage-y += uhci.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += ehci.c
ramstage-y += smbus.c
ramstage-y += pci.c
ramstage-y += ioapic.c
ramstage-y += reset.c
ramstage-y += pciexp_portb.c

endif
