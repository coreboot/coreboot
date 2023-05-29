# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
verstage-y += gpio.c

romstage-y += tpm_tis.c
ramstage-y += tpm_tis.c
verstage-y += tpm_tis.c

bootblock-y += variant.c
romstage-y += variant.c
ramstage-y += variant.c

ramstage-y += ramstage.c

subdirs-y += ./memory

smm-y += gpio.c
