#
# This file is part of the coreboot project.
#
# Copyright (C) 2019 Intel Corporation
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

romstage-$(CONFIG_SOC_INTEL_TIGERLAKE) += fsp_params_tgl.c
romstage-$(CONFIG_SOC_INTEL_JASPERLAKE) += fsp_params_jsl.c
romstage-y += ../../../../cpu/intel/car/romstage.c
romstage-y += romstage.c
romstage-y += pch.c
romstage-y += systemagent.c
