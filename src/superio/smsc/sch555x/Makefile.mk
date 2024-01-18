# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SUPERIO_SMSC_SCH555x) += emi.c bootblock.c
ramstage-$(CONFIG_SUPERIO_SMSC_SCH555x) += ramstage.c
