romstage-$(CONFIG_EARLY_CBMEM_INIT) += car.c
romstage-$(CONFIG_HAVE_ACPI_RESUME) += car.c

subdirs-$(CONFIG_PARALLEL_MP) += name
ramstage-$(CONFIG_PARALLEL_MP) += mp_init.c

SIPI_ELF=$(obj)/cpu/x86/sipi_vector.elf
SIPI_BIN=$(SIPI_ELF:.elf=)
SIPI_DOTO=$(SIPI_ELF:.elf=.o)

ifeq ($(CONFIG_PARALLEL_MP),y)
ramstage-srcs += $(SIPI_BIN)
endif
rmodules-$(CONFIG_PARALLEL_MP) += sipi_vector.S
rmodules-$(CONFIG_PARALLEL_MP) += sipi_header.c

$(SIPI_DOTO): $(dir $(SIPI_ELF))sipi_vector.rmodules.o $(dir $(SIPI_ELF))sipi_header.rmodules.o
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

$(eval $(call rmodule_link,$(SIPI_ELF), $(SIPI_ELF:.elf=.o), 0))

$(SIPI_BIN): $(SIPI_ELF)
	$(OBJCOPY) -O binary $< $@

$(SIPI_BIN).ramstage.o: $(SIPI_BIN)
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $@); $(OBJCOPY) -I binary $(notdir $<) -O elf32-i386 -B i386 $(notdir $@)
