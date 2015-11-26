################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 The ChromiumOS Authors
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
################################################################################

riscv_flags = -I$(src)/arch/riscv/

riscv_asm_flags =

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_RISCV),y)

bootblock-y = bootblock.S stages.c
bootblock-y += trap_util.S
bootblock-y += trap_handler.c
bootblock-y += virtual_memory.c
bootblock-y += boot.c
bootblock-y += rom_media.c
bootblock-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

$(objcbfs)/bootblock.debug: $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) \
		-T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.bootblock.ld --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) \
		$(LIBGCC_FILE_NAME_bootblock) --end-group $(COMPILER_RT_bootblock)

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_RISCV),y)

romstage-y += boot.c
romstage-y += stages.c
romstage-y += rom_media.c
romstage-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

# Build the romstage

$(objcbfs)/romstage.debug: $$(romstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) $(LDFLAGS_romstage) -o $@ -L$(obj) -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.romstage.ld --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group $(COMPILER_RT_romstage)

romstage-c-ccopts += $(riscv_flags)
romstage-S-ccopts += $(riscv_asm_flags)

CBFSTOOL_PRE1_OPTS = -v -m riscv -s $(CONFIG_CBFS_SIZE)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_RISCV),y)

ramstage-y =
ramstage-y += trap_handler.c
ramstage-y += virtual_memory.c
ramstage-y += rom_media.c
ramstage-y += stages.c
ramstage-y += misc.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

$(eval $(call create_class_compiler,rmodules,riscv))

ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(LDFLAGS_ramstage) -o $@ -L$(obj) -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.ramstage.ld --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group $(COMPILER_RT_ramstage)

ramstage-c-ccopts += $(riscv_flags)
ramstage-S-ccopts += $(riscv_asm_flags)

endif
