driver-y += k8t890_ctrl.c
driver-y += k8t890_dram.c
driver-y += k8t890_bridge.c
driver-y += k8t890_host.c
driver-y += k8t890_host_ctrl.c
driver-y += k8t890_pcie.c
driver-y += k8t890_traf_ctrl.c
driver-y += k8t890_error.c
driver-y += k8m890_chrome.c

chipset_bootblock_inc += $(src)/southbridge/via/k8t890/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/via/k8t890/romstrap.lds
