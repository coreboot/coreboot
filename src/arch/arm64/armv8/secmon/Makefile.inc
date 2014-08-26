################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##
################################################################################

$(eval $(call create_class_compiler,secmon,arm64))

SECMON_DIR=$(obj)/arch/arm64/armv8/secmon
SECMON_SRC=$(SECMON_DIR)/secmon
SECMON_OBJ=$(SECMON_DIR)/secmon.o
SECMON_ELF=$(SECMON_DIR)/secmon.elf
SECMON_RMOD=$(SECMON_DIR)/secmon.elf.rmod
SECMON_RAMSTAGE=$(SECMON_DIR)/secmon.ramstage.o

secmon-c-ccopts += -I$(src)/arch/arm64/include/armv8/ -include $(src)/include/kconfig.h -D__SECMON__
secmon-S-ccopts += -I$(src)/arch/arm64/include/armv8/ -include $(src)/include/kconfig.h -D__SECMON__

secmon-y += secmon_init.c
secmon-y += ../../transition_asm.S ../../transition.c

ramstage-srcs += $(SECMON_SRC)

$(SECMON_OBJ):  $$(secmon-objs)
		$(CC_secmon) $(LDFLAGS) -nostdlib -r -o $@ $^

$(eval $(call rmodule_link,$(SECMON_ELF), $(SECMON_OBJ), 0,arm64))

$(SECMON_SRC): $(SECMON_RMOD)
	       $(OBJCOPY_secmon) -O binary $< $@

$(SECMON_RAMSTAGE): $(SECMON_SRC)
		    @printf "    OBJCOPY $(subst $(obj)/,,$(@))\n"
		    cd $(dir $@); $(OBJCOPY_secmon) -I binary $(notdir $<) -O elf64-littleaarch64 -B aarch64 $(notdir $@)