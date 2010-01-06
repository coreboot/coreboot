driver-y += k8t890_ctrl.o
driver-y += k8t890_dram.o
driver-y += k8t890_bridge.o
driver-y += k8t890_host.o
driver-y += k8t890_host_ctrl.o
driver-y += k8t890_pcie.o
driver-y += k8t890_traf_ctrl.o
driver-y += k8t890_error.o
driver-y += k8m890_chrome.o

chipset_bootblock_inc += $(src)/southbridge/via/k8t890/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/via/k8t890/romstrap.lds
