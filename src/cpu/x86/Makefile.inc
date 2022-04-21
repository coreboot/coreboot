subdirs-y += lapic
subdirs-y += mtrr
subdirs-y += pae
subdirs-$(CONFIG_HAVE_SMI_HANDLER) += smm
subdirs-$(CONFIG_UDELAY_TSC) += tsc
# Use ARCH_BOOTBLOCK_X86_64 as a proxy for knowing if 64bit is going to be used
subdirs-$(CONFIG_ARCH_BOOTBLOCK_X86_64) += 64bit

subdirs-$(CONFIG_PARALLEL_MP) += name
ramstage-$(CONFIG_PARALLEL_MP) += mp_init.c

ramstage-y += backup_default_smm.c
ramstage-y += smi_trigger.c
smm-y += smi_trigger.c

subdirs-$(CONFIG_CPU_INTEL_COMMON_SMM) += ../intel/smm

bootblock-y += entry32.S
bootblock-y += entry16.S
bootblock-y += reset16.S

additional-dirs += $(obj)/cpu/x86

SIPI_ELF=$(obj)/cpu/x86/sipi_vector.elf
SIPI_RMOD=$(SIPI_ELF).rmod
SIPI_BIN=$(SIPI_ELF:.elf=)
SIPI_DOTO=$(SIPI_ELF:.elf=.o)

ifeq ($(CONFIG_HAVE_RAMSTAGE),y)
TARGET_STAGE=ramstage
else ifeq ($(CONFIG_RAMPAYLOAD),y)
TARGET_STAGE=postcar
else
$(error Halting the build due to unknown TARGET_STAGE select)
endif

ifeq ($(CONFIG_PARALLEL_MP),y)
$(TARGET_STAGE)-srcs += $(SIPI_BIN).manual
endif
rmodules_$(ARCH-$(TARGET_STAGE)-y)-$(CONFIG_PARALLEL_MP) += sipi_vector.S

$(SIPI_DOTO): $(call src-to-obj,rmodules_$(ARCH-$(TARGET_STAGE)-y),src/cpu/x86/sipi_vector.S)
	$(LD_rmodules_$(ARCH-$(TARGET_STAGE)-y)) -nostdlib -r -o $@ $^

$(eval $(call rmodule_link,$(SIPI_ELF), $(SIPI_DOTO), 0,$(ARCH-$(TARGET_STAGE)-y)))

$(SIPI_BIN): $(SIPI_RMOD)
	$(OBJCOPY_$(TARGET_STAGE)) -O binary $< $@

$(call src-to-obj,$(TARGET_STAGE),$(SIPI_BIN).manual): $(SIPI_BIN)
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(dir $<); $(OBJCOPY_rmodules_$(ARCH-$(TARGET_STAGE)-y)) -I binary $(notdir $<) $(target-objcopy) $(abspath $@)
