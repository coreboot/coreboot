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

ramstage-$(CONFIG_BACKUP_DEFAULT_SMM_REGION) += backup_default_smm.c

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call create_class_compiler,smm,x86_32))
$(eval $(call create_class_compiler,smmstub,x86_32))
else
$(eval $(call create_class_compiler,smm,x86_64))
$(eval $(call create_class_compiler,smmstub,x86_64))
endif

smmstub-generic-ccopts += -D__SMM__
smm-generic-ccopts += -D__SMM__
smm-c-deps:=$$(OPTION_TABLE_H)

$(obj)/cpu/x86/smm/smm.o: $$(smm-objs) $(COMPILER_RT_smm)
	$(LD_smm) -nostdlib -r -o $@ $(COMPILER_RT_FLAGS_smm) --whole-archive --start-group $(smm-objs) --no-whole-archive $(COMPILER_RT_smm) --end-group

# change to the target path because objcopy will use the path name in its
# ELF symbol names.
$(obj)/cpu/x86/smm/smm.ramstage.manual: $(obj)/cpu/x86/smm/smm
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $@); $(OBJCOPY_smm) -I binary $(notdir $<) $(target-objcopy) $(notdir $@)

ifeq ($(CONFIG_SMM_TSEG),y)

smmstub-y += smm_stub.S

smm-y += smm_module_handler.c

ramstage-y += smm_module_loader.c

ramstage-srcs += $(obj)/cpu/x86/smm/smm.manual
ramstage-srcs += $(obj)/cpu/x86/smm/smmstub.manual

# SMM Stub Module. The stub is used as a trampoline for relocation and normal
# SMM handling.
$(obj)/cpu/x86/smm/smmstub.o: $$(smmstub-objs)
	$(CC_smmstub) $(CFLAGS_smmstub) -nostdlib -r -o $@ $^

# Link the SMM stub module with a 0-byte heap.
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smmstub.elf, $(obj)/cpu/x86/smm/smmstub.o, 0,x86_32))
else
$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smmstub.elf, $(obj)/cpu/x86/smm/smmstub.o, 0,x86_64))
endif

$(obj)/cpu/x86/smm/smmstub: $(obj)/cpu/x86/smm/smmstub.elf.rmod
	$(OBJCOPY_smmstub) -O binary $< $@

$(obj)/cpu/x86/smm/smmstub.ramstage.manual: $(obj)/cpu/x86/smm/smmstub
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $@); $(OBJCOPY_smmstub) -I binary $(notdir $<) $(target-objcopy) $(notdir $@)

# C-based SMM handler.

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smm.elf, $(obj)/cpu/x86/smm/smm.o, $(CONFIG_SMM_MODULE_HEAP_SIZE),x86_32))
else
$(eval $(call rmodule_link,$(obj)/cpu/x86/smm/smm.elf, $(obj)/cpu/x86/smm/smm.o, $(CONFIG_SMM_MODULE_HEAP_SIZE),x86_64))
endif

$(obj)/cpu/x86/smm/smm: $(obj)/cpu/x86/smm/smm.elf.rmod
	$(OBJCOPY_smm) -O binary $< $@

else # CONFIG_SMM_TSEG

$(obj)/cpu/x86/smm/smm: $(obj)/cpu/x86/smm/smm.o $(src)/cpu/x86/smm/smm.ld
	$(LD_smm) $(LDFLAGS_smm) -o $(obj)/cpu/x86/smm/smm.elf -T $(src)/cpu/x86/smm/smm.ld $(obj)/cpu/x86/smm/smm.o
	$(NM_smm) -n $(obj)/cpu/x86/smm/smm.elf | sort > $(obj)/cpu/x86/smm/smm.map
	$(OBJCOPY_smm) -O binary $(obj)/cpu/x86/smm/smm.elf $@

ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ramstage-srcs += $(obj)/cpu/x86/smm/smm.manual
endif

smm-y += smmhandler.S
smm-y += smihandler.c

endif # CONFIG_SMM_TSEG
