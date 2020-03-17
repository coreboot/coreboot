#
# This file is part of the coreboot project.
#
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

ifeq ($(CONFIG_NORTHBRIDGE_AMD_PI),y)

subdirs-$(CONFIG_NORTHBRIDGE_AMD_PI_00630F01) += 00630F01
subdirs-$(CONFIG_NORTHBRIDGE_AMD_PI_00730F01) += 00730F01
subdirs-$(CONFIG_NORTHBRIDGE_AMD_PI_00660F01) += 00660F01

endif
