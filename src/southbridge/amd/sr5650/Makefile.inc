ifeq ($(CONFIG_SOUTHBRIDGE_AMD_SR5650),y)

ramstage-y += sr5650.c
ramstage-y += pcie.c
ramstage-y += ht.c

romstage-y += early_setup.c

endif
