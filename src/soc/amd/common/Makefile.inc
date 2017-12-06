ifeq ($(CONFIG_SOC_AMD_COMMON),y)

CPPFLAGS_common += -I$(src)/soc/amd/common

bootblock-y += agesawrapper.c
bootblock-y += def_callouts.c
bootblock-y += heapmanager.c

romstage-y += agesawrapper.c
romstage-y += def_callouts.c
romstage-y += heapmanager.c

ramstage-y += agesawrapper.c
ramstage-y += amd_late_init.c
ramstage-y += def_callouts.c
ramstage-y += heapmanager.c

subdirs-$(CONFIG_SOC_AMD_COMMON_BLOCK) += block

endif
