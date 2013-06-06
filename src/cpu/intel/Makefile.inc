# Note: From here on down, we are socket-centric. Socket choice determines
# what other cpu files are included.
#
# Therefore: ONLY include Makefile.inc from socket directories!

subdirs-$(CONFIG_CPU_INTEL_SOCKET_441) += socket_441
subdirs-$(CONFIG_CPU_INTEL_SOCKET_BGA956) += socket_BGA956
subdirs-$(CONFIG_CPU_INTEL_EP80579) += ep80579
subdirs-$(CONFIG_CPU_INTEL_SOCKET_FC_PGA370) += socket_FC_PGA370
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MFCBGA479) += socket_mFCBGA479
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MFCPGA478) += socket_mFCPGA478
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA478) += socket_mPGA478
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA479M) += socket_mPGA479M
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA603) += socket_mPGA603
subdirs-$(CONFIG_CPU_INTEL_SOCKET_MPGA604) += socket_mPGA604
subdirs-$(CONFIG_CPU_INTEL_SOCKET_PGA370) += socket_PGA370
subdirs-$(CONFIG_CPU_INTEL_SOCKET_RPGA989) += socket_rPGA989
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_NEHALEM) += model_2065x
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += model_206ax
subdirs-$(CONFIG_NORTHBRIDGE_INTEL_HASWELL) += haswell
subdirs-$(CONFIG_CPU_INTEL_SLOT_2) += slot_2
subdirs-$(CONFIG_CPU_INTEL_SLOT_1) += slot_1
subdirs-$(CONFIG_CPU_INTEL_SOCKET_LGA771) += socket_LGA771
subdirs-$(CONFIG_CPU_INTEL_SOCKET_LGA775) += socket_LGA775

#socket_mPGA604_533Mhz
#socket_mPGA604_800Mhz
