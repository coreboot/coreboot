ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)
romstage-$(CONFIG_CACHE_AS_RAM) += car.c
endif

subdirs-$(CONFIG_PARALLEL_MP) += name
ramstage-$(CONFIG_PARALLEL_MP) += mp_init.c
ramstage-$(CONFIG_MIRROR_PAYLOAD_TO_RAM_BEFORE_LOADING) += mirror_payload.c

SIPI_ELF=$(obj)/cpu/x86/sipi_vector.elf
SIPI_RMOD=$(SIPI_ELF).rmod
SIPI_BIN=$(SIPI_ELF:.elf=)
SIPI_DOTO=$(SIPI_ELF:.elf=.o)

ifeq ($(CONFIG_PARALLEL_MP),y)
ramstage-srcs += $(SIPI_BIN).manual
endif
rmodules_$(ARCH-ramstage-y)-$(CONFIG_PARALLEL_MP) += sipi_vector.S

$(SIPI_DOTO): $(dir $(SIPI_ELF))sipi_vector.rmodules_$(ARCH-ramstage-y).o
	$(CC_rmodules_$(ARCH-ramstage-y)) $(CFLAGS_rmodules_$(ARCH-ramstage-y)) -nostdlib -r -o $@ $^

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(SIPI_ELF), $(SIPI_DOTO), 0,x86_32))
else
$(eval $(call rmodule_link,$(SIPI_ELF), $(SIPI_DOTO), 0,x86_64))
endif

$(SIPI_BIN): $(SIPI_RMOD)
	$(OBJCOPY_ramstage) -O binary $< $@

$(SIPI_BIN).ramstage.manual: $(SIPI_BIN)
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
	cd $(dir $@); $(OBJCOPY_rmodules_$(ARCH-ramstage-y)) -I binary $(notdir $<) -O elf32-i386 -B i386 $(notdir $@)
else
	cd $(dir $@); $(OBJCOPY_rmodules_$(ARCH-ramstage-y)) -I binary $(notdir $<) -O elf64-x86_64 -B x86_64 $(notdir $@)
endif
