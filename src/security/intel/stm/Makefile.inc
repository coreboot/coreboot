
# put the stm where it can be found

cbfs-files-$(CONFIG_STM) += stm.bin
stm.bin-file := $(CONFIG_STM_BINARY_FILE)
stm.bin-type := raw

ramstage-$(CONFIG_STM) += SmmStm.c
ramstage-$(CONFIG_STM) += StmPlatformSmm.c
ramstage-$(CONFIG_STM) += StmPlatformResource.c
