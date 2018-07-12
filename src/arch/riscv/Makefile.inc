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

################################################################################
## RISC-V specific options
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_RISCV),y)
check-ramstage-overlap-regions += stack
endif

riscv_flags = -I$(src)/arch/riscv/ -mcmodel=$(CONFIG_RISCV_CODEMODEL) -march=$(CONFIG_RISCV_ARCH) -mabi=$(CONFIG_RISCV_ABI)

riscv_asm_flags = -march=$(CONFIG_RISCV_ARCH) -mabi=$(CONFIG_RISCV_ABI)

COMPILER_RT_bootblock = $(shell $(GCC_bootblock) $(riscv_flags) -print-libgcc-file-name)

COMPILER_RT_romstage  = $(shell  $(GCC_romstage) $(riscv_flags) -print-libgcc-file-name)

COMPILER_RT_ramstage  = $(shell  $(GCC_ramstage) $(riscv_flags) -print-libgcc-file-name)

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_RISCV),y)

bootblock-y += id.S
$(call src-to-obj,bootblock,$(dir)/id.S): $(obj)/build.h

bootblock-y = bootblock.S stages.c
bootblock-y += trap_util.S
bootblock-y += trap_handler.c
bootblock-y += mcall.c
bootblock-y += virtual_memory.c
bootblock-y += boot.c
bootblock-y += misc.c
bootblock-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

$(objcbfs)/bootblock.debug: $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) \
		-T $(call src-to-obj,bootblock,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) \
		$(LIBGCC_FILE_NAME_bootblock) --end-group $(COMPILER_RT_bootblock)

bootblock-c-ccopts += $(riscv_flags)
bootblock-S-ccopts += $(riscv_asm_flags)

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_RISCV),y)

romstage-y += boot.c
romstage-y += stages.c
romstage-y += misc.c
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
	$(LD_romstage) $(LDFLAGS_romstage) -o $@ -L$(obj) -T $(call src-to-obj,romstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group $(COMPILER_RT_romstage)

romstage-c-ccopts += $(riscv_flags)
romstage-S-ccopts += $(riscv_asm_flags)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_RISCV),y)

ramstage-y =
ramstage-y += virtual_memory.c
ramstage-y += stages.c
ramstage-y += misc.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += payload.S
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
	$(LD_ramstage) $(LDFLAGS_ramstage) -o $@ -L$(obj) -T $(call src-to-obj,ramstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group $(COMPILER_RT_ramstage)

ramstage-c-ccopts += $(riscv_flags)
ramstage-S-ccopts += $(riscv_asm_flags)

endif
