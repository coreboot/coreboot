ramstage-$(CONFIG_ELOG) += elog.c

smm-$(CONFIG_ELOG_GSMI) += elog.c gsmi.c

romstage-$(CONFIG_ELOG_BOOT_COUNT) += boot_count.c
ramstage-$(CONFIG_ELOG_BOOT_COUNT) += boot_count.c
