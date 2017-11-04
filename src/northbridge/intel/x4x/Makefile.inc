#
# This file is part of the coreboot project.
#
# Copyright (C) 2012 secunet Security Networks AG
# Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_X4X),y)

romstage-y += early_init.c
romstage-y += raminit.c
romstage-y += raminit_ddr2.c
romstage-y += ram_calc.c
romstage-y += rcven.c
romstage-y += raminit_tables.c
romstage-y += dq_dqs.c

ramstage-y += acpi.c
ramstage-y += ram_calc.c
ramstage-y += gma.c
ramstage-y += northbridge.c

endif
