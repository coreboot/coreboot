#
# This file is part of the coreboot project.
#
# Copyright (C) 2009 coresystems GmbH
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
subdirs-y += loaders


ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.c
endif

bootblock-y += prog_loaders.c
bootblock-y += prog_ops.c
bootblock-y += cbfs.c
bootblock-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
bootblock-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
bootblock-y += libgcc.c
bootblock-$(CONFIG_GENERIC_UDELAY) += timer.c

ifeq ($(CONFIG_EARLY_CBMEM_INIT),y)
bootblock-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
endif

bootblock-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
bootblock-$(CONFIG_I2C_TPM) += delay.c
bootblock-y += memchr.c
bootblock-y += memcmp.c
bootblock-y += boot_device.c
bootblock-y += fmap.c

verstage-y += prog_loaders.c
verstage-y += prog_ops.c
verstage-y += delay.c
verstage-y += cbfs.c
verstage-y += halt.c
verstage-y += fmap.c
verstage-y += libgcc.c
verstage-y += memcmp.c
verstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
verstage-y += boot_device.c
verstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
verstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c

ifeq ($(MOCK_TPM),1)
libverstage-y += mocked_tlcl.c
else
libverstage-y += tlcl.c
endif

verstage-$(CONFIG_GENERIC_UDELAY) += timer.c
verstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c

romstage-y += prog_loaders.c
romstage-y += prog_ops.c
romstage-y += memchr.c
romstage-y += memcmp.c
$(foreach arch,$(ARCH_SUPPORTED),\
	    $(eval rmodules_$(arch)-y += memcmp.c) \
	    $(eval rmodules_$(arch)-y += rmodule.ld))

romstage-y += fmap.c
romstage-$(CONFIG_I2C_TPM) += delay.c
romstage-y += cbfs.c
romstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
romstage-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c lzmadecode.c
romstage-y += libgcc.c
romstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
ramstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c
romstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
romstage-y += stack.c

ifeq ($(CONFIG_EARLY_CBMEM_INIT),y)
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
endif

romstage-y += compute_ip_checksum.c
ifeq ($(CONFIG_COMPILER_GCC),y)
verstage-$(CONFIG_ARCH_VERSTAGE_X86_32) += gcc.c
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += gcc.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
smm-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
endif

romstage-$(CONFIG_GENERIC_UDELAY) += timer.c

ramstage-y += prog_loaders.c
ramstage-y += prog_ops.c
ramstage-y += hardwaremain.c
ramstage-y += selfboot.c
ramstage-y += coreboot_table.c
ramstage-y += bootmem.c
ramstage-y += fmap.c
ramstage-y += memchr.c
ramstage-y += memcmp.c
ramstage-y += malloc.c
smm-$(CONFIG_SMM_TSEG) += malloc.c
ramstage-y += delay.c
ramstage-y += fallback_boot.c
ramstage-y += compute_ip_checksum.c
ramstage-y += cbfs.c
ramstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
ramstage-y += lzma.c lzmadecode.c
ramstage-y += stack.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c
ramstage-$(CONFIG_TRACE) += trace.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
ramstage-$(CONFIG_COVERAGE) += libgcov.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += edid.c
ramstage-y += memrange.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_TIMER_QUEUE) += timer_queue.c
ramstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
ramstage-$(CONFIG_GENERIC_UDELAY) += timer.c
ramstage-y += b64_decode.c

romstage-y += cbmem_common.c
romstage-y += imd_cbmem.c

ramstage-y += cbmem_common.c
ramstage-y += imd_cbmem.c

romstage-y += imd.c
ramstage-y += imd.c

ramstage-y += hexdump.c
romstage-y += hexdump.c

verstage-$(CONFIG_REG_SCRIPT) += reg_script.c
romstage-$(CONFIG_REG_SCRIPT) += reg_script.c
ramstage-$(CONFIG_REG_SCRIPT) += reg_script.c

ifeq ($(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM),y)
ramstage-y += ext_stage_cache.c
romstage-y += ext_stage_cache.c
else
ramstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += cbmem_stage_cache.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += cbmem_stage_cache.c
endif


romstage-y += boot_device.c
ramstage-y += boot_device.c

smm-y += boot_device.c
smm-y += fmap.c
smm-y += cbfs.c memcmp.c

bootblock-y += version.c
romstage-y += version.c
ramstage-y += version.c
smm-y += version.c
verstage-y += version.c

$(obj)/lib/version.bootblock.o : $(obj)/build.h
$(obj)/lib/version.romstage.o : $(obj)/build.h
$(obj)/lib/version.ramstage.o : $(obj)/build.h
$(obj)/lib/version.smm.o : $(obj)/build.h
$(obj)/lib/version.verstage.o : $(obj)/build.h

romstage-y += bootmode.c
ramstage-y += bootmode.c

bootblock-y += halt.c
romstage-y += halt.c
ramstage-y += halt.c
smm-y += halt.c

ifneq ($(CONFIG_ARCH_X86),y)
# X86 bootblock uses custom ldscripts that are all glued together,
# so we need to exclude it here or it would pick these up as well
bootblock-y += program.ld
endif

romstage-y += program.ld
ramstage-y += program.ld
verstage-y += program.ld

ifeq ($(CONFIG_RELOCATABLE_MODULES),y)
ramstage-y += rmodule.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += rmodule.c

RMODULE_LDFLAGS  := -z defs -Bsymbolic

# rmodule_link_rules is a function that should be called with:
# (1) the object name to link
# (2) the dependencies
# (3) heap size of the relocatable module
# (4) arch for which the rmodules are to be linked
# It will create the necessary Make rules to create a rmodule. The resulting
# rmdoule is named $(1).rmod
define rmodule_link
$(strip $(1)): $(strip $(2)) $$(COMPILER_RT_rmodules_$(4)) $(obj)/lib/rmodule.rmodules_$(4).ld | $$(RMODTOOL)
	$$(LD_rmodules_$(4)) $$(LDFLAGS_rmodules_$(4)) $(RMODULE_LDFLAGS) -T $(obj)/lib/rmodule.rmodules_$(4).ld --defsym=__heap_size=$(strip $(3)) -o $$@ --whole-archive --start-group $(filter-out %.ld,$(2)) --end-group
	$$(NM_rmodules_$(4)) -n $$@ > $$(basename $$@).map
endef

endif

$(objcbfs)/%.debug.rmod: $(objcbfs)/%.debug | $(RMODTOOL)
	$(RMODTOOL) -i $< -o $@

$(obj)/%.elf.rmod: $(obj)/%.elf | $(RMODTOOL)
	$(RMODTOOL) -i $< -o $@
