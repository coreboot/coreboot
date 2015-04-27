ifeq ($(CONFIG_SOUTHBRIDGE_AMD_RS690),y)

ramstage-y += rs690.c
ramstage-y += cmn.c
ramstage-y += pcie.c
ramstage-y += ht.c
ramstage-y += gfx.c

endif
