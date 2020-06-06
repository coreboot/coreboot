ifeq ($(CONFIG_ARCH_X86),y)

bootblock-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c
verstage-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c
postcar-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c
romstage-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c
ramstage-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c

bootblock-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c
postcar-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c
romstage-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c
ramstage-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c
smm-$(CONFIG_DRIVERS_MC146818)		+= mc146818rtc.c

bootblock-$(CONFIG_USE_OPTION_TABLE) += option.c
verstage-$(CONFIG_USE_OPTION_TABLE) += option.c
postcar-$(CONFIG_USE_OPTION_TABLE) += option.c
romstage-$(CONFIG_USE_OPTION_TABLE) += option.c
ramstage-$(CONFIG_USE_OPTION_TABLE) += option.c
smm-$(CONFIG_USE_OPTION_TABLE) += option.c

bootblock-$(CONFIG_CMOS_POST) += post.c
verstage-$(CONFIG_CMOS_POST) += post.c
postcar-$(CONFIG_CMOS_POST) += post.c
romstage-$(CONFIG_CMOS_POST) += post.c
ramstage-$(CONFIG_CMOS_POST) += post.c

ifeq ($(CONFIG_USE_OPTION_TABLE),y)
cbfs-files-$(CONFIG_HAVE_CMOS_DEFAULT) += cmos.default
cmos.default-file = $(CONFIG_CMOS_DEFAULT_FILE):nvramtool
cmos.default-type = cmos_default
endif

endif
