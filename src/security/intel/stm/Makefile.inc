
# put the stm where it can be found

cbfs-files-$(CONFIG_STM) += stm.bin
stm.bin-file := $(CONFIG_STM_BINARY_FILE)
stm.bin-type := raw

ramstage-$(CONFIG_STM) += SmmStm.c
ramstage-$(CONFIG_STM) += StmPlatformSmm.c
ramstage-$(CONFIG_STM) += StmPlatformResource.c

3rdparty/stm/Stm/build/StmPkg/Core/stm.bin: $(obj)/config.h
	$(MAKE) -C src/security/intel/stm \
		CONFIG_STM_TTYS0_BASE=$(CONFIG_STM_TTYS0_BASE) \
		CONFIG_STM_HEAPSIZE=$(CONFIG_STM_HEAPSIZE) \
		CONFIG_STM_CONSOLE_DEBUG=$(CONFIG_STM_CONSOLE_DEBUG) \
		CONFIG_STM_CONSOLE_RELEASE=$(CONFIG_STM_CONSOLE_RELEASE) \
		CONFIG_STM_GIT_BRANCH=$(CONFIG_STM_GIT_BRANCH) \
		CONFIG_STM_STMPE_ENABLED=$(CONFIG_STM_STMPE_ENABLED) \
		CONFIG_STM_CBMEM_CONSOLE=$(CONFIG_STM_CBMEM_CONSOLE)
