## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)

romstage-y += romstage.c

ramstage-y += mainboard.c
ramstage-y += ec.c

smm-y += ec.c

subdirs-y += variants/$(VARIANT_DIR)
