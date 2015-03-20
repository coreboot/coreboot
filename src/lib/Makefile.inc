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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
subdirs-y += loaders

bootblock-y += prog_ops.c
bootblock-y += cbfs.c cbfs_core.c
bootblock-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c

bootblock-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
bootblock-y += memchr.c
bootblock-y += memcmp.c

verstage-y += prog_ops.c
verstage-y += delay.c
verstage-y += cbfs.c
verstage-y += memcmp.c
verstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
verstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
verstage-y += tlcl.c
verstage-$(CONFIG_GENERIC_UDELAY) += timer.c

romstage-y += prog_ops.c
romstage-y += memchr.c
romstage-y += memcmp.c
$(foreach arch,$(ARCH_SUPPORTED),\
	    $(eval rmodules_$(arch)-y += memcmp.c))

romstage-$(CONFIG_I2C_TPM) += delay.c
romstage-y += cbfs.c cbfs_core.c
romstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
romstage-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c lzmadecode.c
romstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
ramstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c

ifeq ($(CONFIG_EARLY_CBMEM_INIT),y)
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
endif

romstage-y += compute_ip_checksum.c
ifeq ($(CONFIG_COMPILER_GCC),y)
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += gcc.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
endif

ramstage-y += prog_ops.c
ramstage-y += hardwaremain.c
ramstage-y += selfboot.c
ramstage-y += coreboot_table.c
ramstage-y += bootmem.c
ramstage-y += memchr.c
ramstage-y += memcmp.c
ramstage-y += malloc.c
smm-$(CONFIG_SMM_TSEG) += malloc.c
ramstage-y += delay.c
ramstage-y += fallback_boot.c
ramstage-y += compute_ip_checksum.c
ramstage-y += cbfs.c cbfs_core.c
ramstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
ramstage-y += lzma.c lzmadecode.c
ramstage-y += stack.c
ramstage-y += clog2.c
romstage-y += clog2.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c
ramstage-$(CONFIG_TRACE) += trace.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
ramstage-$(CONFIG_COVERAGE) += libgcov.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += edid.c
ramstage-y += memrange.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_TIMER_QUEUE) += timer_queue.c
ramstage-$(CONFIG_TERTIARY_BOARD_ID) += tristate_gpios.c

romstage-y += cbmem_common.c dynamic_cbmem.c
ramstage-y += cbmem_common.c dynamic_cbmem.c

ramstage-y += hexdump.c
romstage-y += hexdump.c

romstage-$(CONFIG_REG_SCRIPT) += reg_script.c
ramstage-$(CONFIG_REG_SCRIPT) += reg_script.c

romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += ramstage_cache.c

smm-y += cbfs.c cbfs_core.c memcmp.c
smm-$(CONFIG_COMPILER_GCC) += gcc.c

bootblock-y += version.c
romstage-y += version.c
ramstage-y += version.c
smm-y += version.c

$(obj)/lib/version.bootblock.o : $(obj)/build.h
$(obj)/lib/version.romstage.o : $(obj)/build.h
$(obj)/lib/version.ramstage.o : $(obj)/build.h
$(obj)/lib/version.smm.o : $(obj)/build.h

romstage-y += bootmode.c
ramstage-y += bootmode.c

bootblock-y += halt.c
romstage-y += halt.c
ramstage-y += halt.c
smm-y += halt.c
secmon-y += halt.c

ifeq ($(CONFIG_RELOCATABLE_MODULES),y)
ramstage-y += rmodule.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += rmodule.c

RMODULE_LDSCRIPT := $(src)/lib/rmodule.ld
RMODULE_LDFLAGS  := -nostartfiles --gc-sections --emit-relocs -z defs -Bsymbolic -T$(RMODULE_LDSCRIPT)

# rmodule_link_rules is a function that should be called with:
# (1) the object name to link
# (2) the dependencies
# (3) heap size of the relocatable module
# (4) arch for which the rmodules are to be linked
# It will create the necessary Make rules to create a rmodule. The resulting
# rmdoule is named $(1).rmod
define rmodule_link
$(strip $(1)): $(strip $(2)) $$(RMODULE_LDSCRIPT) $$(obj)/ldoptions $$(RMODTOOL)
	$$(LD_rmodules_$(4)) $$(RMODULE_LDFLAGS) --defsym=__heap_size=$(strip $(3)) -o $$@ --start-group $(strip $(2)) $$(COMPILER_RT_rmodules_$(4)) --end-group
	$$(NM_rmodules_$(4)) -n $$@ > $$(basename $$@).map

$(strip $(1)).rmod: $(strip $(1))
	$$(RMODTOOL) -i $$^ -o $$@
endef

endif
