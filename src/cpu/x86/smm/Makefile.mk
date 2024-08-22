## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += smm_module_loader.c
ramstage-$(CONFIG_SMM_PCI_RESOURCE_STORE) += pci_resource_store.c

smm-$(CONFIG_SMM_PCI_RESOURCE_STORE) += pci_resource_store.c

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call create_class_compiler,smm,x86_32))
$(eval $(call create_class_compiler,smmstub,x86_32))
else
$(eval $(call create_class_compiler,smm,x86_64))
$(eval $(call create_class_compiler,smmstub,x86_64))
endif

smmstub-generic-ccopts += -D__SMM__
smm-generic-ccopts += -D__SMM__
smm-c-deps+=$$(OPTION_TABLE_H)

$(obj)/smm/smm.a: $$(smm-objs)
	$(AR_smm) rcsT $@.tmp $(filter-out %.ld, $(smm-objs))
	mv $@.tmp $@


# change to the target path because objcopy will use the path name in its
# ELF symbol names.
$(call src-to-obj,ramstage,$(obj)/cpu/x86/smm/smm.manual): $(obj)/smm/smm
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $<); $(OBJCOPY_smm) -I binary $(notdir $<) $(target-objcopy) $(abspath $@)

ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ramstage-srcs += $(obj)/cpu/x86/smm/smm.manual
endif

smm-y += save_state.c

ramstage-y += tseg_region.c
romstage-y += tseg_region.c
postcar-y += tseg_region.c

ifeq ($(CONFIG_PARALLEL_MP),y)

smmstub-y += smm_stub.S

smm-y += smm_module_handler.c

ramstage-srcs += $(obj)/cpu/x86/smm/smmstub.manual

# SMM Stub Module. The stub is used as a trampoline for relocation and normal
# SMM handling.
$(obj)/smmstub/smmstub.o: $$(smmstub-objs) $(COMPILER_RT_smmstub)
	$(LD_smmstub) -nostdlib -r -o $@ $(COMPILER_RT_FLAGS_smmstub) --whole-archive --start-group $(smmstub-objs) --no-whole-archive $(COMPILER_RT_smmstub) --end-group

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(obj)/smmstub/smmstub.elf, $(obj)/smmstub/smmstub.o,x86_32))
else
$(eval $(call rmodule_link,$(obj)/smmstub/smmstub.elf, $(obj)/smmstub/smmstub.o,x86_64))
endif

$(obj)/smmstub/smmstub: $(obj)/smmstub/smmstub.elf.rmod
	$(OBJCOPY_smmstub) -O binary $< $@

$(call src-to-obj,ramstage,$(obj)/cpu/x86/smm/smmstub.manual): $(obj)/smmstub/smmstub
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $<); $(OBJCOPY_smmstub) -I binary $(notdir $<) $(target-objcopy) $(abspath $@)

# C-based SMM handler.

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(obj)/smm/smm.elf, $(obj)/smm/smm.a,x86_32))
else
$(eval $(call rmodule_link,$(obj)/smm/smm.elf, $(obj)/smm/smm.a,x86_64))
endif

$(obj)/smm/smm: $(obj)/smm/smm.elf.rmod
	$(OBJCOPY_smm) -O binary $< $@

endif
