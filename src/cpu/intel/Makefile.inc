# Note: From here on down, we are socket-centric. Socket choice determines
# what other CPU files are included.
#
# Therefore: ONLY include Makefile.inc from socket directories!

subdirs-$(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE) += fit
subdirs-$(CONFIG_CPU_INTEL_SOCKET_441) += socket_441
subdirs-$(CONFIG_CPU_INTEL_SOCKET_BGA956) += socket_BGA956
subdirs-$(CONFIG_CPU_INTEL_SOCKET_FCBGA559) += socket_FCBGA559
subdirs-$(CONFIG_CPU_INTEL_SOCKET_M) += socket_m
subdirs-$(CONFIG_CPU_INTEL_SOCKET_P) += socket_p
subdirs-$(CONFIG_CPU_INTEL_MODEL_2065X) += model_2065x
subdirs-$(CONFIG_CPU_INTEL_MODEL_206AX) += model_206ax
subdirs-$(CONFIG_CPU_INTEL_HASWELL) += haswell
subdirs-$(CONFIG_CPU_INTEL_SLOT_1) += slot_1
subdirs-$(CONFIG_CPU_INTEL_SOCKET_LGA775) += socket_LGA775

subdirs-y += common
