ifeq ($(CONFIG_SOUTHBRIDGE_VIA_K8T890),y)

ramstage-y += ctrl.c
ramstage-y += dram.c
ramstage-y += bridge.c
ramstage-y += host.c
ramstage-y += host_ctrl.c
ramstage-y += pcie.c
ramstage-y += traf_ctrl.c
ramstage-y += error.c
ramstage-y += chrome.c

bootblock-y += romstrap.ld

endif
