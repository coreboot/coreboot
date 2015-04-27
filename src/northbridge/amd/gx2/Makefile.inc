ifeq ($(CONFIG_NORTHBRIDGE_AMD_GX2),y)

ramstage-y += northbridge.c
ramstage-y += northbridgeinit.c
ramstage-y += grphinit.c

endif
