# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SUPERIO_SMSC_SCH5545) += sch5545_early_init.c
romstage-$(CONFIG_SUPERIO_SMSC_SCH5545) += sch5545_early_init.c

bootblock-$(CONFIG_SUPERIO_SMSC_SCH5545) += sch5545_emi.c
romstage-$(CONFIG_SUPERIO_SMSC_SCH5545) += sch5545_emi.c
ramstage-$(CONFIG_SUPERIO_SMSC_SCH5545) += sch5545_emi.c

ramstage-$(CONFIG_SUPERIO_SMSC_SCH5545) += superio.c
