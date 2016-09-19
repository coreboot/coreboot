ifeq ($(CONFIG_ARCH_X86),y)

romstage-$(CONFIG_DRIVERS_MC146818) += mc146818rtc_early.c
romstage-$(CONFIG_DRIVERS_MC146818) += mc146818rtc.c

ramstage-$(CONFIG_DRIVERS_MC146818) += mc146818rtc_early.c
ramstage-$(CONFIG_DRIVERS_MC146818) += mc146818rtc.c

cbfs-files-$(CONFIG_HAVE_CMOS_DEFAULT) += cmos.default
cmos.default-file = $(CONFIG_CMOS_DEFAULT_FILE):nvramtool
cmos.default-type = cmos_default

smm-$(CONFIG_DRIVERS_MC146818) += mc146818rtc.c

endif
