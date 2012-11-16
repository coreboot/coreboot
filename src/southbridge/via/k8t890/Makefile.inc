ramstage-y += ctrl.c
ramstage-y += dram.c
ramstage-y += bridge.c
ramstage-y += host.c
ramstage-y += host_ctrl.c
ramstage-y += pcie.c
ramstage-y += traf_ctrl.c
ramstage-y += error.c
ramstage-y += chrome.c

chipset_bootblock_inc += $(src)/southbridge/via/k8t890/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/via/k8t890/romstrap.lds
