#
# This file is part of the coreboot project.
#
#
# SPDX-License-Identifier: GPL-2.0-only

subdirs-y += ./spd

romstage-y += variant.c

ramstage-y += ../baseboard/mainboard.c
