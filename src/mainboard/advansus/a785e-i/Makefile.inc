#romstage-y += reset.c #FIXME romstage have include test_rest.c
romstage-y += pmio.c

ramstage-y += reset.c
ramstage-y += pmio.c

#SB800 CIMx share AGESA V5 lib code
ifneq ($(CONFIG_AMD_AGESA),y)
 romstage-y += ../../../vendorcode/amd/agesa/Lib/amdlib.c
 ramstage-y += ../../../vendorcode/amd/agesa/Lib/amdlib.c

 AGESA_INC := -Isrc/vendorcode/amd/agesa/ \
	      -Isrc/vendorcode/amd/agesa/Include \
	      -Isrc/vendorcode/amd/agesa/Proc/IDS/ \
	      -Isrc/vendorcode/amd/agesa/Proc/CPU/ \
	      -Isrc/vendorcode/amd/agesa/Proc/CPU/Family

 CFLAGS += $(AGESA_INC)
endif
