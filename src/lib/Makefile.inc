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

bootblock-y += cbfs.c
bootblock-y += memchr.c
bootblock-y += memcmp.c

romstage-y += memchr.c
romstage-y += memcmp.c
rmodules-y += memcmp.c
romstage-y += cbfs.c
romstage-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c
#romstage-y += lzmadecode.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c

ifeq ($(CONFIG_EARLY_CBMEM_INIT),y)
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
endif

romstage-y += compute_ip_checksum.c
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += gcc.c

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
ramstage-y += version.c
ramstage-y += cbfs.c
ramstage-y += lzma.c
#ramstage-y += lzmadecode.c
ramstage-y += stack.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
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

# The CBMEM implementations are chosen based on CONFIG_DYNAMIC_CBMEM.
ifeq ($(CONFIG_DYNAMIC_CBMEM),y)
ramstage-y += dynamic_cbmem.c
romstage-y += dynamic_cbmem.c
else
ramstage-y += cbmem.c
romstage-$(CONFIG_EARLY_CBMEM_INIT) += cbmem.c
romstage-$(CONFIG_BROKEN_CAR_MIGRATE) += cbmem.c
endif # CONFIG_DYNAMIC_CBMEM
ramstage-y += cbmem_info.c

ramstage-y += hexdump.c
romstage-y += hexdump.c

ramstage-$(CONFIG_REG_SCRIPT) += reg_script.c

romstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += ramstage_cache.c

smm-y += cbfs.c memcmp.c
smm-y += gcc.c

$(obj)/lib/version.ramstage.o : $(obj)/build.h

romstage-y += bootmode.c
ramstage-y += bootmode.c

ifeq ($(CONFIG_RELOCATABLE_MODULES),y)
ramstage-y += rmodule.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += rmodule.c

RMODULE_LDSCRIPT := $(src)/lib/rmodule.ld
RMODULE_LDFLAGS  := -nostartfiles -Wl,--emit-relocs -Wl,-z,defs -Wl,-Bsymbolic -Wl,-T,$(RMODULE_LDSCRIPT)

# rmodule_link_rules is a function that should be called with:
# (1) the object name to link
# (2) the dependencies
# (3) heap size of the relocatable module
# It will create the necessary Make rules to create a rmodule. The resulting
# rmdoule is named $(1).rmod
define rmodule_link
$(strip $(1)): $(strip $(2)) $$(RMODULE_LDSCRIPT) $$(obj)/ldoptions $$(RMODTOOL)
	$$(CC_rmodules) $$(CFLAGS_rmodules) $$(RMODULE_LDFLAGS) -Wl,--defsym=__heap_size=$(strip $(3)) -o $$@ -Wl,--start-group $(strip $(2)) $$(LIBGCC_FILE_NAME_rmodules) -Wl,--end-group
	$$(NM_rmodules) -n $$@ > $$(basename $$@).map

$(strip $(1)).rmod: $(strip $(1))
	$$(RMODTOOL) -i $$^ -o $$@
endef

endif

