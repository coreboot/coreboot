ifeq ($(CONFIG_CONSOLE_SERIAL),y)
romstage-y += util.c
ramstage-y += util.c
bootblock-y += util.c
smm-y += util.c
endif
