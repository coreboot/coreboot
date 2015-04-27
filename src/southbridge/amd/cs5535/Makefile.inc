ifeq ($(CONFIG_SOUTHBRIDGE_AMD_CS5535),y)

ramstage-y += cs5535.c
#ramstage-y += pci.c
#ramstage-y += ide.c
ramstage-y += chipsetinit.c

endif
