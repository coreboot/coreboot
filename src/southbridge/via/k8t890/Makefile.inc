driver-y += ctrl.c
driver-y += dram.c
driver-y += bridge.c
driver-y += host.c
driver-y += host_ctrl.c
driver-y += pcie.c
driver-y += traf_ctrl.c
driver-y += error.c
driver-y += chrome.c

chipset_bootblock_inc += $(src)/southbridge/via/k8t890/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/via/k8t890/romstrap.lds
