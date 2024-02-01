## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_AMD_OPENSIL),y)

ifeq ($(CONFIG_SOC_AMD_OPENSIL_STUB),y)

subdirs-y += stub

else # CONFIG_SOC_AMD_OPENSIL_STUB

ifneq ($(CONFIG_ARCH_RAMSTAGE_X86_32)$(CONFIG_ARCH_RAMSTAGE_X86_64),y)
    $(error OpenSIL can only be built for either x86 or x86_64)
endif

opensil_dir := $(call strip_quotes,$(CONFIG_AMD_OPENSIL_PATH))

subdirs-$(CONFIG_SOC_AMD_OPENSIL_GENOA_POC) += genoa_poc

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
cpu_family_string="x86"
cpu_string="i686"
opensil_target_name=AMDopensil32
is_32bit="true"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
clang_args=\'-m32\'\, \'-fno-pic\'\,
endif
endif

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_64),y)
cpu_family_string="x86_64"
cpu_string="x86_64"
opensil_target_name=AMDopensil64
is_32bit="false"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
clang_args=\'-fno-pic\'\,
endif
endif

ifeq ($(CONFIG_OPENSIL_DEBUG_OUTPUT),y)
sil_debug_enable="true"
else
sil_debug_enable="false"
endif

# check if $(obj) is an absolute or a relative path
ifeq ($(shell printf %.1s "$(obj)"),/)
OBJPATH = $(obj)
else
OBJPATH = $(top)/$(obj)
endif

PYTHON?=python

OPENSIL_CONFIG=opensil_config

bios_base=$(shell printf "%x" $(call int-subtract, $(CONFIG_ROMSTAGE_ADDR) $(CONFIG_C_ENV_BOOTBLOCK_SIZE)))
bios_size=$(CONFIG_C_ENV_BOOTBLOCK_SIZE)

$(OBJPATH)/$(OPENSIL_CONFIG): $(opensil_dir)/../opensil_config.template
	sed -e "s,##APOB_BASE##,$(CONFIG_PSP_APOB_DRAM_ADDRESS)," \
	    -e "s,##BIOS_ENTRY_BASE##,$(bios_base)," \
	    -e "s,##BIOS_ENTRY_SIZE##,$(bios_size) ," \
		$< > $@

$(OBJPATH)/$(OPENSIL_CONFIG).h: $(OBJPATH)/$(OPENSIL_CONFIG) $(obj)/config.h $(objutil)/kconfig/conf
	cd $(opensil_dir); KCONFIG_CONFIG=$(OBJPATH)/$(OPENSIL_CONFIG) KCONFIG_AUTOHEADER=$@ $(PYTHON) util/kconfig/lib/genconfig.py Kconfig

# meson handles ccache on its own
OPENSIL_COMPILER=$(filter-out $(CCACHE), $(CC_ramstage))

$(OBJPATH)/meson_crosscompile: $(opensil_dir)/../meson_cross.template  $(obj)/config.h $(OBJPATH)/$(OPENSIL_CONFIG).h
	sed -e "s,##COMPILER##,$(OPENSIL_COMPILER)," \
	    -e "s,##AR##,$(AR_ramstage)," \
	    -e "s,##NASM##,$(NASM)," \
	    -e "s,##COREBOOT_DIR##,$(top)," \
	    -e "s,##OPENSIL_DIR##,$(opensil_dir)," \
	    -e "s,##OBJPATH##,$(OBJPATH)," \
	    -e "s,##CPU_FAMILY##,$(cpu_family_string)," \
	    -e "s,##CPU##,$(cpu_string)," \
	    -e "s,##IS32BIT##,$(is_32bit)," \
	    -e "s,##SIL_DEBUG_ENABLE##,$(sil_debug_enable)," \
	    -e "s,##CLANG_ARGS##,$(clang_args)," \
		$< > $@

# Don't set a meson buildtype as opensil is broken when compiler optimizations are enabled
$(OBJPATH)/opensil:  $(OBJPATH)/meson_crosscompile $(obj)/config.h
	cd $(opensil_dir); meson setup --cross-file $< $(OBJPATH)/opensil -Db_staticpic=false \
		-DPlatKcfgDir=$(OBJPATH) -DPlatKcfg=$(OPENSIL_CONFIG)

$(OBJPATH)/opensil/lib$(opensil_target_name).a: $(OBJPATH)/opensil
	meson compile -C $(OBJPATH)/opensil $(opensil_target_name)

$(OBJPATH)/opensil.a: $(OBJPATH)/opensil/lib$(opensil_target_name).a
	cp $(OBJPATH)/opensil/lib$(opensil_target_name).a $@

romstage-libs += $(OBJPATH)/opensil.a
ramstage-libs += $(OBJPATH)/opensil.a

endif # CONFIG_SOC_AMD_OPENSIL_STUB

endif # CONFIG_SOC_AMD_OPENSIL
