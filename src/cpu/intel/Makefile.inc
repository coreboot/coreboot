# Note: From here on down, we are socket-centric. Socket choice determines
# what other CPU files are included.
#
# Therefore: ONLY include Makefile.inc from socket directories!

subdirs-$(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE) += fit
subdirs-$(CONFIG_CPU_INTEL_SOCKET_441) += socket_441
subdirs-$(CONFIG_CPU_INTEL_SOCKET_BGA956) += socket_BGA956
subdirs-$(CONFIG_CPU_INTEL_SOCKET_BGA1284) += socket_BGA1284
subdirs-$(CONFIG_CPU_INTEL_SOCKET_FCBGA559) += socket_FCBGA559
subdirs-$(CONFIG_CPU_INTEL_SOCKET_FCBGA1023) += socket_FCBGA1023
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MFCPGA478) += socket_mFCPGA478
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA478MN) += socket_mPGA478MN
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA604) += socket_mPGA604
subdirs-$(CONFIG_CPU_INTEL_SOCKET_RPGA988B) += socket_rPGA988B
subdirs-$(CONFIG_CPU_INTEL_SOCKET_RPGA989) += socket_rPGA989
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_NEHALEM) += model_2065x
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_HASWELL) += haswell
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_FSP_SANDYBRIDGE) += fsp_model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_FSP_IVYBRIDGE) += fsp_model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_FSP_RANGELEY) += fsp_model_406dx
subdirs-$(CONFIG_CPU_INTEL_SLOT_1) += slot_1
subdirs-$(CONFIG_CPU_INTEL_SOCKET_LGA1155) += socket_LGA1155
subdirs-$(CONFIG_CPU_INTEL_SOCKET_LGA775) += socket_LGA775
