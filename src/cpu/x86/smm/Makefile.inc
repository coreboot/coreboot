##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2010 coresystems GmbH
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

ifeq ($(CONFIG_SMM_MODULES),y)
smmstub-y += smm_stub.S
smmstub-y += smm_module_header.c

smm-y += smiutil.c
smm-y += smm_module_header.c
smm-y += smm_module_handler.c

ramstage-y += smm_module_loader.c

ramstage-srcs += $(obj)/cpu/x86/smm/smm
ramstage-srcs += $(obj)/cpu/x86/smm/smmstub

# SMM Stub Module. The stub is used as a trampoline for relocation and normal
# SMM handling.
$(obj)/cpu/x86/smm/smmstub.o: $$(smmstub-objs)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

# Link the SMM stub module with a 0-byte heap.
$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smmstub.elf, $(obj)/cpu/x86/smm/smmstub.o, 0))

$(obj)/cpu/x86/smm/smmstub: $(obj)/cpu/x86/smm/smmstub.elf
	$(OBJCOPY) -O binary $< $@

$(obj)/cpu/x86/smm/smmstub.ramstage.o: $(obj)/cpu/x86/smm/smmstub
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $@); $(OBJCOPY) -I binary $(notdir $<) -O elf32-i386 -B i386 $(notdir $@)

# C-based SMM handler.

$(obj)/cpu/x86/smm/smm.o: $$(smm-objs) $(LIBGCC_FILE_NAME)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(smm-objs) $(LIBGCC_FILE_NAME) -Wl,--end-group


$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smm.elf, $(obj)/cpu/x86/smm/smm.o, $(CONFIG_SMM_MODULE_HEAP_SIZE)))

$(obj)/cpu/x86/smm/smm: $(obj)/cpu/x86/smm/smm.elf
	$(OBJCOPY) -O binary $< $@

$(obj)/cpu/x86/smm/smm.ramstage.o: $(obj)/cpu/x86/smm/smm
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $@); $(OBJCOPY) -I binary $(notdir $<) -O elf32-i386 -B i386 $(notdir $@)

else # CONFIG_SMM_MODULES

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.S
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ramstage-srcs += $(obj)/cpu/x86/smm/smm_wrap
endif

# Use TSEG specific entry point and linker script
ifeq ($(CONFIG_SMM_TSEG),y)
smm-y += smmhandler_tseg.S
SMM_LDFLAGS  := $(LDFLAGS) -pie
SMM_LDSCRIPT := smm_tseg.ld
else
smm-y += smmhandler.S
SMM_LDFLAGS  := $(LDFLAGFS)
SMM_LDSCRIPT := smm.ld
endif

smm-y += smihandler.c
smm-y += smiutil.c

$(obj)/cpu/x86/smm/smm.o: $$(smm-objs) $(LIBGCC_FILE_NAME)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(smm-objs) $(LIBGCC_FILE_NAME) -Wl,--end-group

$(obj)/cpu/x86/smm/smm_wrap: $(obj)/cpu/x86/smm/smm.o $(src)/cpu/x86/smm/$(SMM_LDSCRIPT) $(obj)/ldoptions
	$(CC) $(SMM_LDFLAGS) -nostdlib -nostartfiles -static -o $(obj)/cpu/x86/smm/smm.elf -T $(src)/cpu/x86/smm/$(SMM_LDSCRIPT) $(obj)/cpu/x86/smm/smm.o
	$(NM) -n $(obj)/cpu/x86/smm/smm.elf | sort > $(obj)/cpu/x86/smm/smm.map
	$(OBJCOPY) -O binary $(obj)/cpu/x86/smm/smm.elf $(obj)/cpu/x86/smm/smm

# change to the target path because objcopy will use the path name in its
# ELF symbol names.
$(obj)/cpu/x86/smm/smm_wrap.ramstage.o: $(obj)/cpu/x86/smm/smm_wrap
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(obj)/cpu/x86/smm; $(OBJCOPY) -I binary smm -O elf32-i386 -B i386 smm_wrap.ramstage.o

endif # CONFIG_SMM_MODULES
