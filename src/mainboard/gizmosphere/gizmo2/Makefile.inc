# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += buildOpts.c
romstage-y += BiosCallOuts.c
romstage-y += OemCustomize.c

ramstage-y += buildOpts.c
ramstage-y += BiosCallOuts.c
ramstage-y += OemCustomize.c

# Order of names in SPD_SOURCES is important!
SPD_SOURCES  = Micron_MT41J128M16JT
