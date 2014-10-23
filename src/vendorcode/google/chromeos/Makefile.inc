##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

romstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
ramstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
romstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
ramstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += vboot.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += gnvs.c
romstage-y += fmap.c
ramstage-y += fmap.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
smm-y += fmap.c
romstage-y += vpd_decode.c cros_vpd.c
ramstage-y += vpd_decode.c cros_vpd.c vpd_mac.c vpd_calibration.c

ifeq ($(MOCK_TPM),1)
CFLAGS_common += -DMOCK_TPM=1
else
CFLAGS_common += -DMOCK_TPM=0
endif

VB_SOURCE := vboot_reference
subdirs-$(CONFIG_VBOOT_VERIFY_FIRMWARE) += vboot1
subdirs-$(CONFIG_VBOOT2_VERIFY_FIRMWARE) += vboot2
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/2lib/include
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/include
