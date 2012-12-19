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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

ifneq ($(CONFIG_HAVE_ARCH_MEMSET),y)
romstage-y += memset.c
endif
romstage-y += memchr.c
ifneq ($(CONFIG_HAVE_ARCH_MEMCPY),y)
romstage-y += memcpy.c
endif
romstage-y += memcmp.c
romstage-y += memmove.c
#romstage-y += cbfs.c
#romstage-y += lzma.c
#romstage-y += lzmadecode.c
#romstage-y += compute_ip_checksum.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c
romstage-$(CONFIG_HAVE_ACPI_RESUME) += cbmem.c
romstage-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c
romstage-$(CONFIG_CONSOLE_SERIAL8250MEM) += uart8250mem.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
romstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c
romstage-$(CONFIG_USBDEBUG) += usbdebug.c
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-y += hardwaremain.c
ramstage-y += selfboot.c
ifneq ($(CONFIG_HAVE_ARCH_MEMSET),y)
ramstage-y += memset.c
endif
ramstage-y += memchr.c
ifneq ($(CONFIG_HAVE_ARCH_MEMCPY),y)
ramstage-y += memcpy.c
endif
ramstage-y += memcmp.c
ramstage-y += memmove.c
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
ramstage-$(CONFIG_ARCH_X86) += gcc.c
ramstage-y += clog2.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c
ramstage-$(CONFIG_CONSOLE_SERIAL8250MEM) += uart8250mem.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_USBDEBUG) += usbdebug.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c
ramstage-$(CONFIG_TRACE) += trace.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c

ifneq ($(CONFIG_HAVE_ARCH_MEMSET),y)
smm-y += memset.c
endif
ifneq ($(CONFIG_HAVE_ARCH_MEMCPY),y)
smm-y += memcpy.c
endif
smm-y += cbfs.c memcmp.c
smm-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c
smm-$(CONFIG_CONSOLE_SERIAL8250MEM) += uart8250mem.c
smm-$(CONFIG_USBDEBUG) += usbdebug.c

$(obj)/lib/version.ramstage.o : $(obj)/build.h

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
OPTION_TABLE_H:=$(obj)/option_table.h
endif

$(obj)/lib/uart8250mem.smm.o : $(OPTION_TABLE_H)
$(obj)/lib/uart8250.smm.o : $(OPTION_TABLE_H)

