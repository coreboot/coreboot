# SPDX-License-Identifier: GPL-2.0-only

romstage-y += fixme.c

ramstage-y += fixme.c
ramstage-y += chip_name.c
ramstage-y += model_14_init.c

subdirs-y += ../../mtrr
subdirs-y += ../../../x86/lapic
subdirs-y += ../../../x86/cache
subdirs-y += ../../../x86/mtrr
subdirs-y += ../../../x86/pae
