ifeq ($(CONFIG_ARCH_X86),y)

all-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc_boot.c

all-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c
smm-$(CONFIG_DRIVERS_MC146818)	+= mc146818rtc.c

all-$(CONFIG_USE_OPTION_TABLE)	+= option.c
smm-$(CONFIG_USE_OPTION_TABLE)	+= option.c

all-$(CONFIG_CMOS_POST)		+= post.c

ifeq ($(CONFIG_USE_OPTION_TABLE),y)
cbfs-files-$(CONFIG_HAVE_CMOS_DEFAULT) += cmos.default
cmos.default-file = $(CONFIG_CMOS_DEFAULT_FILE):nvramtool
cmos.default-type = cmos_default
endif

endif
