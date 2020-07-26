ramstage-$(CONFIG_CPU_INTEL_COMMON) += common_init.c
ramstage-$(CONFIG_CPU_INTEL_COMMON) += hyperthreading.c
ramstage-$(CONFIG_CPU_INTEL_COMMON_VOLTAGE) += voltage.c

ifeq ($(CONFIG_CPU_INTEL_COMMON_TIMEBASE),y)
bootblock-y += fsb.c
verstage-y += fsb.c
romstage-y += fsb.c
postcar-y += fsb.c
ramstage-y += fsb.c
smm-y += fsb.c
endif
