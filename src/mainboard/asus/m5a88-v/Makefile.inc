
#SB800 CIMx share AGESA V5 lib code
ifneq ($(CONFIG_CPU_AMD_AGESA),y)
 AGESA_ROOT ?= src/vendorcode/amd/agesa/f14
 romstage-y += ../../../../$(AGESA_ROOT)/Lib/amdlib.c
 ramstage-y += ../../../../$(AGESA_ROOT)/Lib/amdlib.c

 AGESA_INC := -I$(AGESA_ROOT)/ \
	      -I$(AGESA_ROOT)/Include \
	      -I$(AGESA_ROOT)/Proc/IDS/ \
	      -I$(AGESA_ROOT)/Proc/CPU/ \
	      -I$(AGESA_ROOT)/Proc/CPU/Family

 CFLAGS += $(AGESA_INC)
endif
