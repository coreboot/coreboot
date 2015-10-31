#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
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

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_NEHALEM),y)

ramstage-y += ram_calc.c
ramstage-y += northbridge.c
ramstage-y += smi.c
ramstage-y += gma.c

ramstage-y += acpi.c
ramstage-y += ../sandybridge/mrccache.c

romstage-y += ram_calc.c
romstage-y += raminit.c
romstage-y += early_init.c
romstage-y += ../sandybridge/mrccache.c
romstage-y += ../../../arch/x86/walkcbfs.S

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

$(obj)/mrc.cache:
	dd if=/dev/zero count=1 \
	bs=$(shell printf "%d" $(CONFIG_TRAINING_CACHE_SIZE) ) | \
	tr '\000' '\377' > $@

cbfs-files-y += mrc.cache
mrc.cache-file := $(obj)/mrc.cache
mrc.cache-align := 0x10000
mrc.cache-type := mrc_cache

endif
