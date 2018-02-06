#
# This file is part of the coreboot project.
#
# Copyright (C) 2018 Google, LLC.
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

bootblock-y += gpio.c
bootblock-y += OemCustomize.c

verstage-y += tpm_tis.c

romstage-y += gpio.c
romstage-y += memory.c
romstage-y += tpm_tis.c

ramstage-y += gpio.c
ramstage-y += tpm_tis.c
