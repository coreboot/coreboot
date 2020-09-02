bootblock-$(CONFIG_ELOG) += elog.c
verstage-$(CONFIG_ELOG) += elog.c
romstage-$(CONFIG_ELOG) += elog.c
postcar-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_ELOG) += elog.c

smm-$(CONFIG_ELOG_GSMI) += elog.c gsmi.c

romstage-$(CONFIG_ELOG_BOOT_COUNT) += boot_count.c
ramstage-$(CONFIG_ELOG_BOOT_COUNT) += boot_count.c
