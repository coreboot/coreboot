ifeq ($(CONFIG_VBOOT_VBNV_CMOS),y)
bootblock-y += vbnv_cmos.c
verstage-y += vbnv_cmos.c
romstage-y += vbnv_cmos.c
ramstage-y += vbnv_cmos.c
endif
