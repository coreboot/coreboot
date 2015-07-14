ifeq ($(CONFIG_SOUTHBRIDGE_AMD_RS780),y)

ramstage-y += rs780.c
ramstage-y += cmn.c
ramstage-y += pcie.c
ramstage-y += ht.c
ramstage-y += gfx.c

endif
