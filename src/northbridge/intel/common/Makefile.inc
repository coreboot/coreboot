##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Alexander Couzens <lynxis@fe80.eu>
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

romstage-$(CONFIG_NORTHBRIDGE_INTEL_COMMON_MRC_CACHE) += mrc_cache.c
ramstage-$(CONFIG_NORTHBRIDGE_INTEL_COMMON_MRC_CACHE) += mrc_cache.c
ramstage-$(CONFIG_NORTHBRIDGE_INTEL_COMMON_GMA_OPREGION) += gma_opregion.c
