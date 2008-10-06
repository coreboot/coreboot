##
## This file is part of the coreboot project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

			
STAGE0_MAINBOARD_SRC := $(src)/lib/clog2.c \
			$(src)/mainboard/$(MAINBOARDDIR)/stage1.c \
			$(src)/mainboard/$(MAINBOARDDIR)/option_table.c \
			$(src)/arch/x86/stage1_mtrr.c \
			$(src)/arch/x86/amd/model_fxx/dualcore_id.c \
			$(src)/arch/x86/amd/model_fxx/stage1.c \
			$(src)/northbridge/amd/k8/get_nodes.c \
			$(src)/northbridge/amd/k8/libstage1.c \
			$(src)/southbridge/amd/amd8111/stage1_smbus.c \
			$(src)/southbridge/amd/amd8111/stage1_ctrl.c \
			$(src)/southbridge/amd/amd8111/stage1_enable_rom.c \

INITRAM_SRC= $(src)/mainboard/$(MAINBOARDDIR)/initram.c \
			$(src)/northbridge/amd/k8/raminit.c \
			$(src)/northbridge/amd/k8/dqs.c \
			$(src)/northbridge/amd/k8/reset_test.c \
			$(src)/northbridge/amd/k8/coherent_ht.c \
			$(src)/northbridge/amd/k8/incoherent_ht.c \
			$(src)/arch/x86/pci_ops_conf1.c \
			$(src)/arch/x86/stage1_mtrr.c \
			$(src)/southbridge/amd/amd8111/stage1_smbus.c \
			$(src)/arch/x86/amd/model_fxx/dualcore.c \
			$(src)/arch/x86/amd/model_fxx/fidvid.c \
			$(src)/arch/x86/amd/model_fxx/init_cpus.c \
			$(src)/lib/clog2.c


STAGE2_MAINBOARD_SRC = 

$(obj)/coreboot.vpd:
	$(Q)printf "  BUILD   DUMMY VPD\n"
	$(Q)dd if=/dev/zero of=$(obj)/coreboot.vpd bs=256 count=1 $(SILENT)

