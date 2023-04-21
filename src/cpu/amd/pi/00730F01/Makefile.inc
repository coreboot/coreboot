# SPDX-License-Identifier: GPL-2.0-only

romstage-y += cpu_io_init.c

ramstage-y += chip_name.c
ramstage-y += cpu_io_init.c
ramstage-y += model_16_init.c
ramstage-y += update_microcode.c

subdirs-y += ../../mtrr
