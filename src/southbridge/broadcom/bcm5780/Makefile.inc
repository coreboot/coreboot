ifeq ($(CONFIG_SOUTHBRIDGE_BROADCOM_BCM5870),y)

ramstage-y += nic.c
ramstage-y += pcix.c
ramstage-y += pcie.c

endif
